#pragma once
#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "UserComponent.h"

#include <vector>
#include <string>

class GameObject;

class GameManager : public UserComponent
{
private:
	static GameManager* instance;

	bool paused;

	int numPlayers;

	std::vector<int> playerIndexes;
	std::vector<GameObject*> knights;

	std::string level;
	std::string song;

	int health;
	int time;

	// Bottom limit of the level 
	float bottomLimit = -10;

public:
	GameManager();
	GameManager(GameObject* gameObject);

	~GameManager();

	static GameManager* GetInstance();

	virtual void start();

	void setNumPlayers(int nPlayers);
	int getNumPlayers();

	void setPlayerIndexes(std::vector<int> playerIndexes);
	std::vector<int>& getPlayerIndexes();
	std::vector<GameObject*>& getKnights();

	void setLevel(std::string level);
	std::string getLevel();

	void setSong(std::string song);
	std::string getSong();

	void setHealth(int health);
	int getHealth();

	void setTime(int time);
	int getTime();

	float getBottomLimit() const;
};

#endif