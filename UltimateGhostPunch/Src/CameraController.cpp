#include "CameraController.h"
#include <sstream>
#include <ComponentRegister.h>
#include <GameObject.h>
#include <MathUtils.h>

#include "GameManager.h"

#include <Timer.h>
#include "UltimateGhostPunch.h"

REGISTER_FACTORY(CameraController);

CameraController::CameraController(GameObject* gameObject) : UserComponent(gameObject), smoothFactor(0.125f), minZ(20), maxZ(100), zoomFactor(1.0f),
time(0.0f), slowMoTime(0.3f), state(MIDPOINT), slowMoDistance(5.0f), slowMoTimeScale(0.3f), slowMoZ(15.0f), playerPunching(nullptr)
{

}

CameraController::~CameraController()
{

}

void CameraController::preUpdate(float deltaTime)
{
	// Update SLOWMO timer
	if (time > 0.0f)
		time -= deltaTime;
	else if (state == SLOWMO)
		deactivateSlowMo();
}

void CameraController::update(float deltaTime)
{
	handleState();

	if (state != SLOWMO)
		checkSlowMo();

	smoothMove();
}

void CameraController::handleData(ComponentData* data)
{
	for (auto prop : data->getProperties())
	{
		std::stringstream ss(prop.second);

		if (prop.first == "minZ") {
			if (!(ss >> minZ))
				LOG("DYNAMIC CAMERA: Invalid value for property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "maxZ") {
			if (!(ss >> maxZ))
				LOG("DYNAMIC CAMERA: Invalid value for property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "smoothFactor") {
			if (!(ss >> smoothFactor))
				LOG("DYNAMIC CAMERA: Invalid value for property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "zoomFactor") {
			if (!(ss >> zoomFactor))
				LOG("DYNAMIC CAMERA: Invalid value for property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "slowMoTime") {
			if (!(ss >> slowMoTime))
				LOG("DYNAMIC CAMERA: Invalid value for property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "slowMoDistance") {
			if (!(ss >> slowMoDistance))
				LOG("DYNAMIC CAMERA: Invalid value for property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "slowMoTimeScale") {
			if (!(ss >> slowMoTimeScale))
				LOG("DYNAMIC CAMERA: Invalid value for property with name \"%s\"", prop.first.c_str());
		}
		else if (prop.first == "slowMoZ") {
			if (!(ss >> slowMoZ))
				LOG("DYNAMIC CAMERA: Invalid value for property with name \"%s\"", prop.first.c_str());
		}
		else
			LOG("DYNAMIC CAMERA: Invalid property with name \"%s\"", prop.first.c_str());
	}
}

void CameraController::smoothMove()
{
	Vector3 lerpDest = gameObject->transform->getPosition();
	lerpDest.lerp(target, smoothFactor);
	gameObject->transform->setPosition(lerpDest);
}

void CameraController::handleState()
{
	if (state == MIDPOINT)
		setTargetToMidPointPlayers();
	else if (state == SLOWMO)
		setTargetToSlowMo();
}

float CameraController::getMaxDistBetweenPlayers()
{
	std::vector<GameObject*> players = GameManager::GetInstance()->getKnights();
	// number of players
	int n = players.size();
	float maxDist = -1;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i != j) {
				float d = (players[i]->transform->getPosition() - players[j]->transform->getPosition()).magnitude();
				if (d > maxDist) maxDist = d;
			}
		}
	}

	return maxDist;
}

Vector3 CameraController::getMidPointBetweenPlayers()
{
	// Vector with every player
	std::vector<GameObject*> players = GameManager::GetInstance()->getKnights();
	// number of players
	int n = players.size();
	float midX = 0.0f, midY = 0.0f;

	for (auto p : players) {
		midX += p->transform->getPosition().x / n;
		midY += p->transform->getPosition().y / n;
	}

	return Vector3(midX, midY, 0.0f);
}

void CameraController::setTargetToMidPointPlayers()
{
	// Move towards mid-point position
	Vector3 midPos = getMidPointBetweenPlayers();// +Vector3(0, 0, gameObject->transform->getPosition().z);

	// Zoom in/out
	float dist = getMaxDistBetweenPlayers();
	//clamp between minZ and maxZ
	dist *= zoomFactor;
	dist = std::min(maxZ, std::max(dist, minZ));

	target = midPos + Vector3(0, 0, dist);
}

void CameraController::setTargetToSlowMo()
{
	if (playerPunching == nullptr) return;

	Vector3 playerPunchingPos = playerPunching->transform->getPosition();
	target = { playerPunchingPos.x, playerPunchingPos.y, slowMoZ };
}

void CameraController::activateSlowMo()
{
	Timer::GetInstance()->setTimeScale(slowMoTimeScale);
	time = slowMoTime;
	state = SLOWMO;
}

void CameraController::deactivateSlowMo()
{
	Timer::GetInstance()->setTimeScale(1.0f);
	state = MIDPOINT;
}

GameObject* CameraController::someonePunching()
{
	// Vector with every player
	std::vector<GameObject*> players = GameManager::GetInstance()->getKnights();
	// number of players
	int n = players.size();

	int i = 0;
	while (i < n && !players[i]->getComponent<UltimateGhostPunch>()->isPunching())
		i++;

	if (i < n) return players[i];
	return nullptr;
}

void CameraController::checkSlowMo()
{
	playerPunching = someonePunching();

	if (playerPunching != nullptr && getMaxDistBetweenPlayers() < slowMoDistance)
		activateSlowMo();
}