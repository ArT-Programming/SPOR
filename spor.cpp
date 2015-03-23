/*
  	  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 ~~~~~~~~~~~~~~~~~~ S P O R ~~~~~~~~~~~~~~~~~~ 
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 	  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

Aalborg University
Art & Technology
3rd Semester, Fall 2014
Group 2 (Cardiod)

Group members: Frederik Gøeg, Grith Henriksen, Louise Wunderlich, Nikolaj Andersson, Alf Andersen
*/

#include "allocv/al_VideoCapture.hpp"
#include "allocore/io/al_App.hpp"
#include "spor_headers/video.hpp"
#include "spor_headers/pattern.hpp"
#include "spor_headers/mask.hpp"
#include <string>
#include <sstream>
#include <ctime>

using namespace al;

class CP : public App, public VideoCaptureHandler{
public:
	
	Video vid; // Video class from video.hpp
	
	Mask mask; // Mask class from mask.hpp
	
	Pattern pattern; // Pattern, web, Atom, and Lightning classes from pattern.hpp
	Web web;
	Atom atom;
	Lightning lightning[10];
	
	Texture lastFrame; // Texture to store frame buffer for access of the rendered frame on the next frame
	
	Vec3d cam; // nav().pos() will be stored here
	float time;
	bool maskWindows;
	bool firstFrame;
	bool takeScreenShots;
	float screenShotTime;
	double aspect;
	int patternCounter;
	double hue, hue1, hue2;
	bool hueOverload;
	
	int scaleWay; // Used to scale the circle or video higher lower wider thinner
	Vec3f scale, fromPos, pos;
	Vec3f winSize;
	int resX,resY;
	float scaleTime, scaleAmount;
	
	CP():
	scaleWay(-1), scaleTime(0), scaleAmount(0.1),
	vid(640,480, 1), // Constructor chaining: call vid's constructor within CP's constructor with vid(width, height, videoinput)
	time(0),
	maskWindows(false),
	firstFrame(true),
	takeScreenShots(false),
	screenShotTime(0),
	aspect(800/600.),
	patternCounter(0),
	hue(1/6.), hue1(1/6.), hue2(1/6.),
	hueOverload(false)
	{
		scale = 1;
		fromPos = pos = 0;
		winSize = Vec3f(2,2,0);
		resX = 800;
		resY = 600;
		
		// Attach video to VideoCaptureHandler so onVideo gets called
		attach(vid.vid);
		
		// Start video playback
		startVideo();
		
		nav().pos(0, 0, 4);
		cam = nav().pos();
		initWindow();
		window().remove(navControl());
		
		std::cout // Print instructions in the terminal on startup
		<<"\n      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~      \n"
		<<  "  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  \n"
		<<  " ~~~~~~~~~~~~~~~~~~ S P O R ~~~~~~~~~~~~~~~~~~ \n"
		<<  "  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  \n"
		<<  "      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~      \n"
		<<"Keyboard controls: \n"
		<<"'c' - Cursor            - on/off \n"
		<<"'s' - Show video        - on/off \n"
		<<"'t' - Take Screen shots - on/off \n"
		<<"'m' - Mask visible      - on/off \n"
		<<"'f' - First frame (clean black canvas) \n"
		<<"Arrow Keys - Scale the width and height of the mask and video \n";
	}

	// onVideo gets called whenever a new video frame is ready
	void onVideo(VideoCapture& vidcap, int streamIdx){
		if(!mask.show){
			if(vid.vidFrame(vidcap) && !vid.show){ // Copy video frame into texture; functions returns true on success
				// Analyze the video frame. This function takes thresholds for highlights and shadows as an unsigned int
				vid.getPixels(25,25,winSize,resX,resY,scale,pos,mask.texture); 
			}
		}
	}
	
