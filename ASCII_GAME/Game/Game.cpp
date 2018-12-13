#include "Game.h"
#include <windows.h>
#include <MMSystem.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "../Core/Utils.h"
#include "../Core/Renderer/ASCIIRenderer.h"
#include "../Core/Renderer/AnimatedSprite.h"
#include "../Core/Maths/Vector3.h"
#include"../GameTitleSprites.h"
#include "../DifficultySelectMenu.h"
#include "../InGameSprites.h"
#include "../GameOverSprites.h"
#include "../WinnerSprites.h"

const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 96;

const int SCORE_SPRITE_SIZE = 12;

float deltaTime = 1.0f / 30.0f;

int playerLives = 3;

#define VK_NUM1		0x31
#define VK_NUM2		0x32	
#define VK_NUM3		0x33
#define VK_SPACE	0x20  

Game::Game()
:m_pRenderer(NULL)
, m_bInitialised(false)
, m_bExitApp(false)
, m_Background(SCREEN_WIDTH, SCREEN_HEIGHT) //Initialise background to the screen size.
, m_Score(0) //Initialise score to 0.
{
}

Game::~Game()
{
	SAFE_DELETE_PTR(m_pRenderer);

	//Delete enemy missiles after game ends.
	for (int i = 0; i < m_EnemyMissiles.size(); i++)
		delete m_EnemyMissiles[i];
}

void Game::Initialise()
{
	InitialiseRenderer();

	InitialiseMainMenu();

	InitialiseHighScoresMenu();

	InitialiseDifficultySelect();

	InitialiseGame();

	m_GameState = E_GAME_STATE_MAIN_MENU; //Initialise to the main menu before the game runs.

	m_bInitialised = true;

	InitialiseGameOver();
	InitialiseYouWin();
}

void Game::InitialiseRenderer()
{
	m_pRenderer = new ASCIIRenderer();

	m_pRenderer->Initialise(SCREEN_WIDTH, SCREEN_HEIGHT);
}


void Game::InitialiseMainMenu()
{
	//Main Menu - initialize sprites
	m_GameTitle.Initialise(GameTitleSprite, titleSize);
	m_SirenTitle.Initialise(&SirenTitleSprite[0][0], Vector3(SIREN_SPIRTE_WIDTH, SIREN_SPRITE_HEIGHT, SIREN_SPRITE_ANIMATION_FRAMES));
	m_Play.Initialise(&PlaySprite[0][0], Vector3(PLAY_SPIRTE_WIDTH, PLAY_SPRITE_HEIGHT, PLAY_SPRITE_ANIMATION_FRAMES));
	m_Quit.Initialise(EscQuitSprite, EscQuitSize);

	PlaySound("../Sounds/Main_Menu_Theme", NULL, SND_ASYNC);

	//Set position of sprite
	Vector2 titlePosition((SCREEN_WIDTH / 2) - (titleSize.x / 2), 20);
	m_GameTitle.SetPosition(titlePosition);
	Vector2 sirenPosition((SCREEN_WIDTH / 2) - (SIREN_SPIRTE_WIDTH / 2), 55);
	m_SirenTitle.SetPosition(sirenPosition);
	Vector2 playPosition((SCREEN_WIDTH / 2) - (PLAY_SPIRTE_WIDTH / 2), 70);
	m_Play.SetPosition(playPosition);
	Vector2 quitPosition((SCREEN_WIDTH / 2) - (EscQuitSize.x / 2), 80);
	m_Quit.SetPosition(quitPosition);
}

void Game::InitialiseHighScoresMenu()
{
	ReadFromFile(); //reads scores from file.
	std::sort(m_HighScores.begin(), m_HighScores.end()); //sorts scores in acceding order.
	std::reverse(m_HighScores.begin(), m_HighScores.end()); //Reverses the order of the list, sorting them into decending order.

	for (int i = m_HighScores.size() - 1; i >= 0; i--) //due to arrays starting at element 0, the -1 stops the game from crashing.
	{
		m_Scores.push_back(new ScoreDisplay[4]); 		//pushes new score display into ScoreDisplay vector.
		for (int j = 0; j < 4; j++)
		{
			int arrayWidth = (DIGIT_WIDTH * 4) + (3 * 3);
			int position = (SCREEN_WIDTH / 2) - (arrayWidth / 2); //Sets array to the middle of the screen width.

			m_Scores.back()[j].Initialise(Vector2(position + (j * 10), 5 + (i * 11)));
			UpdateScoreDisplay(m_Scores.back(), m_HighScores[i]);
		}

		m_Scores.back()->SetDigitValue(m_HighScores[i]); //Gets the score display that was pushed into vector and stores the value of the array into the score display
	}
}

