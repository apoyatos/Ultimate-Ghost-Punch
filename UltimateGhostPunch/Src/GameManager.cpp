#include "GameManager.h"
#include "Timer.h"

#include "ComponentRegister.h"

REGISTER_FACTORY(GameManager);

GameManager* GameManager::instance = nullptr;

GameManager::GameManager() : UserComponent(nullptr), paused(false)
{

}

GameManager::GameManager(GameObject* gameObject) : UserComponent(gameObject)
{
	if (instance == nullptr)
		instance = this;
	else
		destroy(gameObject);
}

GameManager::~GameManager()
{
	if (instance == this)
		instance = nullptr;
}

GameManager* GameManager::GetInstance()
{
	return instance;
}

void GameManager::start()
{
	level = "";
	song = "";

	dontDestroyOnLoad(gameObject);
}

void GameManager::update(float deltaTime)
{

}

void GameManager::setLevel(std::string level)
{
	this->level = level;
	//todo:
	//Leer archivo config del nivel y guardar las posiciones de players/obst�culos para crearlos
	//Leer el l�mite inferior del nivel: bottomLimit = ...
}

std::string GameManager::getLevel()
{
	return level;
}

void GameManager::setSong(std::string song)
{
	this->song = song;
}

std::string GameManager::getSong()
{
	return song;
}

void GameManager::setHealth(int health)
{
	this->health = health;
}

int GameManager::getHealth()
{
	return health;
}

void GameManager::setTime(int time)
{
	this->time = time;
}

int GameManager::getTime()
{
	return time;
}

float GameManager::getBottomLimit() const
{
	return bottomLimit;
}

void GameManager::setPlayerIndexes(std::vector<int> playerIndexes)
{
	this->playerIndexes = playerIndexes;
}

std::vector<int>& GameManager::getPlayerIndexes()
{
	return playerIndexes;
}

void GameManager::reset()
{
	Timer::GetInstance()->setTimeScale(1.0f);
	paused = false;
}

void GameManager::setNPlayers(int nPlayers)
{
	this->nPlayers = nPlayers;
}

int GameManager::getNPlayers()
{
	return nPlayers;
}

void GameManager::pauseGame(bool setPaused)
{
	if (paused == setPaused) return;

	paused = setPaused;

	if (paused)
		Timer::GetInstance()->setTimeScale(0.0f); //Pause the game
	else
		Timer::GetInstance()->setTimeScale(1.0f); //Resume the game
}

bool GameManager::gameIsPaused()
{
	return paused;
}

std::vector<GameObject*>& GameManager::getKnights()
{
	return knights;
}
