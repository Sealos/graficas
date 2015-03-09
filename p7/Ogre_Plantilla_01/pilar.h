#include "obstacle.h"

class Pilar : public Obstacle{
public:
	Real vMov;

	Pilar(){};
	Pilar(Real,SceneNode*, Vector3,Real);
	~Pilar(){};
	void onUpdate(Real);


};