	void onAnimate(double dt){
		scaleTime+=dt;
		
		if(scaleTime >= 1 && scaleWay != -1){
			scaleAmount*= 2;
			scaleTime -= 1;
			//std::cout<<"more!! \n";
		}
		
		switch(scaleWay){
		case 0: if(mask.show){mask.scale.y += scaleAmount * dt;} else{scale.y += scaleAmount * dt;} break;
		case 1: if(mask.show){mask.scale.y -= scaleAmount * dt;} else{scale.y -= scaleAmount * dt;} break;
		case 2: if(mask.show){mask.scale.x += scaleAmount * dt;} else{scale.x += scaleAmount * dt;} break;
		case 3: if(mask.show){mask.scale.x -= scaleAmount * dt;} else{scale.x -= scaleAmount * dt;} break;
		}
		
		
	
		if(takeScreenShots){ screenShotTime += dt; } // Add delta time to screenShotTime if screenshots are enabled
		
		time+= dt; // Add delta time to time
		
		if(time > 5){ // If 30 seconds has passed
			time-= 5;
			
			patternCounter++; // Go to next pattern
			if(patternCounter == 4){ patternCounter = 0; }
			
			hue1 = (int(100000*(hue+1/3.)) % 100000) / 100000.; // Go to next hue and make sure it is between 0 and 1
			
			if(hue1 < hue){ hueOverload = true; } // If hue1 wrapped around 0 then hueOverlaod is true
		}
		
		if(hue1 > hue){ // Add to hue as long as it hasn't reached the new value
			hue += 1/10. * dt;
		}else if(hue1 < hue && hueOverload){
			hue2 = hue;
			hue += 1/10. * dt;
			hue = (int(hue * 100000) % 100000) / 100000.;
			if(hue2 > hue){
				hueOverload = false;
			}
		}
		
		switch(patternCounter){ //call the functions from the current pattern to animate it
		case 0: pattern.reset(); pattern.animate(0.02); break;
		
		case 1: for(int i = 0; i < 10; i++){ lightning[i].reset(); lightning[i].go(); } break;
		
		case 2: web.reset(); web.go(vid, cam, aspect, scale, pos); break;
		
		case 3: atom.reset(); atom.animate(); break;
		}
	}
	
