#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include "pieces.h"
using namespace sf;
using namespace std;

void print_array(int colorNum, int nextColorNum, int heldColorNum)
{
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
            std::cout << gameGrid[i][j] << ' ';
        std::cout << '\n';
    }
    std::cout << "------------------------------------------------\n";
    std::cout << "current piece:\n";
    std::cout << currentPiece.ch << '\n';
    for (int i = 0; i < 4; i++)
        std::cout << currentPiece.coords[i][0] << ' ' << currentPiece.coords[i][1] << '\n';
    std::cout << "------------------------------------------------\n";
    std::cout << "current 2 piece:\n";
    std::cout << currentPiece_2.ch << '\n';
    for (int i = 0; i < 4; i++)
        std::cout << currentPiece_2.coords[i][0] << ' ' << currentPiece_2.coords[i][1] << '\n';
    std::cout << "------------------------------------------------\n";
    std::cout << "next piece:\n";
    std::cout << nextPiece.ch << '\n';
    for (int i = 0; i < 4; i++)
        std::cout << nextPiece.coords[i][0] << ' ' << nextPiece.coords[i][1] << '\n';
    std::cout << "------------------------------------------------\n";
    std::cout << "held piece:\n";
    std::cout << heldPiece.ch << '\n';
    for (int i = 0; i < 4; i++)
        std::cout << heldPiece.coords[i][0] << ' ' << heldPiece.coords[i][1] << '\n';
    std::cout << "------------------------------------------------\n";
    std::cout << "Current color: " << colorNum << '\n'
        << "Next Color: " << nextColorNum << '\n'
        << "Held color: " << heldColorNum << '\n';
}

void setShapeProperties(RectangleShape &shape, const Vector2f &size, const Color &fillColor, const Vector2f &position, Color outlineColor = Color::Transparent, float outlineThickness = 0.f)
{
    shape.setSize(size);
    shape.setFillColor(fillColor);
    shape.setPosition(position);
    shape.setOutlineColor(outlineColor);
    shape.setOutlineThickness(outlineThickness);
}

void setTextProperties(Text& text, const Font &font, const Vector2f &position)
{
    text.setFont(font);
    text.setFillColor(Color::White);
    text.setCharacterSize(25);
    text.setPosition(position);
    text.setLineSpacing(0.7f);
}

void initialSpawn(int colorNum, int nextColorNum)
{
    currentPiece = BLOCKS[colorNum];
    for (int i = 0; i < 4; i++)
        currentPiece.coords[i][0] += colorNum;
    nextPiece = BLOCKS[nextColorNum];
}

void clearGrid()
{
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            gameGrid[i][j] = 0;
}

void startNewGame(int &colorNum, int &nextColorNum, int &score, int &level, int &rowsCleared, bool &playing)
{
    clearGrid();

    currentPiece.ch = ' ';
    for (int i = 0; i < 4; i++)
        currentPiece.coords[i][0] = currentPiece.coords[i][1] = -100;

    currentPiece_2 = heldPiece = currentPiece;
    difficultyLevel = score = rowsCleared = 0;
    level = playing = 1;

    colorNum = rand() % 7;
    nextColorNum = rand() % 7;
    initialSpawn(colorNum, nextColorNum);
}

void showMainMenu(bool &isPaused, stringstream &ss)
{
    ss.clear();
    ss.str("");
    ss << "\t\t\t\tMain Menu\n\n"
        << "1. Start New Game\n"
        << "2. Show High Scores\n"
        << "3. See Help\n"
        << "4. Quit\n"
        << (isPaused ? "5. Continue\n" : "");
}

void readScores()
{
    ifstream file;
    file.open("highscores.txt");
    for (int i = 0; i < 10; i++)
        file >> highScores[i];
    file.close();
}

void writeScores()
{
    ofstream file;
    file.open("highscores.txt");
    for (int i = 0; i < 10; i++)
        file << highScores[i] << '\n';
    file.close();
}

void showHighScores(int highScores[], bool &displayingHighScores, stringstream &ss)
{
    readScores();

    ss.clear();
    ss.str("");
    ss << "\t\t\t\tHighscores\n\n";
    for (int i = 0; i < 5; i++)
        ss << i + 1 << ". " << highScores[i] << "\t\t\t\t" << i + 6 << ". " << highScores[i + 5] << '\n';
    ss << "\n Press b to go back.";
    displayingHighScores = 1;
}

