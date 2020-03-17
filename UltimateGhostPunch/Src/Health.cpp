#include "Health.h"
#include <sstream>
#include "GameObject.h"

#include "GhostManager.h"

Health::Health(GameObject* gameObject) : UserComponent(gameObject)
{

}

Health::~Health()
{

}

void Health::start()
{
	alive = true;
	invencible = false;

	ghost = gameObject->getComponent<GhostManager>();
}

void Health::handleData(ComponentData* data)
{
	for (auto prop : data->getProperties()) {
		std::stringstream ss(prop.second);

		if (prop.first == "health") {
			int health; ss >> health;
			this->health = health;
		}
		else if (prop.first == "resHealth") {
			int resHealth; ss >> resHealth;
			this->resurrectionHealth = resHealth;
		}
		else if (prop.first == "invTime") {
			float invTime; ss >> invTime;
			this->invencibleTime = invTime;
		}
		else
		{
			printf("HEALTH: Invalid property name \"%s\"", prop.first.c_str());
		}
	}
}

void Health::receiveDamage(int damage)
{
	if ((ghost != nullptr && ghost->isGhost()) || invencible)
		return;

	// update UI health

	health -= damage;
	if (health <= 0)
	{
		if(ghost != nullptr && ghost->hasGhost())
			ghost->activateGhost(); 
		else
			die();
	}
}

void Health::die()
{
	alive = false;

	// update UI

	// deactivate gameObject
	gameObject->setActive(false);

	// kick the player out of the game (?)
	//...
	// save info to show in the winner screen (position of the podium, kills, etc.) (?)
	//...
}

void Health::resurrect()
{
	health = resurrectionHealth;

	// activate invencibility for a specified time
	invencible = true;
	//invencibleTimer->start() (???)
}

int Health::getHealth()
{
	return health;
}

void Health::setHealth(int health)
{
	this->health = health;
}

bool Health::isAlive()
{
	return alive;
}

bool Health::isInvencible()
{
	return invencible;
}