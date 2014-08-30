#ifndef DF_TWEEN
#define DF_TWEEN
#include "Util.h"
namespace df {
//:FIXME: 30.08.14 10:24:17, Hugues Ross
// This system is no longer supported, but has no adequate replacement. This must be rectified with an initial version of the animation game system ASAP
//enum tween_interp_type { TWEEN_INTERP_LINEAR = 0, TWEEN_INTERP_SMOOTH_START, TWEEN_INTERP_SMOOTH_END, TWEEN_INTERP_SMOOTH_BOTH, TWEEN_INTERP_BOUNCE };
//enum tween_end_type { TWEEN_END = 0, TWEEN_RESTART, TWEEN_RESTART_INDEF, TWEEN_REVERSE, TWEEN_REVERSE_INDEF };

//template <class T> class Tween {
//public:
	//Tween(void* ntarget, T ngoal, float ntime = 1, int it = TWEEN_INTERP_LINEAR, int et = TWEEN_END) {
		//target = (T*)ntarget;
		//interp_type = it;
		//end_type = et;
		//if(target == nullptr)
			//finished = true;
		//else {
			//start = *target;
			//goal = ngoal;
			//end_time = ntime;
		//}
	//}
	//void run(float delta_time) {
		//if(target == nullptr)
			//finished = true;
		//if(finished)
			//return;
		//current_time += delta_time / FPS;
		////std::cerr << delta_time << "\n";
		//if(current_time > 0){
			//switch(interp_type){
			//case TWEEN_INTERP_LINEAR:
				//*target = start + (current_time/end_time) * (goal - start);
				//break;
			//case TWEEN_INTERP_SMOOTH_START:
				//*target = start + (1 - cos(((current_time/end_time) * 90) * DEGTORAD)) * (goal - start);
				//break;
			//case TWEEN_INTERP_SMOOTH_END:
				//*target = start + (sin(((current_time/end_time) * 90) * DEGTORAD)) * (goal - start);
				//break;
			//case TWEEN_INTERP_SMOOTH_BOTH:
				//*target = start + ((1 - cos(((current_time/end_time) * 180) * DEGTORAD)) / 2) * (goal - start);
				//break;
			//case TWEEN_INTERP_BOUNCE:
				///[>target = start + (current_time/end_time) * (goal - start);
				//break;
			//}
		//}
		//if(current_time >= end_time){
			//*target = goal;
			//if(end_type == TWEEN_END)
				//finished = true;
			//else if(end_type == TWEEN_RESTART || end_type == TWEEN_RESTART_INDEF)
				//current_time = 0;
			//else if(end_type == TWEEN_REVERSE || end_type == TWEEN_REVERSE_INDEF){
				//current_time = 0;
				//T temp = start;
				//start = goal;
				//goal = temp;
				//[>if(interp_type == TWEEN_INTERP_SMOOTH_START)
					//interp_type = TWEEN_INTERP_SMOOTH_END;
				//else if(interp_type == TWEEN_INTERP_SMOOTH_END)
					//interp_type = TWEEN_INTERP_SMOOTH_START;*/
			//}
			
			//if(end_type == TWEEN_RESTART || end_type == TWEEN_REVERSE)
				//end_type = TWEEN_END;
		//}
	//}
	//float current_time = 0;
	//float end_time = 1;
	//T goal;
	//T start;
	//int interp_type = TWEEN_INTERP_LINEAR;
	//int end_type = TWEEN_END;
	//bool finished = false;
//protected:
	//T* target;
//};
}
#endif
