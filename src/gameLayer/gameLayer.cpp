#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <IconsForkAwesome.h>
#include <imguiTools.h>
#include <logs.h>

#pragma region Player default values
	const float PLAYER_SPEED = 200;
#pragma endregion

struct GameData
{
	glm::vec2 playerPos = { 100,100 };
} gameData;

gl2d::Renderer2D renderer;
gl2d::Texture spaceShipTexture;

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();

	spaceShipTexture.loadFromFile(RESOURCES_PATH "spaceShip/ships/green.png", true);

	//loading the saved data. Loading an entire structure like this makes savind game data very easy.
	platform::readEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

	platform::log("Init");

	return true;
}


//IMPORTANT NOTICE, IF YOU WANT TO SHIP THE GAME TO ANOTHER PC READ THE README.MD IN THE GITHUB
//https://github.com/meemknight/cmakeSetup
//OR THE INSTRUCTION IN THE CMAKE FILE.
//YOU HAVE TO CHANGE A FLAG IN THE CMAKE SO THAT RESOURCES_PATH POINTS TO RELATIVE PATHS
//BECAUSE OF SOME CMAKE PROGBLMS, RESOURCES_PATH IS SET TO BE ABSOLUTE DURING PRODUCTION FOR MAKING IT EASIER.

bool gameLogic(float deltaTime, platform::Input &input)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);
#pragma endregion

#pragma region movement
	//you can also do platform::isButtonHeld(platform::Button::Left)
	glm::vec2 move = {};
	if (input.isButtonHeld(platform::Button::Left) || 
		input.isButtonHeld(platform::Button::A))
	{
		move.x = -1;
		//gameData.playerPos.x -= deltaTime * 100;
	}
	if (input.isButtonHeld(platform::Button::Right) ||
		input.isButtonHeld(platform::Button::D))
	{
		move.x = 1;
		//gameData.playerPos.x += deltaTime * 100;
	}
	if (input.isButtonHeld(platform::Button::Up) ||
		input.isButtonHeld(platform::Button::W))
	{
		move.y = -1;
		//gameData.playerPos.y -= deltaTime * 100;
	}
	if (input.isButtonHeld(platform::Button::Down) ||
		input.isButtonHeld(platform::Button::S))
	{
		move.y = 1;
		//gameData.playerPos.y += deltaTime * 100;
	}

	if (move.x != 0 || move.y != 0) //Zero check to avoid Division by 0 error
	{
		// Normalize the movement so the diagonal movement is not faster than straight.
		move = glm::normalize(move);
		move *= deltaTime * PLAYER_SPEED;
		gameData.playerPos += move;
	}

#pragma endregion

	gameData.playerPos = glm::clamp(gameData.playerPos, glm::vec2{0,0}, glm::vec2{w - 100,h - 100});
	renderer.renderRectangle({gameData.playerPos, 100, 100}, spaceShipTexture);

	renderer.flush();

#if REMOVE_IMGUI == 0
	//ImGui::ShowDemoWindow();
	ImGui::PushMakeWindowNotTransparent();
	ImGui::Begin("Test Imgui");

	ImGui::DragFloat2("Positions", &gameData.playerPos[0]);

	ImGui::Text("Emoji moment: " ICON_FK_AMBULANCE);

	ImGui::helpMarker("test");

	ImGui::addErrorSymbol();
	ImGui::addWarningSymbol();

	ImGui::BeginChildFrameColoured(1, Colors_Gray, {100,100}, 0);

	ImGui::Text("Test");

	ImGui::EndChildFrame();


	ImGui::PopMakeWindowNotTransparent();
	ImGui::End();
#endif

	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{

	//saved the data.
	platform::writeEntireFile(RESOURCES_PATH "gameData.data", &gameData, sizeof(GameData));

}