void Game::InitialiseDifficultySelect()
{
	//Initialise Sprites
	m_SelectDifficultyTitle.Initialise(DifficultySelectTitleSprite, DifficultySelectTitleSize);
	m_SelectDifficulty.Initialise(DifficultySelectSprite, DifficultySelectSize);
	m_SelectDifficultyBackground.Initialise(DifficultySelectBackgroundSprite, DifficultySelectBackgroundSize);

	//Set Position of sprites on screen.
	m_SelectDifficultyTitle.SetPosition((SCREEN_WIDTH / 2) - (DifficultySelectTitleSize.x / 2), 3);
	m_SelectDifficulty.SetPosition((SCREEN_WIDTH - DifficultySelectSize.x) - 3, 12);
	m_SelectDifficultyBackground.SetPosition(SCREEN_WIDTH - DifficultySelectBackgroundSize.x, SCREEN_HEIGHT - DifficultySelectBackgroundSize.y);
}

void Game::InitialiseGame()
{
	m_EnemyCounter = 0;

	for (int i = 0; i < m_EnemyMissiles.size(); i++) //delete any missiles from previous round.
	{
		delete m_EnemyMissiles[i];
	}

	m_EnemyMissiles.clear();

	m_Player.Initialise(Vector2(0, SCREEN_HEIGHT / 2));


	m_Planet.Initialise(PlanetSprite, planetSize);
	m_Planet.SetPosition(175, 15);


	for (int i = 0; i < 4; i++)
	{
		m_ScoreDigit[i].Initialise(Vector2(7 + (i * 10), 7)); //Spacing digits out equally.
	}

	for (int i = 0; i < 3; i++) //Initialise players health at top right of screen.
	{
		m_PlayerHealth[i].Initialise(PlayerHealthSprite, PlayerHealthSpriteSize);
		m_PlayerHealth[i].SetPosition(Vector2(SCREEN_WIDTH - 70 + (i * m_PlayerHealth[i].GetSize().x + 2), 2));
	}

	for (int i = 0; i < m_HighScores.size(); i++)
	{
		if (m_Score > m_HighScores[i])
			m_HighScores.insert(m_HighScores.begin() + i, m_Score);
	}

	m_Score = 0;

	playerLives = 3;
}

void Game::InitialiseGameOver()
{
	//Initialise Sprites
	m_GameOver.Initialise(GameOverSprite,gameOverSize);
	m_Return.Initialise(ReturnMenuSprite, ReturnMenuSize);

	//Set Position of Sprites
	Vector2 gameOverPosition((SCREEN_WIDTH / 2) - gameOverSize.x / 2, (SCREEN_HEIGHT / 2) - gameOverSize.y /2);
	m_GameOver.SetPosition(gameOverPosition);
	Vector2 returnMenuPostion((SCREEN_WIDTH - ReturnMenuSize.x) - 3, 90);
	m_Return.SetPosition(returnMenuPostion);

	for (int i = 0; i < m_EnemyShips.size(); i++) //Deletes enemy ships.
		delete m_EnemyShips[i];

	m_EnemyShips.clear(); //Empties vector

	m_EnemyShips.clear(); //Empties vector

	for (int i = 0; i < m_PlayerMissiles.size(); i++) //Deletes player missiles.
		delete m_PlayerMissiles[i];

	m_PlayerMissiles.clear(); //Empties vector
}

void Game::InitialiseYouWin()
{
	//Initialise Sprites
	m_YouWin.Initialise(WinnerSprite, WinnerSpriteSize);
	m_YouWinShip.Initialise(YouWinShipSprite, YouWinShipSpriteSize);

	//Set Position of Sprites
	Vector2 youWinTitlePosition((SCREEN_WIDTH / 2) - WinnerSpriteSize.x / 2, 5);
	m_YouWin.SetPosition(youWinTitlePosition);
	Vector2 youWinShipPosition((SCREEN_WIDTH / 2) - YouWinShipSpriteSize.x / 2, SCREEN_HEIGHT / 3);
	m_YouWinShip.SetPosition(youWinShipPosition);

	for (int i = 0; i < m_EnemyShips.size(); i++) //Deletes enemy ships.
		delete m_EnemyShips[i];

	m_EnemyShips.clear(); //Empties vector

	for (int i = 0; i < m_PlayerMissiles.size(); i++) //Deletes player missiles.
		delete m_PlayerMissiles[i];

	m_PlayerMissiles.clear(); //Empties vector
	
}

