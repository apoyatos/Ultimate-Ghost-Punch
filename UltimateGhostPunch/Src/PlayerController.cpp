#include "PlayerController.h"

#include <sstream>
#include <GameObject.h>
#include <InputSystem.h>
#include <Scene.h>
#include <Camera.h>
#include <Light.h>

#include "Movement.h"
#include "GhostMovement.h"
#include "Attack.h"
#include "Jump.h"
#include "GhostManager.h"
#include "UltimateGhostPunch.h"
#include "Health.h"
#include "ComponentRegister.h"

REGISTER_FACTORY(PlayerController);

PlayerController::PlayerController(GameObject* gameObject) : UserComponent(gameObject)
{

}

void PlayerController::start()
{
	inputSystem = InputSystem::GetInstance();

	movement = gameObject->getComponent<Movement>();
	ghostMovement = gameObject->getComponent<GhostMovement>();
	ghost = gameObject->getComponent<GhostManager>();
	ghostPunch = gameObject->getComponent<UltimateGhostPunch>();

	std::vector<GameObject*> aux = gameObject->findChildrenWithTag("groundSensor");
	if (aux.size() > 0) jump = aux[0]->getComponent<Jump>();

	aux = gameObject->findChildrenWithTag("attackSensor");
	if (aux.size() > 0) attack = aux[0]->getComponent<Attack>();

	iniPosition = gameObject->transform->getPosition();
}

void PlayerController::update(float deltaTime)
{
	// Ignore input if:
	if (frozen) return; // Player is frozen
	if (checkOutsideLimits()) return; // Player has fallen off limits
	
	checkInput(dir);

	if (ghost != nullptr && ghost->isGhost()) {
		if (ghostPunch == nullptr || (ghostPunch->getState() != CHARGING && ghostPunch->getState() != PUNCHING))
			if (ghostMovement != nullptr) ghostMovement->move(dir);
	}
}