void showHelp(bool &displayingHelp, stringstream &ss)
{
    ss.clear();
    ss.str("");
    ss << "\t\t\t\t\tHelp\n\n"
       << "Clear horizontal lines. Use:\n"
       << "- left/right arrow for moving\n"
       << "- up arrow for rotating\n"
       << "- space for hard drop\n"
       << "- c to hold\n"
       << "\nPress b to go back.";
    displayingHelp = 1;
}

void updateScores(int scores)
{
    int i = 9;
    while (i >= 0 && scores > highScores[i])
    {
        highScores[i + 1] = highScores[i];
        i--;
    }
    highScores[i + 1] = scores;
    writeScores();
}

void showScores(int scores, bool &displaying, stringstream &ss)
{
    readScores();
    ss.clear();
    ss.str("");
    ss << "\t\t\t\tGame Over\n\n"
       << "You scored: " << scores << " points\n"
       << (scores > highScores[0] ? "\t\t\tHIGHSCORE!\n" : "")
       << "\nPress c to continue.\n";

    if (scores > highScores[9])
        updateScores(scores);

    displaying = 1;
}

bool gameOver()
{
    for (int i = 0; i < N; i++)
        if (gameGrid[0][i])
            return 1;
    return 0;
}

void handleBombing(int colorNum)
{
    bool bombColorMatches = 0;
    int bombX = currentPiece.coords[0][1];
    int bombY = currentPiece.coords[0][0];
    int vacancies = 0;

    for (int i = -1; i <= 1; i += 2)
    {
        int coord = bombY + i;
        if (coord >= 0 && coord < N)
        {
            if (colorNum + 1 == gameGrid[bombX][coord])
                bombColorMatches = 1;
        }
        if (!gameGrid[bombX][coord])
            vacancies++;
    }

    std::cout << "Bomb color is: " << colorNum << '\n';

    if (colorNum + 1 == gameGrid[bombX + 1][bombY])
        bombColorMatches = 1;

    if (!gameGrid[bombX + 1][bombY])
        vacancies++;

    if (vacancies == 3)
        return;

    if (bombColorMatches)
        clearGrid();
    else
    {
        int blocksDestroyed = 0;
        for (int i = bombY + 1; i < N; i++)
        {
            if (gameGrid[bombX][i] && gameGrid[bombX][i + 1])
            {
                gameGrid[bombX][i] = 0;
                blocksDestroyed++;
            }
            if (blocksDestroyed == 4)
                return;
        }

        for (int i = bombY - 1; i >= 0; i--)
        {
            if (gameGrid[bombX][i] && gameGrid[bombX][i - 1])
            {
                gameGrid[bombX][i] = 0;
                blocksDestroyed++;
            }
            if (blocksDestroyed == 4)
                return;
        }

        for (int i = bombY; i < N; i++)
        {
            if (gameGrid[bombX + 1][i] && gameGrid[bombX + 1][i + 1])
            {
                gameGrid[bombX + 1][i] = 0;
                blocksDestroyed++;
            }
            if (blocksDestroyed == 4)
                return;
        }
    }
}

//---Piece Starts to Fall When Game Starts---//
void fallingPiece(float &timer, float &delay, int &colorNum, int &nextColorNum, int heldColorNum, bool &hasHeldPiece, bool &bombFalling)
{
    if (timer > delay)
    {
        // print_array(colorNum, nextColorNum, heldColorNum);

        currentPiece_2 = currentPiece;
        for (int i = 0; i < 4; i++)
            currentPiece.coords[i][1] += 1;                   //How much units downward

        if (!anamoly())
        {
            currentPiece = currentPiece_2;
            if (currentPiece.ch == 'B')
                handleBombing(colorNum);

            if (currentPiece.ch != 'B')
                for (int i = 0; i < 4; i++)
                    gameGrid[currentPiece_2.coords[i][1]][currentPiece_2.coords[i][0]] = colorNum + 1;

            colorNum = nextColorNum;
            currentPiece = nextPiece;

            if (!bombFalling)
            {
                int randomX = rand() % 7;
                for (int i = 0; i < 4; i++)
                    currentPiece.coords[i][0] += randomX;

                nextColorNum = rand() % (beginner ? 4 : 7);
                nextPiece = BLOCKS[nextColorNum];
            }
            else
            {
                int bombX = rand() % 10;
                colorNum = rand() % 7;
                currentPiece = bombPiece;
                for (int i = 0; i < 4; i++)
                    currentPiece.coords[i][0] += bombX;
            }

            timer = 0;
            hasHeldPiece = 0;
        }

        timer = 0;
    }
}

