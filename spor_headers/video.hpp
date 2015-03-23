#include "allocore/io/al_App.hpp"
#include "math.h"

using namespace al;

/// -------------------------------------------------------------------------
/// --------------------------- Video Class ---------------------------------
/// -------------------------------------------------------------------------
class Video{
public:
	VideoCapture vid;
	Texture texture[2]; // Two textures to compare between
	int Nx, Ny; // Video size
	int frame; // Will be used to constantly switch between texture[0] and texture [1]
	bool* pixelOn; // Pointer because video size is yet unknown. Will be true if the pixel changed
	bool show; // Show video
	bool first; // First frame
	
	// Initialize the video size nx*ny and variables
	Video(const int nx,const int ny, int input):
	Nx(nx), Ny(ny), 
	pixelOn(new bool[nx*ny]),
	frame(1),
	show(true),
	first(true)
	{
		// Open video input
		vid.open(input);
		
		vid.set(CV_CAP_PROP_FRAME_WIDTH, Nx);
		vid.set(CV_CAP_PROP_FRAME_HEIGHT, Ny);
		
		// Print resolution and frame rate so errors can be avoided manually
		vid.print();
		
		for(int i = 0; i < Nx * Ny; i++){
			pixelOn[i] = false;
		}
		texture[0].format(Graphics::BGR);
		texture[1].format(Graphics::BGR);
	}
	
	// Free the pointer from memory when the video class gets deleted
	~Video(){
		delete pixelOn;
	}
	
	// Copy video frame into texture and return true on success
	bool vidFrame(VideoCapture& vidcap){
		return vidcap.retrieveFlip(texture[frame].array());
	}
	
	// Update which pixels changed
	void getPixels(unsigned highlights, unsigned shadows, Vec3f winSize, int resX, int resY, Vec3f scale, Vec3f pos, Texture mask){
		
		// Create texture buffers for this frame and the one before
		unsigned char * texBufNow = texture[frame].data<unsigned char>();
		unsigned char * texBufBef;
		
		// On first frame make before the same frame as the current
		if(first){
			first = false;
			texBufBef = texture[frame].data<unsigned char>();
		}else{
			texBufBef = texture[!frame].data<unsigned char>();
		}
		
		// Loop through all pixels
		for(int j=0; j<Ny; j++){
			for(int i=0; i<Nx; i++){
				int idx = j*Nx + i; // Calculate index number
				/*
				Vec2f coord;
				coord.x = pos.x + (winSize.x  * scale.x * (i/float(Nx))) - (winSize.x  * scale.x / 2.);
				coord.y = pos.y + (winSize.y * scale.y * (j/float(Ny))) - (winSize.y * scale.y / 2.);
				
					*/
						/*, Vec3f maskScale, Vec3f maskPos
						Vec3f d = pix - maskPos;
				
						float radius = sqrt(d.x*d.x + d.y*d.y);
				
						float angle = atan2(d.y , d.x);
				
						Vec3f circlePoint;
						circlePoint.x = maskScale.x * cos(angle);
						circlePoint.y = maskScale.y * cos(angle);
				
						float maxRadius = */
				
				
				
						// Store how many values each pixel contains
						int stride = texture[frame].numComponents();
				
						// k=0:Red; k=1:Green; k=2:Blue
						for(int k = 0; k < 3; k++){
							unsigned char beforeHigh = 255;
							unsigned char beforeLow = 0;
					
							// The current frame pixel must brighter than before + highlights threshold
							if(texBufBef[idx*stride + k] > 255 - highlights){
								beforeHigh = 255;
							} else {
								beforeHigh = texBufBef[idx*stride + k] + highlights;
							}
							// or
							// The current frame pixel must darker than before - shadows threshold
							if(texBufBef[idx*stride + k] < shadows){
								beforeLow = 0;
							} else {
								beforeLow = texBufBef[idx*stride + k] - shadows;
							}
						
							// pixelOn = true if brighter or darker
							if(	texBufNow[idx*stride + k] > beforeHigh
							||	texBufNow[idx*stride + k] < beforeLow
							){
							pixelOn[idx] = true;
							//std::cout<<Vec2i(i,j)<<"           True          1 \n";
							}else{
								pixelOn[idx] = false;
								//std::cout<<Vec2i(i,j)<<"      False              2 \n";
							}
						}
					/*}
					else{
						pixelOn[idx] = false;
						//std::cout<<Vec2i(i,j)<<"      False                   3 \n";
					}
				}*/
			}
		}
		texture[frame].dirty();
		// Toggle which texture is now and before
		frame = !frame;
	}
	
	// Video pixels
	int size(){
		return Nx * Ny;
	}
	
	// Get the current video texture
	Texture getTexture(){
		return texture[frame];
	}
	
	
};