void PlayerController::handleData(ComponentData* data)
{
	for (auto prop : data->getProperties())
	{
		std::stringstream ss(prop.second);

		if (prop.first == "keyboard")
		{
			if (!(ss >> usingKeyboard))
				LOG("PLAYER CONTROLLER: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "index")
		{
			if (!(ss >> playerIndex))
				LOG("PLAYER CONTROLLER: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "fallDamage")
		{
			if (!(ss >> fallDamage))
				LOG("PLAYER CONTROLLER: Invalid property with name \"%s\"", prop.first.c_str());
		}
		else
			LOG("PLAYER CONTROLLER: Invalid property name \"%s\"", prop.first.c_str());
	}
}

void PlayerController::checkInput(Vector3& dir)
{
	dir = Vector3(0, 0, 0);
	Vector3 punchDir;
	//Controles con teclado y raton
	if (usingKeyboard)
	{

		if (inputSystem->isKeyPressed("A")) {
			dir = Vector3(-1, 0, 0);
			gameObject->transform->setRotation({ 0,-90,0 });
		}
		else if (inputSystem->isKeyPressed("D")) {
			dir = Vector3(1, 0, 0);
			gameObject->transform->setRotation({ 0,90,0 });
		}

		if (ghost != nullptr && ghost->isGhost()) {
			if (inputSystem->isKeyPressed("W"))
				dir += Vector3(0, 1, 0);
			else if (inputSystem->isKeyPressed("S"))
				dir += Vector3(0, -1, 0);

			else if (inputSystem->getMouseButtonHold('l'))
			{
				if (ghostPunch != nullptr && ghostPunch->getState() == CHARGING)
				{
					std::pair<int, int> mousePos = inputSystem->getMousePosition();
					Vector3* thisOnScreen = &gameObject->getScene()->getMainCamera()->worldToScreenPixel(gameObject->transform->getPosition());
					ghostPunch->aim(mousePos.first - thisOnScreen->x, thisOnScreen->y - mousePos.second);
				}
			}
			else if (inputSystem->getMouseButtonRelease('l'))
			{
				if (ghostPunch != nullptr && ghostPunch->getState() == CHARGING)
				{
					std::pair<int, int> mousePos = inputSystem->getMousePosition();
					Vector3* thisOnScreen = &gameObject->getScene()->getMainCamera()->worldToScreenPixel(gameObject->transform->getPosition());
					ghostPunch->aim(mousePos.first - thisOnScreen->x, thisOnScreen->y - mousePos.second);

					ghostPunch->ghostPunch();
				}
			}
		}

		if (inputSystem->getMouseButtonClick('l')) {
			if (ghost == nullptr || !ghost->isGhost())
				attack->quickAttack();
			else if (ghostPunch != nullptr && ghostPunch->getState() == AVAILABLE)
				ghostPunch->charge();
		}
		else if (inputSystem->getMouseButtonClick('r')) {
			if (ghost == nullptr || !ghost->isGhost())
				if (attack != nullptr) attack->strongAttack();
		}
		else if (InputSystem::GetInstance()->isKeyPressed("Space"))
			if (ghost == nullptr || !ghost->isGhost())
				if (jump != nullptr) jump->salta();
	}
	//Controles con mando
	else
	{
		if (inputSystem->getLeftJoystick(playerIndex).first < 0 || inputSystem->isButtonPressed(playerIndex, "Left")) {
			dir = Vector3(-1, 0, 0);
			gameObject->transform->setRotation({ 0,-90,0 });
		}
		else if (inputSystem->getLeftJoystick(playerIndex).first > 0 || inputSystem->isButtonPressed(playerIndex, "Right")) {
			dir = Vector3(1, 0, 0);
			gameObject->transform->setRotation({ 0,90,0 });
		}

		if (ghost != nullptr && ghost->isGhost()) {
			if (inputSystem->getLeftJoystick(playerIndex).second < 0 || inputSystem->isButtonPressed(playerIndex, "Up"))
				dir += Vector3(0, 1, 0);
			else if (inputSystem->getLeftJoystick(playerIndex).second > 0 || inputSystem->isButtonPressed(playerIndex, "Down"))
				dir += Vector3(0, -1, 0);

			if (inputSystem->getRightJoystick(playerIndex).first != 0 || inputSystem->getRightJoystick(playerIndex).second != 0)
			{
				if (ghostPunch != nullptr)
				{
					if (ghostPunch->getState() == AVAILABLE) ghostPunch->charge();
					else if (ghostPunch->getState() == CHARGING)
					{
						ghostPunch->aim(inputSystem->getRightJoystick(playerIndex).first, -inputSystem->getRightJoystick(playerIndex).second);

						if (inputSystem->getButtonPress(playerIndex, "X"))
							ghostPunch->ghostPunch();
					}
				}
			}
		}

		if (inputSystem->getButtonPress(playerIndex, "X")) {
			if (ghost == nullptr || !ghost->isGhost())
				if (attack != nullptr) attack->quickAttack();
		}

		else if (inputSystem->getButtonPress(playerIndex, "Y")) {
			if (ghost == nullptr || !ghost->isGhost())
				if (attack != nullptr) attack->strongAttack();
		}

		else if (InputSystem::GetInstance()->isButtonPressed(playerIndex, "A"))
			if (ghost == nullptr || !ghost->isGhost())
				if (jump != nullptr) jump->salta();
	}

	if (ghost != nullptr && ghost->isGhost()) {
		if (ghostPunch == nullptr || (ghostPunch->getState() != CHARGING && ghostPunch->getState() != PUNCHING))
			if (ghostMovement != nullptr) ghostMovement->move(dir);
	}
}

void PlayerController::fixedUpdate(float deltaTime)
{
	//Movimiento
	if (ghost == nullptr || !ghost->isGhost()) {
		if (movement != nullptr) movement->move(dir);
	}
}

bool PlayerController::checkOutsideLimits()
{
	bool out = gameObject->transform->getPosition().y <= bottomLimit;
	if (out)
	{
		Health* health = gameObject->getComponent<Health>();
		health->receiveDamage(fallDamage);

		// If the player is alive after falling it respawns
		// else, it appears as a ghost at initial position
		if (health->getHealth() > 0)
			respawn(iniPosition);
		else
		{
			gameObject->transform->setPosition(iniPosition);
			// Sets the initial position as the respawn if the player resurrects
			ghost->setDeathPosition(iniPosition);
		}
	}

	return out;
}

int PlayerController::getPlayerIndex() const
{
	return playerIndex;
}

Vector3 PlayerController::getInitialPosition() const
{
	return iniPosition;
}

void PlayerController::respawn(const Vector3& respawnPos)
{
	Health* health = gameObject->getComponent<Health>();
	if (health != nullptr) health->resurrect();

	gameObject->transform->setPosition(respawnPos);
	movement->stop();
}


void PlayerController::setFrozen(bool freeze)
{
	frozen = freeze;
}
