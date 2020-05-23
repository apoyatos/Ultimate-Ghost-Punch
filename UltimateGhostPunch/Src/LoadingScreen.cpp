#include "LoadingScreen.h"

#include <ComponentRegister.h>
#include <SceneManager.h>
#include <GameObject.h>
#include <UILayout.h>
#include <time.h>

REGISTER_FACTORY(LoadingScreen);

std::string LoadingScreen::getRandomTip()
{
	srand(time(NULL));

	if (tipsVector.empty())
		return "No tips for you";

	int i = rand() % (tipsVector.size() - 1);

	return tipsVector[i];
}

LoadingScreen::LoadingScreen(GameObject* gameObject) : UserComponent(gameObject), sceneToLoad(""), loadDelay(0), currentDelay(0)
{

}

LoadingScreen::~LoadingScreen()
{

}

void LoadingScreen::start()
{
	sceneToLoad = "NO SCENE";
	loadDelay = 1.0f;
	currentDelay = 0;
	GameObject* mainCamera = findGameObjectWithName("MainCamera");
	if (notNull(mainCamera) && notNull(mainCamera->getComponent<UILayout>())) {
		UIElement root = mainCamera->getComponent<UILayout>()->getRoot();
		UIElement tipsText = root.getChild("Tips");

		tipsText.setText(getRandomTip());
		tipsText.setProperty("HorzFormatting", "WordWrapCentred");
	}
}

void LoadingScreen::update(float deltaTime)
{
	if (sceneToLoad == "NO SCENE")
		currentDelay += deltaTime;

	if (notNull(SceneManager::GetInstance()) && currentDelay > loadDelay)
	{
		sceneToLoad = SceneManager::GetInstance()->getSceneToLoad();

		if (sceneToLoad != "NO SCENE")
			SceneManager::GetInstance()->changeScene(sceneToLoad);
	}
}

void LoadingScreen::handleData(ComponentData* data)
{
	int i = 1;
	checkNullAndBreak(data);
	for (auto prop : data->getProperties())
	{
		std::stringstream ss(prop.second);

		std::string propName = "tip" + std::to_string(i);

		if (prop.first == propName)
		{
			tipsVector.push_back(prop.second);
		}

		i++;
	}
}