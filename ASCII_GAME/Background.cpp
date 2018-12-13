#include "Background.h"
#include "Core/Renderer/ConsoleColours.h"
#include <chrono>
#include <cstdlib>


//Creates a background of stars.
Background::Background(int width, int height) :
	m_StarThreshold(99),	//Determines how many stars there will be.
	m_ColourThreshold(75)	//Determines the colour of the stars.
{
	if (m_bInitialised)
		return;

	m_Size = Vector2(width, height);

	m_PixelData = new CHAR_INFO*[m_Size.y]; //allocates pointers to pixel rows on the heap

	for (int i = 0; i < m_Size.y; i++)
	{
		m_PixelData[i] = new CHAR_INFO[m_Size.x]; //allocates an array of pixels for each row.
	}

	int time = std::chrono::system_clock::now().time_since_epoch().count(); //RNG uses system time as seed.
	srand(time);

	for (int y = 0; y < m_Size.y; y++) //Iterates through every row.
	{
		for (int x = 0; x < m_Size.x; x++) //Iterates through every collumn.
		{
			m_PixelData[y][x].Attributes = BACKGROUND_BLACK; //Initialises pixel.
			m_PixelData[y][x].Char.UnicodeChar = 0; //Sets to null character.

			if ((rand() % 100 + 1) > m_StarThreshold) //If number is greater than 99, set to a star.
			{
				m_PixelData[y][x].Char.UnicodeChar = '*';
				if ((rand() % 100 + 1) > m_ColourThreshold) //Determines which stars will be white and which shall be grey.
				{
					m_PixelData[y][x].Attributes = FOREGROUND_DARK_GREY;
				}
				else
				{
					m_PixelData[y][x].Attributes = FOREGROUND_WHITE;
				}
			}
		}
	}
	m_bInitialised = true;
}