void Game::Run()
{
	while (!m_bExitApp)
	{

		Update();//Calls Game::Update()

		Render();//Calls Game::Render()

		Sleep(30);
	}
}

void Game::Update()
{
	if (GetKeyState(VK_ESCAPE) < 0)
	{
		m_bExitApp = true;
		return;
		//Exit option available on all states of the game.
	}

	switch (m_GameState)
	{
	case E_GAME_STATE_MAIN_MENU: UpdateMainMenu(); //If in the game menu, update main menu.
		break;
	//If in difficulty select menu, update difficulty select and stop playing opening sound.
	case E_GAME_STATE_SELECT_DIFFICULTY: UpdateDifficultySelect(); PlaySound(NULL, NULL, 0);
		break;
	case E_GAME_STATE_IN_GAME: UpdateGame(); //If in the game, update game.
		break;
	case E_GAME_STATE_LOSE_GAME: UpdateGameOver(); //If player looses, update game over.
		break;
	case E_GAME_STATE_WIN_GAME: UpdateYouWin(); //If player wins, update win.
		break;
	}
}

void Game::UpdateMainMenu()
{
	if (GetKeyState(69) < 0 || GetKeyState(101) < 0) //Press E or e to change state to difficulty select.
	{
		m_GameState = E_GAME_STATE_SELECT_DIFFICULTY;
	}

	if (GetKeyState(70) < 0 || GetKeyState(102) < 0) //Press F or f to change state to high score menu.
	{
		m_GameState = E_GAME_STATE_HIGH_SCORES;
	}

	//Updates animations
	m_SirenTitle.UpdateAnimation(deltaTime);
	m_Play.UpdateAnimation(deltaTime);

}

void Game::UpdateDifficultySelect()
{
	if (GetKeyState(VK_NUMPAD1) < 0 || GetKeyState(VK_NUM1) < 0)
	{
		SetDifficulty(DIFFICULTY_EASY);

		m_GameState = E_GAME_STATE_IN_GAME;
	}
	if (GetKeyState(VK_NUMPAD2) < 0 || GetKeyState(VK_NUM2) < 0)
	{
		SetDifficulty(DIFFICULTY_NORMAL);
		m_GameState = E_GAME_STATE_IN_GAME;
	}
	if (GetKeyState(VK_NUMPAD3) < 0 || GetKeyState(VK_NUM3) < 0)
	{
		SetDifficulty(DIFFICULTY_HARD);
		m_GameState = E_GAME_STATE_IN_GAME;
	}
}

void Game::SetDifficulty(Difficulty option)
{
	m_Difficulty = option;

	switch (m_Difficulty)
	{
	case DIFFICULTY_EASY:

		SetEasyDifficulty();

		break;

	case DIFFICULTY_NORMAL:

		SetNormalDifficulty();

		break;


	case DIFFICULTY_HARD:

		SetHardifficulty();
		
		break;
	}
}
void Game::SetEasyDifficulty()
{
	//Iterate through enemy ships and initialise.
	m_EnemyShips.reserve(3); //Optimisation - reserving space.
	for (int i = 0; i < 3; i++)
	{
		m_EnemyShips.push_back(new EnemyShip);
		m_EnemyShips[i]->Initialise(Vector2(SCREEN_WIDTH, SCREEN_HEIGHT / 2), 10, 0, 1);
	}
}

void Game::SetNormalDifficulty()
{
	m_EnemyShips.reserve(5); //easy ship - grey
	for (int i = 0; i < 3; i++)
	{
		m_EnemyShips.push_back(new EnemyShip);
		m_EnemyShips[i]->Initialise(Vector2(SCREEN_WIDTH, SCREEN_HEIGHT / 2), 10, 0, 1);
	}

	for (int i = 3; i < 5; i++) //medium ship - white
	{
		m_EnemyShips.push_back(new EnemyShip);
		m_EnemyShips[i]->Initialise(Vector2(SCREEN_WIDTH, SCREEN_HEIGHT / 2), 30, 1, 2);
	}
}

