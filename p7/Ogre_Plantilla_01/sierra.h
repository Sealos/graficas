#include "obstacle.h"

class Sierra : public Obstacle{
public:
	Quaternion orientacion;
	Real vMov;

	Sierra(){};
	Sierra(Real,SceneNode*,Vector3,  Real);
	~Sierra(){};
	void onUpdate(Real);
};