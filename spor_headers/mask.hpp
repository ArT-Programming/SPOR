#include "allocore/io/al_App.hpp"
#include "math.h"

using namespace al;

/// -------------------------------------------------------------------------
/// --------------------------- Mask Class ----------------------------------
/// -------------------------------------------------------------------------

class Mask : public App{
public:
	
	Mesh black, white, window;
	Texture texture;
	
	Vec3f pos, fromPos, scale;
	
	float blackColor, whiteColor;
	int whiteVertex, winVertex;
	bool show;
	
	Mask():
	blackColor(0), whiteColor(1),
	whiteVertex(0), winVertex(0),
	show(false)
	{
		// Set all mesh primitives as QUADS
		black.primitive(Graphics::QUADS);
		white.primitive(Graphics::TRIANGLE_FAN);
		window.primitive(Graphics::QUADS);
		
		/*
		// Use this to reload a mask already made that was saved as a screenshot
		Image image;
		image.load(RUN_MAIN_SOURCE_PATH "CP_headers/white.jpg");

		// Copy pixels from image to texture
		texture.allocate(image.array());
		*/
		
		// Create a black background quad
		black.vertex(-2,-2, -0.001);
		black.vertex( 2,-2, -0.001);
		black.vertex( 2, 2, -0.001);
		black.vertex(-2, 2, -0.001);
		black.color(RGB(blackColor));
		
		white.vertex(0,0,0);
		white.color(RGB(whiteColor));
		for(int i = 0; i < 100; i++){
			float t = 2 * M_PI * i / 99.;
			float x = cos(t);
			float y = sin(t);
			white.vertex(x,y,0);
		}
		scale = 1;
		pos = fromPos = 0;
	}
	
	void lockWhiteVertex(const ViewpointWindow& w, const Mouse& m, Vec3d cam){		
		fromPos = getMousePos(w, m, cam);
	}
	
	void setWhiteVertex(const ViewpointWindow& w, const Mouse& m, Vec3d cam){		
		Vec3f toPos = getMousePos(w, m, cam);
		pos += toPos - fromPos;
		fromPos = toPos;
	}
	
	// Function to get the mouse coordinates in the z plane 0
	Vec3f getMousePos(const ViewpointWindow& w, const Mouse& m, Vec3d cam){
		// We have narrowed the camera lens constant down to be 0.26795
		float cX = cam.z * 0.26795 * w.aspect();
		float cY = cam.z * 0.26795;
		
		// Convert the pixel values to coordinates
		float mouseX = ( (2. * cX / (cX - cam.x)) * m.x() / w.width()  - 1) * (cX - cam.x);
		float mouseY = (-(2. * cY / (cY + cam.y)) * m.y() / w.height() + 1) * (cY + cam.y);
	
		return Vec3f(mouseX, mouseY, 0);
	}
	
	// Function to show the mask while modifying it
	void doShow(Graphics& g, const Viewpoint& v){
		// Draw all meshes
		g.draw(black);
		g.draw(window);
		
		g.pushMatrix(Graphics::MODELVIEW);
			g.translate(pos.x,pos.y);
			g.scale(scale.x, scale.y);
			g.draw(white);
		g.popMatrix();
		
		// Store the framebuffer as a texture to be able to use the texture as the actual mask
		texture.bind();
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 0,0, v.viewport().w, v.viewport().h, 0);
		texture.unbind();
	}
	
	// Function to draw the mask
	void draw(Graphics& g, const Viewpoint& v, Vec3d cam){
		// Get the coordinates of the window
		double width = 2 * cam.z * 0.26795 * v.viewport().aspect(); 
		double height = 2 * cam.z * 0.26795;
		
		// Use multiply blending to hide everything on black and reveal everything on white
		g.blendMul();
			g.pushMatrix(Graphics::MODELVIEW);
				// Make sure the mask is in front by translating it forward and scale up
				g.translate(0,0,cam.z/4);
				g.scale(3./4);
				g.color(RGB(1)); // The mask must be drawn on a white surface
				// Draw the mask on a full window quad
				texture.quad(g, width, height, cam.x - width / 2., cam.y - height / 2);
			g.popMatrix();
		g.blendOff();
	}
};
