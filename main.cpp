#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <time.h>
#include "functionality.h"
using namespace sf;
using namespace std;

int main()
{
    srand(time(0));
    RenderWindow window(VideoMode(660, 480), title);
    window.setFramerateLimit(60);

    Texture obj1, obj2, obj3;
    Font font;

    if (!obj1.loadFromFile("assets/tiles.png"))
        cout << "cannot load assets/tiles.png\n";
    if (!obj2.loadFromFile("assets/background.png"))
        cout << "cannot load assets/background.png\n";
    if (!obj3.loadFromFile("assets/bomb.png"))
        cout << "cannot load assets/bomb.png\n";
    if (!font.loadFromFile("assets/font.ttf"))
        cout << "cannot load assets/font.ttf\n";

    Sprite sprite(obj1), background(obj2), bomb(obj3);

    Color white(255, 255, 255, 230);

    int delta_x = 0,
        colorNum = rand() % 4,
        nextColorNum = rand() % 4,
        heldColorNum = -1,
        rowsCleared = 0,
        beforeIntervalRows = 0,
        afterIntervalRows = 0,
        level = 1,
        scores = 0;

    bool playing = 0,
         isPaused = 0,
         rotate = 0,
         hasHeldPiece = 0,
         showingScores = 0,
         displaying[3] = { 0  },
         bombFalling = 0;

    float timer = 0,
          delay = 0.3,
          interval = 0;

    RectangleShape gridSquare,
                   piecesBox,
                   heldPieceBox,
                   frame,
                   statsBox,
                   linesBox,
                   levelBox,
                   scoreBox,
                   shadowBlock,
                   nextPieceBox,
                   menuBox;

    Text menuText,
         linesText,
         levelText,
         scoreText,
         nextPieceText,
         heldPieceText;

    Clock clock;
    stringstream ss;

    setShapeProperties(gridSquare, Vector2f(18.f, 18.f), Color::Transparent, Vector2f(0, 0), Color(35, 35, 35), -0.5f);
    setShapeProperties(shadowBlock, Vector2f(18.f, 18.f), Color::Transparent, Vector2f(0, 0), colors[colorNum -1], -1.5f);

    setShapeProperties(piecesBox, Vector2f(180.f, 225.f), white, Vector2f(28, 82.5f));

    setShapeProperties(nextPieceBox, Vector2f(150.f, 90.f), Color::Black, piecesBox.getPosition() + Vector2f(15.f, 15.f));
    setTextProperties(nextPieceText, font, nextPieceBox.getPosition() + Vector2f(8.f, 1.f));
    nextPieceText.setString("Next Piece");

    setShapeProperties(heldPieceBox, Vector2f(150.f, 90.f), Color::Black, piecesBox.getPosition() + Vector2f(15.f, 30.f + nextPieceBox.getSize().y));
    setTextProperties(heldPieceText, font, heldPieceBox.getPosition() + Vector2f(8.f, 1.f));
    heldPieceText.setString("Hold");

    setShapeProperties(frame, Vector2f(18.f * N, 18.f * M), Color::Black, Vector2f(piecesBox.getSize().x + 56.f, 31.f), white, 15.f);

    setShapeProperties(statsBox, Vector2f(180.f, 255.f), Color(255, 255, 255, 230), Vector2f(frame.getPosition().x + frame.getSize().x + 28.f, 83.5f));

    setShapeProperties(linesBox, Vector2f(150.f, 66.f), Color::Black, statsBox.getPosition() + Vector2f(15.f, 15.f));
    setTextProperties(linesText, font, linesBox.getPosition() + Vector2f(8.f, 1.f));

    setShapeProperties(levelBox, Vector2f(150.f, 60.f), Color::Black, statsBox.getPosition() + Vector2f(15.f, 30.f + linesBox.getSize().y));
    setTextProperties(levelText, font, levelBox.getPosition() + Vector2f(8.f, 1.f));

    setShapeProperties(scoreBox, Vector2f(150.f, 60.f), Color::Black, statsBox.getPosition() + Vector2f(15.f, 45.f + linesBox.getSize().y * 2));
    setTextProperties(scoreText, font, scoreBox.getPosition() + Vector2f(8.f, 1.f));

    setShapeProperties(menuBox, Vector2f(400.f, 300.f), Color(0, 0, 0, 200), Vector2f(120.f, 50.f));

    setTextProperties(menuText, font, menuBox.getPosition() + Vector2f(8.f, 1.f));
    menuText.setCharacterSize(35);

    Time slp = seconds(10.f);

    initialSpawn(colorNum, nextColorNum);
    while (window.isOpen())
    {
        window.clear(Color::Magenta);
        window.draw(background);

        if (gameOver() && !showingScores)
        {
            playing = 0;
            showScores(scores, displaying[2], ss);
            showingScores = 1;
        }

        if (!playing)
        {
            if (!displaying[0] && !displaying[1] && !displaying[2])
                showMainMenu(isPaused, ss);

            Event e;
            while (window.pollEvent(e))
            {
                if (e.type == Event::Closed)
                    window.close();
                if (e.type == Event::KeyPressed)
                {
                    if (e.key.code == Keyboard::Num1)
                        startNewGame(colorNum, nextColorNum, scores, level, rowsCleared, playing);
                    else if (e.key.code == Keyboard::Num2)
                        showHighScores(highScores, displaying[0], ss);
                    else if (e.key.code == Keyboard::B && displaying[0])
                        displaying[0] = 0;
                    else if (e.key.code == Keyboard::Num3)
                        showHelp(displaying[1], ss);
                    else if (e.key.code == Keyboard::B && displaying[1])
                        displaying[1] = 0;
                    else if (e.key.code == Keyboard::C && displaying[2])
                        displaying[2] = 0;
                    else if (e.key.code == Keyboard::Num4)
                        window.close();
                    else if (isPaused && e.key.code == Keyboard::Num5)
                        playing = 1, isPaused = 0;
                }
            }

            window.draw(menuBox);
            menuText.setString(ss.str());
            window.draw(menuText);
        }
        else
        {
            float time = clock.getElapsedTime().asSeconds();
            clock.restart();
            timer += time;
            interval += time;
            showingScores = 0;

            if (interval <= 5 * 60)
                afterIntervalRows = rowsCleared;
            else
            {
                interval = 0;
                if (beforeIntervalRows == afterIntervalRows)
                    difficultyLevel++;
                beforeIntervalRows = rowsCleared;
            }

            Event e;
            while (window.pollEvent(e))
            {
                if (e.type == Event::Closed)
                    window.close();
                if (e.type == Event::KeyPressed)
                {
                    if (e.key.code == Keyboard::Up || e.key.code == Keyboard::K)
                        rotate = 1;
                    else if (e.key.code == Keyboard::Left || e.key.code == Keyboard::H)
                        delta_x = -1;
                    else if (e.key.code == Keyboard::Right || e.key.code == Keyboard::L)
                        delta_x = 1;
                }
            }

            // bombFalling = true;
            bombFalling = rand() % 1000 / 100.f < 0.5;

            if (Keyboard::isKeyPressed(Keyboard::P) || Keyboard::isKeyPressed(Keyboard::Escape))
                playing = 0, isPaused = 1;

            if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::J) || currentPiece.ch == 'B')
                delay=0.05;

            if (Keyboard::isKeyPressed(Keyboard::Space))
                hardDrop();
            else
                fallingPiece(timer, delay, colorNum, nextColorNum, heldColorNum, hasHeldPiece, bombFalling);
            if (Keyboard::isKeyPressed(Keyboard::C) && !hasHeldPiece)
                handleHolding(heldColorNum, colorNum, nextColorNum, hasHeldPiece);
            handleMovement(delta_x);
            updateShadow();
            if (rotate)
                rotatePiece();
            checkRows(level, rowsCleared, scores);

            window.draw(piecesBox);
            window.draw(frame);

            if (rowsCleared > 5)
                beginner = 0;

            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++)
                {
                    if (gameGrid[i][j] == 0)
                        continue;
                    sprite.setTextureRect(IntRect((gameGrid[i][j] - 1) * 18, 0, 18, 18));
                    sprite.setPosition(j * 18, i * 18);
                    sprite.move(236, 31); //offset
                    window.draw(sprite);
                }

            if (currentPiece.ch != 'B')
            {
                for (int i = 0; i < 4; i++)
                {
                    sprite.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
                    sprite.setPosition(currentPiece.coords[i][0] * 18, currentPiece.coords[i][1] * 18);
                    sprite.move(236, 31);
                    window.draw(sprite);
                }
                for (int i = 0; i < 4; i++)
                {
                    shadowBlock.setOutlineColor(colors[colorNum]);
                    shadowBlock.setPosition(shadowPiece.coords[i][0] * 18, shadowPiece.coords[i][1] * 18);
                    shadowBlock.move(236, 31);
                    window.draw(shadowBlock);
                }
            }
            else
            {
                bomb.setColor(colors[colorNum]);
                bomb.setPosition(currentPiece.coords[0][0] * 18, currentPiece.coords[0][1] * 18);
                bomb.move(236, 31);
                window.draw(bomb);
            }

            delay = 0.3 - (level - 1) * 0.02 - difficultyLevel * 0.03, delta_x = 0, rotate = 0;
            level = rowsCleared / 10 + 1;

            if (delay < 0.05)
                delay = 0.05;

            for (int i = 0; i < N; i++)
                for (int j = 0; j < M; j++)
                {
                    gridSquare.setPosition(Vector2f(i * 18, j * 18));
                    gridSquare.move(236, 31);
                    gridSquare.setFillColor(Color::Transparent);
                    window.draw(gridSquare);
                }

            for (int i = 0; i < N; i++)
                for (int j = M - 1; j >= M - difficultyLevel; j--)
                {
                    gridSquare.setPosition(Vector2f(i * 18, j * 18));
                    gridSquare.move(236, 31);
                    gridSquare.setFillColor(Color(90, 90, 90));
                    window.draw(gridSquare);
                }

            window.draw(statsBox);
            window.draw(nextPieceBox);
            window.draw(nextPieceText);
            window.draw(heldPieceBox);
            window.draw(heldPieceText);

            for (int i = 0; i < 4; i++)
            {
                sprite.setTextureRect(IntRect(nextColorNum * 18, 0, 18, 18));
                sprite.setPosition(nextPiece.coords[i][0] * 18 + nextPieceText.getPosition().x + (nextPiece.ch != 'O' ? 35.f : 45.f), nextPiece.coords[i][1] * 18 + nextPieceText.getPosition().y + 39.f);
                window.draw(sprite);
            }

            for (int i = 0; i < 4; i++)
            {
                sprite.setTextureRect(IntRect(heldColorNum * 18, 0, 18, 18));
                sprite.setPosition(heldPiece.coords[i][0] * 18 + heldPieceBox.getPosition().x + (heldPiece.ch != 'O' ? 35.f : 45.f), heldPiece.coords[i][1] * 18 + heldPieceText.getPosition().y + 39.f);
                window.draw(sprite);
            }

            window.draw(linesBox);
            ss.clear();
            ss.str("");
            ss << "Lines Cleared\n" << rowsCleared;
            linesText.setString(ss.str());
            window.draw(linesText);

            window.draw(levelBox);
            ss.clear();
            ss.str("");
            ss << "Level\n" << level;
            levelText.setString(ss.str());
            window.draw(levelText);

            window.draw(scoreBox);
            ss.clear();
            ss.str("");
            ss << "Score\n" << scores;
            scoreText.setString(ss.str());
            window.draw(scoreText);
        }

        window.display();
    }

    return 0;
}