void Game::SetHardifficulty()
{
	m_EnemyShips.reserve(5);
	for (int i = 0; i < 2; i++) //easy ship - grey
	{
		m_EnemyShips.push_back(new EnemyShip);
		m_EnemyShips[i]->Initialise(Vector2(SCREEN_WIDTH, SCREEN_HEIGHT / 2), 10, 0, 1);
	}


	for (int i = 2; i < 4; i++) //medium ship - white
	{
		m_EnemyShips.push_back(new EnemyShip);
		m_EnemyShips[i]->Initialise(Vector2(SCREEN_WIDTH, SCREEN_HEIGHT / 2), 30, 1, 2);
	}

	for (int i = 4; i < 5; i++) //hard ship - red
	{
		m_EnemyShips.push_back(new EnemyShip);
		m_EnemyShips[i]->Initialise(Vector2(SCREEN_WIDTH, SCREEN_HEIGHT / 2), 50, 2, 3);
	}
}


void Game::UpdateGame() //Functionality that should only be available inside of the actual game state.
{
	if (playerLives <= 0)
	{
		m_GameState = E_GAME_STATE_LOSE_GAME;
		return;
	}

	m_PlayerCounter++;

	if (GetKeyState(VK_SPACE) < 0) //Fire player missile if space bar is pressed
	{
		
		if (m_PlayerCounter >= 10) //Stops player spamming fire missile.
		{
			FireMissile();
			PlaySound("../Sounds/Player_Missile.wav", NULL, SND_ASYNC);
			m_PlayerCounter = 0;
		}
	}


	for (int i = 0; i < m_EnemyShips.size(); i++)
	{
		if (!m_EnemyShips[i]->Active() && m_EnemyCounter <= 50) //only 50 enemy ships can spawn.
		{
			if (rand() % 100 < 1) //randomly spawns enemy ships
			{
				m_EnemyShips[i]->SetActive(true);
				//Sets screen boundaries to below the score sprite. 
				m_EnemyShips[i]->SetPosition(SCREEN_WIDTH, rand() % ((SCREEN_HEIGHT - m_EnemyShips[i]->GetSize().y) - SCORE_SPRITE_SIZE + 1) + SCORE_SPRITE_SIZE); 
				m_EnemyCounter++;
			}
		}
		else
		{
			if (m_EnemyShips[i]->Update(deltaTime)) //Enemy fires
			{
				FireEnemyMissile(*m_EnemyShips[i]);
				PlaySound("../Sounds/Enemy_Missile.wav", NULL, SND_ASYNC);
			}

			if (m_EnemyShips[i]->GetPosition().x + m_EnemyShips[i]->GetSize().x < 0) // if enemy goes off screen, player looses life
			{
				m_EnemyShips[i]->SetActive(false);
				playerLives--;
				NewRoundBegins();
				return; //stop iteration after enemies have been cleared by NewRoundBegins.
			}	
			//If enemy collides with player, "destroy" enemy and player loses game
			else if (m_Player.Collides(m_EnemyShips[i]->GetCurrentSprite()))
			{
				m_GameState = E_GAME_STATE_LOSE_GAME;
			}
		}
	}

	if (m_EnemyCounter >= 50)
	{
		m_GameState = E_GAME_STATE_WIN_GAME; //player wins by defeating all enemies that spawn.
	}

	m_Player.Update();

	UpdateEnemyMissiles();
	UpdatePlayerMissiles();

	UpdateScoreDisplay(m_ScoreDigit, m_Score);
}

void Game::FireMissile()
{
	//Player has two missiles that fire simultaneously. 
	m_PlayerMissiles.push_back(new Missile);
	m_PlayerMissiles.back()->Initialise(Vector2(m_Player.GetPosition().x + 7, m_Player.GetPosition().y), Vector2(3, 0));

	m_PlayerMissiles.push_back(new Missile);
	m_PlayerMissiles.back()->Initialise(Vector2(m_Player.GetPosition().x + 7, m_Player.GetPosition().y + m_Player.GetSize().y -1), Vector2(3, 0));
}

