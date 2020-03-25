#pragma once

#include "UserComponent.h"

class GhostManager;
class PlayerUI;

class Health : public UserComponent
{
public:
	Health(GameObject* gameObject);
	virtual ~Health();

	virtual void start();
	virtual void update(float deltaTime);
	virtual void handleData(ComponentData* data);

	int getHealth();
	int getMaxHealth();
	void setHealth(int health);
	bool isAlive();
	bool isInvencible();

	void receiveDamage(int damage);
	void resurrect();
	void die();

private:
	int maxHealth;
	int health; // 1 life = 2 health points
	int resurrectionHealth;

	float time = 0.0f;
	float invencibleResurrectionTime;
	float invencibleDamageTime = 0.2f;

	bool alive; // player alive

	bool invencible;

	GhostManager* ghost;
	PlayerUI* playerUI;
};