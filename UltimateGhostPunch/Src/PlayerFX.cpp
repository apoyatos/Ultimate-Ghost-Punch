#include "PlayerFX.h"
#include <ComponentRegister.h>
#include <GameObject.h>
#include <sstream>

#include "MeshRenderer.h"
#include "Health.h"
#include "GhostManager.h"

REGISTER_FACTORY(PlayerFX);

PlayerFX::PlayerFX(GameObject* gameObject) : UserComponent(gameObject)
{

}

PlayerFX::~PlayerFX()
{

}

void PlayerFX::start()
{
	mesh = gameObject->getComponent<MeshRenderer>();
	health = gameObject->getComponent<Health>();
	ghost = gameObject->getComponent<GhostManager>();

	for (int i = 0; i < mesh->getSubentitiesSize(); i++)
		textureNames.push_back(mesh->getTexture(i));

	for (int i = 0; i < mesh->getSubentitiesSize(); i++)
		diffuses.push_back(mesh->getDiffuse(i));

	gameObject->findChildrenWithTag("shield")[0]->getComponent<MeshRenderer>()->setVisible(false);

	time = 0.0f;
	hurtTime = 0.5f;
	invencibleFrec = 0.1f;
	ghostFXFrec = 0.1f;
	ghostFXTime = 2.0f;

	frecuency = invencibleFrec;
	effect = NONE;
}

void PlayerFX::update(float deltaTime)
{
	if (time > 0.0f)
		time -= deltaTime;
	else
		deactivateHurt();

	if (health->isInvencible())
	{
		effect = INVENCIBLE;

		if (frecuency > 0.0f)
		{
			frecuency -= deltaTime;
			if (frecuency <= 0.0f)
			{
				activateInvencible();
				frecuency = -invencibleFrec;
			}
		}
		else
		{
			frecuency += deltaTime;
			if (frecuency >= 0.0f)
			{
				deactivateInvencible();
				frecuency = invencibleFrec;
			}
		}
	}
	else if (effect == INVENCIBLE)
	{
		deactivateInvencible();
	}

	if (ghost->isGhost() && ghost->getGhostTime() < ghostFXTime)
	{
		effect = GHOST;

		if (frecuency > 0.0f)
		{
			frecuency -= deltaTime;
			if (frecuency <= 0.0f)
			{
				activateGhostFX();
				frecuency = -invencibleFrec;
			}
		}
		else
		{
			frecuency += deltaTime;
			if (frecuency >= 0.0f)
			{
				deactivateGhostFX();
				frecuency = invencibleFrec;
			}
		}
	}
	else if (effect == GHOST)
	{
		deactivateGhostFX();
	}
}

void PlayerFX::handleData(ComponentData* data)
{
	for (auto prop : data->getProperties())
	{
		std::stringstream ss(prop.second);

		if (prop.first == "hurtTime") {
			if (!(ss >> hurtTime))
				LOG("PLAYERFX: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "invencibleFrec") {
			if (!(ss >> invencibleFrec))
				LOG("PLAYERFX: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "ghostFXFrec") {
			if (!(ss >> ghostFXFrec))
				LOG("PLAYERFX: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "ghostFXTime") {
			if (!(ss >> ghostFXTime))
				LOG("PLAYERFX: Invalid property with name \"%s\"", prop.first.c_str());
		}
	}
}

void PlayerFX::activateHurt()
{
	time = hurtTime;
	//effect = HURT;

	for (int i = 0; i < mesh->getSubentitiesSize(); i++)
		mesh->setDiffuse(i, { 1,0,0 }, 1);
}

void PlayerFX::deactivateHurt()
{
	effect = NONE;

	for (int i = 0; i < mesh->getSubentitiesSize(); i++)
		mesh->setDiffuse(i, diffuses[i], 1);
}

void PlayerFX::activateInvencible()
{
	for (int i = 0; i < mesh->getSubentitiesSize(); i++) mesh->setDiffuse(i, { 1,1,1 }, 1);
}

void PlayerFX::deactivateInvencible()
{
	//effect = NONE;

	for (int i = 0; i < mesh->getSubentitiesSize(); i++) mesh->setDiffuse(i, diffuses[i], 1);
}

void PlayerFX::activateGhostFX()
{
	mesh->setVisible(false);
}

void PlayerFX::deactivateGhostFX()
{
	//effect = NONE;

	mesh->setVisible(true);
}

void PlayerFX::activateShield()
{
	effect = SHIELD;

	gameObject->findChildrenWithTag("shield")[0]->getComponent<MeshRenderer>()->setVisible(true);
}

void PlayerFX::deactivateShield()
{
	effect = NONE;

	gameObject->findChildrenWithTag("shield")[0]->getComponent<MeshRenderer>()->setVisible(false);
}