void Game::FireEnemyMissile(EnemyShip &enemy)
{
	m_EnemyMissiles.push_back(new Missile);

	//Makes enemy ships fire.

	static bool alternate = true;

	int yPosition;

	if (alternate) //alternates which "gun" enemies fire from.
		yPosition = enemy.GetPosition().y + 2;
	else
		yPosition = enemy.GetPosition().y + enemy.GetSize().y - 3;

	switch (enemy.GetEnemyType()) //Initialise enemy Missile based on enemy type.
	{
	case 0:
		m_EnemyMissiles.back()->Initialise(Vector2(enemy.GetPosition().x, yPosition), Vector2(-2, 0));
		break;
	case 1:
		m_EnemyMissiles.back()->Initialise(Vector2(enemy.GetPosition().x, yPosition), Vector2(-3, 0));
		break;
	case 2:
		m_EnemyMissiles.back()->Initialise(Vector2(enemy.GetPosition().x, yPosition), Vector2(-5, 0));
		break;
	}

	alternate = !alternate;
}

void Game::UpdatePlayerMissiles()
{

	for (int i = 0; i < m_PlayerMissiles.size(); i++) //Iterate through player missiles and update
	{
		m_PlayerMissiles[i]->Update();

		if (m_PlayerMissiles[i]->GetPosition().x > SCREEN_WIDTH) //if missile goes off right hand side of screen, deletes
		{
			m_PlayerMissiles[i]->Explode();

			continue;
		}

		for (int j = 0; j < m_EnemyShips.size(); j++) //Iterates through the enemy ships
		{
			if (m_EnemyShips[j]->Active() && m_PlayerMissiles[i]->Collides(m_EnemyShips[j]->GetCurrentSprite())) //If the player missiles collides with enemy, enemy explodes
			{
				m_PlayerMissiles[i]->Explode();

				m_EnemyShips[j]->Explode();
				m_Score += 100;
				break;
			}
		}
	}

	for (auto missile = m_PlayerMissiles.begin(); missile != m_PlayerMissiles.end(); ) //Iterate through player missiles and clear out.
	{
		if (!(*missile)->Active())
		{
			delete *missile;
			missile = m_PlayerMissiles.erase(missile);
		}
		else
		{
			missile++;
		}
	}
}


void Game::UpdateEnemyMissiles()
{
	//Iterate through all missiles.
	for (auto missile = m_EnemyMissiles.begin(); missile != m_EnemyMissiles.end(); ) //the type of the variable declared will be automatically selected
	{
		(*missile)->Update();

		if ((*missile)->Collides(m_Player)) //If a missile collides with player, the missile explode and player loses a health point.
		{
			delete *missile;
			missile = m_EnemyMissiles.erase(missile);	// Sets iterator to position after current missile
			playerLives--;
		}
		else if ((*missile)->GetPosition().x + (*missile)->GetSize().x < 0) //Destroys enemy missile once off the left hand side of the screen.
		{
			delete *missile;
			missile = m_EnemyMissiles.erase(missile);
		}
		else
		{
			missile++;	// Missile did not collide, increment iterator to next missile
		}
	}
} 


void Game::UpdateScoreDisplay(ScoreDisplay *display, int value)
{
	for (int i = 0; i < 4; i++)
	{
		display[i].Update(0.0f);
	}

	int ScoreValues[4];

	ScoreValues[3] = value > 9999 ? 9 : value % 10;
	ScoreValues[2] = value > 9999 ? 9 : value / 10 % 10;
	ScoreValues[1] = value > 9999 ? 9 : value / 100 % 10;
	ScoreValues[0] = value > 9999 ? 9 : value / 1000 % 10;

	display[0].SetDigitValue(ScoreValues[0]);
	display[1].SetDigitValue(ScoreValues[1]);
	display[2].SetDigitValue(ScoreValues[2]);
	display[3].SetDigitValue(ScoreValues[3]);
}

void Game::NewRoundBegins()
{
	for (int i = 0; i < m_EnemyShips.size(); i++) //delete any enemies from previous round.
	{
		delete m_EnemyShips[i];
	}
	m_EnemyShips.clear();

	SetDifficulty(m_Difficulty);

	m_Player.Initialise(Vector2(0, SCREEN_HEIGHT / 2)); //Set player position to middle of y axis.

	for (int i = 0; i < m_EnemyMissiles.size(); i++) //delete any  enemy missiles missiles from previous round.
	{
		delete m_EnemyMissiles[i];
	}
	m_EnemyMissiles.clear();
}

