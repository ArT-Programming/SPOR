#include "allocore/math/al_Vec.hpp"

namespace al{

// A turtle in two or three dimensions
template <unsigned Nstack=8>
class Turtle{
public:

	Turtle(): mStackIdx(0){}

	// Get current position
	Vec3d& pos(){ return state().pos; }
	const Vec3d& pos() const { return state().pos; }

	// Get previous position
	Vec3d& posPrev(){ return state().posPrev; }
	const Vec3d& posPrev() const { return state().posPrev; }

	// Get right direction vector
	Vec3d& ur(){ return state().ur; }
	const Vec3d& ur() const { return state().ur; }

	// Get forward direction vector
	Vec3d& uf(){ return state().uf; }
	const Vec3d& uf() const { return state().uf; }

	// Get up direction vector
	Vec3d& uu(){ return state().uu; }
	const Vec3d& uu() const { return state().uu; }

	// Move forward by some amount
	void move(double by){
		posPrev() = pos();
		pos() += state().uf*by;
	}
	
	// Turn by some angle, in degrees, according to right-handed convention
	void turn(double degrees, int turnAxis=2){
		turnRad(degrees * M_PI/180., turnAxis);
	}

	// Turn by some angle, in radians, according to right-handed convention
	void turnRad(double radians, int turnAxis=2){
		double re = cos(radians);
		double im = sin(radians);
		int e0 = (turnAxis+1)%3;
		int e1 = (turnAxis+2)%3;
		Vec3d u0 = state().dir(e0);
		Vec3d u1 = state().dir(e1);
		state().dir(e0) = u0*re + u1*im;
		state().dir(e1) = u1*re - u0*im;
	}

	// Reset back to origin pointing up the y axis
	void identity(){ state().identity(); }

	// Push current state onto stack
	void push(){
		if(mStackIdx < (Nstack-1)){
			mStack[mStackIdx+1] = mStack[mStackIdx];
			++mStackIdx;
		}
		else{
			fprintf(stderr, "Turtle: stack overflow (size=%d)\n", Nstack);
		}
	}
	
	// Pop current state off stack
	void pop(){
		if(mStackIdx != 0){
			--mStackIdx;
		}
	}

private:
	struct State{
		Vec3d pos, posPrev;
		Vec3d ur, uf, uu;
		State(){ identity(); }
		void identity(){
			pos.set(0,0,0);
			posPrev.set(0,0,0);
			ur.set(1,0,0);
			uf.set(0,1,0);
			uu.set(0,0,1);
		}
		Vec3d& dir(int axis){
			return (&ur)[axis];
		}
	};
	
	State mStack[Nstack];
	unsigned mStackIdx;
	
	State& state(){ return mStack[mStackIdx]; }
};

}
