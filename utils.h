#include <SFML/Graphics.hpp>

//---Title: FoP Project - Fall 2023---//
const char title[] = "FoP Project - Fall 2023";

//---Height and Width of the Actual Interactive Game---//
const int M = 20;   //Number of rows for a piece to cover on the screen (not the entire screen) = 20
const int N = 10;   //Number of columns for a piece to cover on the screen (not the entire screen) = 10

const int MAIN_MENU = 1,
          PLAYING = 2,
          PAUSED = 3;

//---The Actual Interactive Game Grid - Built Over (MxN)---//
int gameGrid[M][N] = {0};

int highScores[10], difficultyLevel = 0, bombColor;

bool beginner = 0;

struct Pieces
{
    char ch;
    int coords[4][2];
};

//---To Hold the Coordinates of the Piece---//
// int point_1[4][2], point_2[4][2];
Pieces currentPiece, currentPiece_2, shadowPiece, nextPiece, heldPiece;

Pieces bombPiece = {
    'B',
    {0}
};

sf::Color colors[7] = {
    sf::Color(255, 217, 0),
    sf::Color(255, 0, 0),
    sf::Color(0, 235, 0),
    sf::Color(238, 0, 255),
    sf::Color(255, 109, 0),
    sf::Color(0, 215, 252),
    sf::Color(0, 63, 221)
};

//---Check Uncertain Conditions---//
bool anamoly()
{
    for (int i = 0; i < 4; i++)
        if (currentPiece.coords[i][0] < 0 || currentPiece.coords[i][0] >= N || currentPiece.coords[i][1] >= M - difficultyLevel)
            return 0;
        else if (gameGrid[currentPiece.coords[i][1]][currentPiece.coords[i][0]])
            return 0;

    return 1;
}

bool shadowAnamoly()
{
    for (int i = 0; i < 4; i++)
        if (shadowPiece.coords[i][0] < 0 || shadowPiece.coords[i][0] >= N || shadowPiece.coords[i][1] >= M - difficultyLevel)
            return 1;
        else if (gameGrid[shadowPiece.coords[i][1]][shadowPiece.coords[i][0]])
            return 1;

    return 0;
}
