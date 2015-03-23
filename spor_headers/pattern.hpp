#include "allocore/io/al_App.hpp"
#include "Turtle.hpp"

using namespace al;

/// -------------------------------------------------------------------------
/// ---------------------------- Pattern 1 ----------------------------------
/// -------------------------------------------------------------------------

class Pattern{
public:
	Mesh pattern;
	Vec2f position; // Pixel position
	Turtle<> turtle; // Use turtle graphics to create the pattern
	float a; // Variable used to animate the pattern
	
	// Initialize the variables and pattern primitive
	Pattern(){
		pattern.primitive(Graphics::TRIANGLE_STRIP);
		position = 0;
		a = 10;
	}
	
	// Function to reset the pattern before animating
	void reset(){
		turtle.pos() = Vec3f(0,0,0);
		pattern.reset();
		pattern.vertex(turtle.pos());
		a = 10;
	}
	
	// Function to animate the pattern
	void animate(float input){
		int direction = 83; // Degrees
		
		/*int N = 50;
		for(int i=0; i<N; i++){
			a += input;
			float frac = float(i)/N;

			// Move and turn turtle
			turtle.move(0.1 * input);
			turtle.turn(frac * 720, 2); // Turn around the z axis

			if((i % 32) == 0){*/
				for(int j=0; j<7; ++j){
					pattern.vertex(turtle.pos());
					// Move and turn turtle
					turtle.move(a * input);
					turtle.turn(direction, 0); // Turn around the x axis
				}
			//}
		//}
	}
	
	// Function to draw the pattern
	void draw(Graphics& g, float scale){
		g.pushMatrix(Graphics::MODELVIEW);
			g.stroke(2);
			g.translate(position.x,position.y);
			g.scale(0.1*scale);
			g.draw(pattern);
		g.popMatrix();
	}
};

/// -------------------------------------------------------------------------
/// ------------------------ Lightning pattern ------------------------------
/// -------------------------------------------------------------------------

class Lightning{
public:
	Turtle<> turtle; // Use turtle graphics
	Mesh line;
	Vec3f startPos;
	Vec2f position;
	
	// Initialize the variables and mesh primitive
	Lightning(){
		line.primitive(Graphics::LINE_STRIP);
		startPos = 0;
		turtle.pos() = startPos;
		position = 0;
	}
	
	// Reset the pattern
	void reset(){
		line.reset();
		turtle.pos() = startPos;
		line.vertex(turtle.pos());
	}
	
	// Animate the pattern
	void go(){
		for(int i = 0; i < 10; i++){
			// Move and turn the turtle randomly away from the centre
			int t = rand() % 100 - 50;
			float dist = rand() % 10 / 100.0;
			turtle.move(dist);
			turtle.turn(t);

			// Add turtle's position to mesh
			line.vertex(turtle.pos());
		}
	}
	
	// Draw the pattern at the pixel
	void draw(Graphics& g, float scale){
		g.pushMatrix(Graphics::MODELVIEW);
			g.translate(position.x,position.y);
			g.scale(.1*scale);
			g.draw(line);
		g.popMatrix();
	}
};

/// -------------------------------------------------------------------------
/// --------------------------- Web pattern ---------------------------------
/// -------------------------------------------------------------------------

class Web{
public:
	Mesh line;
	Mesh dots;
	Vec2f position;
	
	// Initialize the variables and mesh primitives
	Web(){
		dots.primitive(Graphics::POINTS);
		line.primitive(Graphics::LINES);
	}
	
	// Reset the pattern
	void reset(){
		line.reset();
		dots.reset();
	}
	
	//Animate the pattern
	void go(Video& vid, Vec3d cam, float aspect, Vec3f scale, Vec3f pos){
		// Create a web connecting up to 1000 of the changed pixels
		for(int i = 0; i < 1000; i++){
			// Random number between 0 and the amount of pixels in the video stream
			int random = rand() % vid.size();
			if(vid.pixelOn[random]){
				// Calculate the pixel position
				double width = 2 * cam.z * 0.26795 * aspect; 
				double height = 2 * cam.z * 0.26795;
				Vec2f pix;
				pix.x = pos.x + (width  * scale.x * (random % vid.Nx) / float(vid.Nx)) - (width  * scale.x / 2.);
				pix.y = pos.y + (height * scale.y * (random / vid.Nx) / float(vid.Ny)) - (height * scale.y / 2.);
				Vec2f win = Vec2f(width  * scale.x / 2. ,height * scale.y / 2.);
				
				//if(pix.x <= win.x && pix.x >= -win.x && pix.y <= win.y && pix.y >= -win.y){
				// Create vertices for the web
				line.vertex(pix.x, pix.y);
				dots.vertex(pix.x, pix.y);
				//}
			}
		}
	}
	
	// Draw function
	void draw(Graphics& g, float scale){
		g.stroke(7*scale);
		g.draw(dots);
		//Only draw one out of ten lines
		if(rand() % 10 == 0){
			g.stroke(2*scale);
			g.draw(line);
		}
	}
};

/// -------------------------------------------------------------------------
/// --------------------------- Atom pattern --------------------------------
/// -------------------------------------------------------------------------

class Atom{
public:
	Mesh pattern;
	Vec2f position; // Pixel position
	Turtle<> turtle;
	
	// Initialize variable and mesh primitive
	Atom(){
		pattern.primitive(Graphics::LINE_STRIP);
		position = 0;
	}
	
	// Reset function
	void reset(){
		turtle.pos() = Vec3f(0,0,0);
		pattern.reset();
		pattern.vertex(turtle.pos());
	}
	
	// Animate the pattern
	void animate(){
		int direction = 17; // Degrees
		int N = 50;
		
		// Create 100 vertices turning in random directions
		for(int j=0; j<N; ++j){
			int axis = rand() % 2;
			pattern.vertex(turtle.pos());
			turtle.move(0.001);
			turtle.turn(direction,axis * 2);
		}
	}
	
	// Draw function
	void draw(Graphics& g, float scale){
		g.pushMatrix(Graphics::MODELVIEW);
			g.stroke(6);
			g.scale(scale);
			g.translate(position.x,position.y);
			g.draw(pattern);
		g.popMatrix();
	}
};

