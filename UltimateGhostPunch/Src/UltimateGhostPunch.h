#pragma once
#ifndef GHOSTPUNCH_H
#define GHOSTPUNCH_H

#include <UserComponent.h>

class RigidBody;
class GameObject;

enum State {AVAILABLE, CHARGING, PUNCHING, USED};

class UltimateGhostPunch : public UserComponent
{
private:
	RigidBody* body;
	Vector3 dir = { 0,0,0 };
	State state;
	float duration = 0, force = 0;

public:
	UltimateGhostPunch(GameObject* gameObject);

	virtual void update(float deltaTime);
	virtual void start();

	virtual void charge();
	virtual void aim(double mousePosX, double mousePosY);
	void ghostPunch();

	State getState();
	const Vector3& getDir();

	virtual void handleData(ComponentData* data);
};

#endif