void Game::UpdateGameOver()
{
	if (GetKeyState(VK_RETURN) < 0)
	{
		m_GameState = E_GAME_STATE_MAIN_MENU;
		
		InitialiseGame(); //Reset game to original values.
	}
}

void Game::UpdateYouWin()
{
	if (GetKeyState(VK_RETURN) < 0)
	{
		m_GameState = E_GAME_STATE_MAIN_MENU;

		InitialiseGame(); //Reset game to original values.
	}
}

void Game::RenderMainMenu()
{
	m_Background.Render(m_pRenderer);
	m_GameTitle.Render(m_pRenderer);
	m_SirenTitle.Render(m_pRenderer);
	m_Play.Render(m_pRenderer);
	m_Quit.Render(m_pRenderer);
}

void Game::RenderHighScoresMenu()
{
	for (int i = 0; i < m_Scores.size(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ScoreDisplay* temp = m_Scores[i];
			temp[j].Render(m_pRenderer);
		}
	}
}

void Game::RenderDifficultySelect()
{
	m_Background.Render(m_pRenderer);
	m_SelectDifficultyBackground.Render(m_pRenderer);
	m_SelectDifficultyTitle.Render(m_pRenderer);
	m_SelectDifficulty.Render(m_pRenderer);
}

void Game::RenderGame()
{
	//Render backgrounds first so player/enemies and missiles are drawn on top.
	m_Background.Render(m_pRenderer);

	m_Planet.Render(m_pRenderer); 


	for (int i = 0; i < m_EnemyShips.size(); i++)
	{
		if (m_EnemyShips[i]->Active())
		{
			m_EnemyShips[i]->Render(m_pRenderer);
		}
	}

	m_Player.Render(m_pRenderer);

	RenderMissiles(m_PlayerMissiles);
	RenderMissiles(m_EnemyMissiles);
	
	RenderScore();

	for (int i = 0; i < playerLives; i++)
	{
		m_PlayerHealth[i].Render(m_pRenderer);
	}
}

void Game::RenderMissiles(std::vector<Missile*> &missiles)
{
	for (int i = 0; i < missiles.size(); i++)
	{
		if(missiles[i]->Active())
			missiles[i]->Render(m_pRenderer);
	}
}

void Game::RenderScore()
{
	for (int i = 0; i < 4; i++)
	{
		m_ScoreDigit[i].Render(m_pRenderer);
	}
}


void Game::RenderGameOver()
{
	m_Background.Render(m_pRenderer);
	m_GameOver.Render(m_pRenderer);
	m_Return.Render(m_pRenderer);
	RenderScore();
}

void Game::RenderYouWin()
{
	m_Background.Render(m_pRenderer);
	m_YouWin.Render(m_pRenderer);
	m_YouWinShip.Render(m_pRenderer);
	m_Return.Render(m_pRenderer);
}

void Game::Render()
{
	//call this first
	m_pRenderer->ClearScreen();	

	switch (m_GameState)
	{
	case E_GAME_STATE_MAIN_MENU: RenderMainMenu();
		break;
	case E_GAME_STATE_HIGH_SCORES: RenderHighScoresMenu();
		break;
	case E_GAME_STATE_SELECT_DIFFICULTY: RenderDifficultySelect();
		break;
	case E_GAME_STATE_IN_GAME: RenderGame();
		break;
	case E_GAME_STATE_LOSE_GAME: RenderGameOver();
		break;
	case E_GAME_STATE_WIN_GAME: RenderYouWin();
		break;
	}
	//call this last
	m_pRenderer->Render();
}

void Game::ReadFromFile()
{
	InFile.open("../ASCII_GAME/HighScores.txt"); //Opens file.

	if (InFile.fail()) //Checks for error.
	{
		//std::cerr << "File could not open.";
		return;
	}

	std::string line;
	while (std::getline(InFile, line)) //get every line from InFile and store them in line.
	{
		m_HighScores.push_back(std::stoi(line)); //stoi - string to integer.
	}
}

void Game::WriteToFile()
{
	OutFile.open("../ASCII_GAME/HighScores.txt.tmp");

	int maxScores;

	if (m_HighScores.size() > 5)
		maxScores = 5;
	else
		maxScores = m_HighScores.size();

	for (int i = 0; i < maxScores; i++)
	{
		OutFile << m_HighScores[i];
	}

	OutFile.close();

	MoveFile("../ASCII_GAME/HighScores.txt.tmp", "../ASCII_GAME/HighScores.txt");
}