	void onDraw(Graphics& g, const Viewpoint& v){	
		aspect = v.viewport().aspect(); // Get the current aspect ratio
		// Get the dimension of the coordinates in z plane 0
		// We have narrowed the camera lens constant down to be 0.26795
		double width = 2 * cam.z * 0.26795 * aspect; 
		double height = 2 * cam.z * 0.26795;
		winSize = Vec3f(width, height, 0);
		resX = v.viewport().w;
		resY = v.viewport().h;
		
		if(mask.show){ 
			mask.doShow(g,v); // Show the mask
			// Take screenShot every 10 sec. if enabled
			if(screenShotTime >= 10 && takeScreenShots){
				screenShotTime -= 10;
				screenShot(v);
			}
		}
		else if(vid.show){	//Draw video
			g.color(RGB(1)); // Make sure the quad we draw on is white
			// Draw the video on a full window quad
			vid.getTexture().quad(g, width*scale.x, height*scale.y, pos.x + cam.x - width*scale.x / 2., pos.y + cam.y - height*scale.y / 2.);
			// Take screenShot every 10 sec. if enabled
			if(screenShotTime >= 10 && takeScreenShots){
				screenShotTime -= 10;
				screenShot(v);
			}
			mask.draw(g,v,cam); // Draw mask
		}
		else{	//Draw patterns
			
			// There will not be any texture saved to lastframe on the first frame, 
			// therefor only do this after the first draw has been done
			if(!firstFrame){ 
				// Turn off depth mask so that everything after renders over the previous frame
				g.depthMask(false);
			
				// These ajustments animate the previos frames to give various effects:
				double scaleX = 1.00015; // Scale horizontally
				double scaleY = 1.00015; // Scale vertically
				double moveX = 0; // Pixels move right (positive number) or left (negative number)
				double moveY = 0; // Pixels move up (positive number) or down (negative number)
				double rotate = 0; // Rotate the previous frame 
				float keepAmount = 0.97; // Between 0 (nothing is preserved) and 1 (fully exposed)
				
				// Draw the previos frame on a full window quad
				g.color(RGB(keepAmount));
				g.pushMatrix(Graphics::MODELVIEW);
				g.rotate(rotate);
				lastFrame.quad(g, width * scaleX, height * scaleY, moveX + cam.x - width * scaleX / 2., moveY + cam.y - height * scaleY / 2.);
				g.popMatrix();
				g.depthMask(true);
			}
			
			for(int idx = 0; idx < vid.size(); idx++){
				// Loop through all pixels in the video, and if a change has happened,
				// draw the current pattern there
				if(vid.pixelOn[idx]){
					int random = rand() % 10;
					bool didDraw = false;
					g.color(HSV(hue,1,1));
					Vec2f pix, win;
					pix.x = pos.x + (width  * scale.x * (idx % vid.Nx) / float(vid.Nx)) - (width  * scale.x / 2.);
					pix.y = pos.y + (height * scale.y * (idx / vid.Nx) / float(vid.Ny)) - (height * scale.y / 2.);
					win.x = width  / 2.;
					win.y = height / 2.;
					
					//if(pix.x <= win.x && pix.x >= -win.x && pix.y <= win.y && pix.y >= -win.y){
					switch(patternCounter){
					case 0:
						pattern.position.x = pix.x;
						pattern.position.y = pix.y;
						pattern.draw(g, scale.y);
						break;
				
					case 1:
					
						lightning[random].position.x = pix.x;
						lightning[random].position.y = pix.y;
						lightning[random].draw(g, scale.y);
						break;
					
					case 2:
						web.draw(g, scale.y);
						didDraw = true;
						break;
					
					case 3:
						//if(rand() % 3 == 0){
							atom.position.x = pix.x;
							atom.position.y = pix.y;
							atom.draw(g, scale.y);
						//}
						break;
					}
					if(didDraw){
						break;
					}
					//}
				}
			}
			
			// Copy current frame buffer to lastFrame texture
			lastFrame.bind();
			// OpenGL function
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 0,0, v.viewport().w, v.viewport().h, 0);
			lastFrame.unbind();
			firstFrame = false;
			
			// Take screenShot every 10 sec. if enabled
			if(screenShotTime >= 10 && takeScreenShots){
				screenShotTime -= 10;
				screenShot(v);
			}
			mask.draw(g,v,cam); // Draw mask
		}
	}
	
	// Use the mouse to modify the mask. See mask.hpp for info about the functions
	void onMouseDown(const ViewpointWindow& w, const Mouse& m){
		if(m.button() == 0 && mask.show){
			mask.lockWhiteVertex(w, m, cam);
			mask.setWhiteVertex(w, m, cam);
		}
		else if(m.button() == 0 && vid.show){
			fromPos = mask.getMousePos(w, m, cam);
		}
	}
	
	void onMouseDrag(const ViewpointWindow& w, const Mouse& m){
		if(m.button() == 0 && mask.show){
			mask.setWhiteVertex(w, m, cam);
		}
		else if(m.button() == 0 && vid.show){
			Vec3f toPos = mask.getMousePos(w, m, cam);
			pos += toPos - fromPos;
			fromPos = toPos;
		}
	}
	
	// Use keyboard commands as explained in the terminal
	void onKeyDown(const ViewpointWindow& w, const Keyboard& k){
		switch(k.key()){
		case 'c': window().cursorHideToggle(); break;
		
		case 's': vid.show = !vid.show; firstFrame = true; break;
		
		case 'f': firstFrame = true;
			for(int i = 0; i < vid.size(); i++){ vid.pixelOn[i] = false; } break;
			
		case 't': takeScreenShots = !takeScreenShots;
			std::cout<<"takeScreenShots: "<<takeScreenShots<<"\n";
			screenShotTime = 10; break;
			
		case 'm':	mask.show = !mask.show; break;
		
		case Keyboard::UP: scaleWay = 0; break;
		
		case Keyboard::DOWN: scaleWay = 1; break;
		
		case Keyboard::RIGHT: scaleWay = 2; break;
		
		case Keyboard::LEFT: scaleWay = 3; break;
		}
		scaleTime = 0;
		scaleAmount = 0.1;
	}
	
	void onKeyUp(const ViewpointWindow& w, const Keyboard& k){
		scaleWay = -1;
	}
	
	// Screenshot function
	void screenShot(const Viewpoint& v){
		unsigned w = v.viewport().w; // Get the height and width of the window
		unsigned h = v.viewport().h;
		unsigned char buffer[w * h * 3]; // Define a buffer big enough to handle RGB values of each pixel
		
		time_t t = std::time(0);   // Get time now in seconds since 00:00 01/01-1970
    struct tm * now = localtime( & t ); // Convert t to a struct of local time variables
		// Path to store screenshots (go back two steps because it saves by default in build/bin/)
		std::string path= "../../examples/screens/";
		
		//convert integers to strings. Source: http://www.cplusplus.com/articles/D9j2Nwbp/
		std::string year= static_cast<std::ostringstream*>( &(std::ostringstream() << now->tm_year + 1900) )->str();
		std::string mon = static_cast<std::ostringstream*>( &(std::ostringstream() << now->tm_mon  +    1) )->str();
		std::string day = static_cast<std::ostringstream*>( &(std::ostringstream() << now->tm_mday       ) )->str();
		std::string hour= static_cast<std::ostringstream*>( &(std::ostringstream() << now->tm_hour       ) )->str();
		std::string min = static_cast<std::ostringstream*>( &(std::ostringstream() << now->tm_min        ) )->str();
		std::string sec = static_cast<std::ostringstream*>( &(std::ostringstream() << now->tm_sec        ) )->str();
		
		// Add a zero in front of the string (number) in case it is less than 10
		if(now->tm_mon  < 10){ mon  = "0" + mon; }
		if(now->tm_mday < 10){ day  = "0" + day; }
		if(now->tm_hour < 10){ hour = "0" + hour;}
		if(now->tm_min  < 10){ min  = "0" + min; }
		if(now->tm_sec  < 10){ sec  = "0" + sec; }
		
		// Add the strings to get a final path and file name
		path = path+year+"-"+mon+"-"+day+"_"+hour+"."+min+"."+sec+".jpg";
		
		// Copy the frame buffer to a pixel array buffer
		glReadPixels(0,0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer);
		
		// Save the buffer as an image
		Image::write(path, buffer, w, h, Image::RGB, 100);
	}
};


int main(){
	CP().start(); // Start the application
}
