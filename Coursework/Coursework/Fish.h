#pragma once
#include "DXF.h"
using namespace std;
using namespace DirectX;


//billboarded fish class
class Fish {
public:

	//variables public for simple use
	XMFLOAT3 currentLocation;
	XMFLOAT3 destination;

	float speed = 3.0f;
	float Yrotation;

	//update function - controls fish movement as well as billboarding angle
	void update(XMFLOAT3 camLocation, float dt) {

		//update move towards the target position

		if (abs(currentLocation.x - destination.x) > 1) {
			currentLocation.x += dt * speed * (abs(destination.x - currentLocation.x) / (destination.x - currentLocation.x));
		}
		if (abs(currentLocation.y - destination.y) > 1) {
			currentLocation.y += dt * speed * (abs(destination.y - currentLocation.y) / (destination.y - currentLocation.y));
		}
		if (abs(currentLocation.z - destination.z) > 1) {
			currentLocation.z += dt * speed * (abs(destination.z - currentLocation.z) / (destination.z - currentLocation.z));
		}

		//update rotation - billboarding
		XMFLOAT2 faceDirV = XMFLOAT2(camLocation.x - currentLocation.x, camLocation.z - currentLocation.z);

		Yrotation = atan(faceDirV.x / faceDirV.y);

		if (faceDirV.y > 0) {
			Yrotation += 3.14;
		}

		//if close to the target destination, set a new target
		if (abs(currentLocation.x - destination.x) + abs(currentLocation.y - destination.y) + abs(currentLocation.z - destination.z) < 5) {
			destination.x = 5 + rand() % 90;
			destination.y = 3 + rand() % 20;
			destination.z = 5 + rand() % 90;

			destination.y *= -1;

		}
	}

	//setup function, spawns fish and their first destination
	void setup() {
		destination.x = 5 + rand() % 90;
		destination.y = 3 + rand() % 20;
		destination.z = 5 + rand() % 90;

		currentLocation.x = 5 + rand() % 90;
		currentLocation.y = 3 + rand() % 20;
		currentLocation.z = 5 + rand() % 90;

		destination.y *= -1;
		currentLocation.y *= -1;
	}
};
