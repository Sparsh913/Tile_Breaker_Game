#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "fssimplewindow.h"
#include "yssimplesound.h"
#include "ysglfontdata.h"
#include <algorithm>
#include <set>
#include <fstream> 
#include "rendering_module_main.cpp"
#include "demo_additional.cpp"
#include "file_management.cpp"

int main(){
    std::string playerName;
    if (playerName.empty()) 
    {
        std::cout << "Enter your name: ";
        std::cin >> playerName;
    }

    static YsSoundPlayer SoundPlayer;
    static YsSoundPlayer::SoundData paddleSound;
    static YsSoundPlayer::SoundData bg_music;
    srand(time(NULL));
    FsOpenWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);

    bool terminate = false; // Add terminate flag
    int game_state;
    // std::string playerName;


    while (!terminate) { // Keep the game running until the terminate flag is set to true
        int game_state = displayMainMenu(); // Tariq's code will handle instructions and leaderboard
        std::cout << "Game State: " << game_state << std::endl;

        if (game_state == 0) { // New Game
            
            srand(static_cast<unsigned int>(time(nullptr)));

            // Initialize sound files
            SoundPlayer.Start();
            if (YSOK != paddleSound.LoadWav("bounce.wav")) {
                std::cerr << "Error: Could not load paddle_hit.wav" << std::endl;
            }
            if (YSOK != bg_music.LoadWav("bg_music.wav")) {
                std::cerr << "Error: Could not load bg_music.wav" << std::endl;
            }
            SoundPlayer.PlayBackground(bg_music);

            Paddle paddle(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 50, 100, 10, 10);
            Ball ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 8, 3, -3);

            ball.LoadSounds("bounce.wav");

            std::vector<Tile> tiles;
            ParticleSystem particleSystem;
            InitializeTiles(tiles);

            int lives = num_lives(false);
            int score = 0;

            while (true) {
                FsPollDevice();
                int key = FsInkey();
                if (key == FSKEY_ESC) {
                    int mid_state = displayPauseMenu(); // Pause Menu Logic
                    if (mid_state == 0) { // Exit game
                        terminate = true; // Set terminate flag to true
                        break;
                    } else if (mid_state == 1) { // Resume game
                        continue;
                    } else if (mid_state == 2) { // Start new game
                        ResetGame(ball, paddle, tiles);
                        lives = num_lives(true);
                    }
                }

                if (key == FSKEY_LEFT) {
                    paddle.MoveLeft();
                }
                if (key == FSKEY_RIGHT) {
                    paddle.MoveRight();
                }

                ball.Move();

                if (ball.CheckPaddleCollision(paddle)) {
                    SoundPlayer.PlayOneShot(paddleSound);
                }

                bool specialTileHit = false;
                int specialTileType = -1;

                for (auto &tile : tiles) {
                    if (!tile.destroyed &&
                        ball.x + ball.radius >= tile.x &&
                        ball.x - ball.radius <= tile.x + tile.width &&
                        ball.y + ball.radius >= tile.y &&
                        ball.y - ball.radius <= tile.y + tile.height) {
                        specialTileHit = tile.isSpecial;
                        specialTileType = tile.Hit();
                        ball.speedY = -ball.speedY;
                        particleSystem.Explode(tile.x + tile.width / 2, tile.y + tile.height / 2);
                        SoundPlayer.PlayOneShot(paddleSound);
                        score++;
                    }
                }

                if (AreAllTilesDestroyed(tiles, ball, paddle)) {
                    level_up(ball.speedX, ball.speedY, paddle.width);
                    paddle.x = SCREEN_WIDTH / 2 - paddle.width / 2;
                    paddle.y = SCREEN_HEIGHT - 50;
                    ball.x = SCREEN_WIDTH / 2;
                    ball.y = SCREEN_HEIGHT / 2;
                    InitializeTiles(tiles);
                }

                if (ball.IsOutOfBounds()) {
                    lives = num_lives(true);
                    if (lives == 0) {
                        std::cout << "Game Over!" << std::endl;
                        // std::string playerName;
                        // std::cout << "Enter your name: ";
                        // std::cin >> playerName;
                        std::ofstream leaderboardFile("leaderboard.csv", std::ios::app);
                        if (leaderboardFile.is_open()) {
                            leaderboardFile << playerName << "," << score << "\n";
                            leaderboardFile.close();
                            std::cout << "Your score has been saved to the leaderboard!" << std::endl;
                        } else {
                            std::cerr << "Error: Could not open leaderboard file." << std::endl;
                        }
                        break;
                    } else {
                        paddle.x = SCREEN_WIDTH / 2 - paddle.width / 2;
                        paddle.y = SCREEN_HEIGHT - 50;
                        ball.x = SCREEN_WIDTH / 2;
                        ball.y = SCREEN_HEIGHT / 2;
                        ball.speedX = 3;
                        ball.speedY = -3;
                    }
                }

                particleSystem.Update();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                RenderBackground();
                display_game_stats(score);
                paddle.Render();
                ball.Render();
                for (auto &tile : tiles) {
                    tile.Render();
                }
                particleSystem.Render();
                FsSwapBuffers();
                FsSleep(10);
            }
            SoundPlayer.End();
        } else if (game_state == 1) { // Instructions
            displayInstructions();
        } else if (game_state == 2) { // Leaderboard
            displayLeaderboard();
        } else if (game_state == -1) { // Exit
            terminate = true; // Set terminate flag to true
        }
    }

    return 0;
}