void hardDrop()
{
    if (currentPiece.ch != 'B')
        currentPiece = shadowPiece;
}

void resetCoords()
{
    int n = 0;
    for (int i = 0; i < 7; i++)
        if (BLOCKS[i].ch == heldPiece.ch)
        {
            n = i;
            break;
        }
    heldPiece = BLOCKS[n];
}

void handleHolding(int &heldColorNum, int &colorNum, int &nextColorNum, bool &hasHeldPiece)
{
    if (currentPiece.ch != 'B')
    {
        if (heldColorNum == -1)
        {
            heldColorNum = colorNum;
            colorNum = nextColorNum;
            heldPiece = currentPiece;
            currentPiece = nextPiece;
        }
        else
        {
            int temp = heldColorNum;
            heldColorNum = colorNum;
            colorNum = temp;
            currentPiece = heldPiece;
            heldPiece = currentPiece_2;
        }

        resetCoords();
        int randomX = rand() % 7;
        for (int i = 0; i < 4; i++)
            currentPiece.coords[i][0] += randomX;

        hasHeldPiece = 1;
    }
}

void updateShadow()
{
    shadowPiece = currentPiece;
    while (!shadowAnamoly())
        for (int j = 0; j < 4; j++)
            shadowPiece.coords[j][1] += 1;

    for (int j = 0; j < 4; j++)
        shadowPiece.coords[j][1] -= 1;
}

void handleMovement(int delta_x)
{
    if (currentPiece.ch != 'B')
    {
        currentPiece_2 = currentPiece;
        for (int i = 0; i < 4; i++)
            currentPiece.coords[i][0] += delta_x;

        if (!anamoly())
            currentPiece = currentPiece_2;
    }
}

void rotatePiece()
{
    if (currentPiece.ch == 'O' || currentPiece.ch == 'B')
        return;

    for (int i = 0; i < 4; i++)
    {
        int x = currentPiece.coords[i][0] - currentPiece.coords[1][0];
        int y = currentPiece.coords[i][1] - currentPiece.coords[1][1];
        currentPiece.coords[i][0] = -y + currentPiece.coords[1][0];
        currentPiece.coords[i][1] = x + currentPiece.coords[1][1];
    }

    for (int i = 0; i < 4; i++)
    {
        if (currentPiece.coords[i][0] < 0)
        {
            for (int j = 0; j < 4; j++)
                currentPiece.coords[j][0] += (currentPiece.ch != 'I' ? 1 : 2);
            break;
        }
        else if (currentPiece.coords[i][0] >= N)
        {
            for (int j = 0; j < 4; j++)
                currentPiece.coords[j][0] -= (currentPiece.ch != 'I' ? 1 : 2);
            break;
        }
    }

    if (!anamoly())
        currentPiece = currentPiece_2;
}

bool isEmptyRow(int row[])
{
    int emptyColumns = 0;

    for (int i = 0; i < N; i++)
        if (row[i] == 0)
            emptyColumns++;

    return emptyColumns == N
                ? 1
                : 0;
}

void moveRowDown(int row[], int nextRow[])
{
    for (int i = 0; i < N; i++)
        nextRow[i] = row[i];
}

int calculateScore(int rows, int level)
{
    int points = 0;
    for (int i = 1; i <= rows; i++)
        points += (i * 10);
    return points * level;
}

void checkRows(int &level, int &rowsCleared, int &score)
{
    int rows = 0;
    int filled = N;
    int *filledRows = nullptr;
    filledRows = new int[rows];

    for (int i = 0; i < M; i++)
    {
        filled = N;
        for (int j = 0; j < N; j++)
            if (gameGrid[i][j] == 0)
                filled--;
        if (filled == N)
        {
            filledRows[rows] = i;
            rows++;
        }
    }

    rowsCleared += rows;
    score += calculateScore(rows, level);

    for (int i = 0; i < rows; i++)
        for (int j = filledRows[i]; j > 0; j--)
            if (!isEmptyRow(gameGrid[j]))
                moveRowDown(gameGrid[j - 1], gameGrid[j]);

    delete[] filledRows;
}
