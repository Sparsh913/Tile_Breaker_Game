
// #include <iostream>
// #include <vector>
// #include <cstdlib>
// #include <ctime>
// #include <cmath>
// #include "fssimplewindow.h"
// #include "yssimplesound.h"
// #include "ysglfontdata.h"
// #include <algorithm>
// #include <set>
// // #include "rendering_module_main.h"
// // #include "demo_additional.h"
// // #include "file_management.h"
// #include <fstream> 
// #include "rendering_module_main.cpp"
// #include "demo_additional.cpp"
// #include "file_management.cpp"

// //TODO: Implement the following functions in the respective files
// // TODO: Nishanth - Split rendering_module_main.cpp into rendering_module.cpp and physics_module.cpp


// int main(){
//     static YsSoundPlayer SoundPlayer;
//     static YsSoundPlayer::SoundData paddleSound;
//     static YsSoundPlayer::SoundData bg_music;
//     srand(time(NULL));
//     FsOpenWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
//     int game_state = displayMainMenu(); // Tariq's code will handle instructions and leaderboard
//     std::cout << "Game State: " << game_state << std::endl;
//     if (game_state == 0) {
//         srand(static_cast<unsigned int>(time(nullptr)));

//         // Initialize sound files
//         soundPlayer.Start();
//         if (YSOK != paddleSound.LoadWav("bounce.wav")) {
//             std::cerr << "Error: Could not load paddle_hit.wav" << std::endl;
//         }
//         if (YSOK != bg_music.LoadWav("bg_music.wav")) {
//             std::cerr << "Error: Could not load bg_music.wav" << std::endl;
//         }
//         soundPlayer.PlayBackground(bg_music);
        
//         Paddle paddle(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 50, 100, 10, 10);
//         Ball ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 8, 3,-3);

//         // Load ball's wall collision sound
//         ball.LoadSounds("bounce.wav");

//         std::vector<Tile> tiles;
//         ParticleSystem particleSystem;

//         // Initialize tiles
//         int totalTileWidth = TILE_COLUMNS * TILE_WIDTH;
//         int totalTileHeight = TILE_ROWS * TILE_HEIGHT;

//         int startX = (SCREEN_WIDTH - totalTileWidth) / 2;  // Center horizontally
//         int startY = 50;  // Fixed vertical position

//         int lives = num_lives(false); // to be defined in vasvi's code; the ball has not fallen yet output initial number of lives
//         InitializeTiles(tiles);

//         int score = 0;
        
//         while (true) {
//             FsPollDevice();
//             int key = FsInkey();
//             if (key == FSKEY_ESC) {
//                 // break;
//                 int mid_state = displayPauseMenu(); // to be defined in tariq's code
//                 // exit game
//                 if (mid_state == 0) {
//                     break;
//                 }
//                 // resume game
//                 else if (mid_state == 1) {
//                     continue;
//                 }
//                 // start new game
//                 else if (mid_state == 2) {
//                     // break;
//                     ResetGame(ball, paddle, tiles);
//                     // score = 0;
//                     lives = num_lives(true); // to be defined in vasvi's code; the ball has fallen output updated number of lives
//                 }
//             }

//             if (key == FSKEY_LEFT) {
//                 paddle.MoveLeft();
//             }
//             if (key == FSKEY_RIGHT) {
//                 paddle.MoveRight();
//             }

//             ball.Move();

//             // Check collision with paddle
//             if (ball.CheckPaddleCollision(paddle)) {
//                 soundPlayer.PlayOneShot(paddleSound);  // Play paddle collision sound
//             }

//             // // Check collision with walls
//             // if (ball.CheckScreenCollision()) {
//             //     soundPlayer.PlayOneShot(paddleSound);  // Play wall collision sound
//             // }

//             bool specialTileHit = false;
//             int specialTileType = -1;
            
//             for (auto &tile : tiles) {
//                 if (!tile.destroyed && ball.x + ball.radius >= tile.x && ball.x - ball.radius <= tile.x + tile.width &&
//                     ball.y + ball.radius >= tile.y && ball.y - ball.radius <= tile.y + tile.height) {
//                     specialTileHit = tile.isSpecial;
//                     specialTileType = tile.Hit();
//                     ball.speedY = -ball.speedY;
//                     particleSystem.Explode(tile.x + tile.width / 2, tile.y + tile.height / 2);
//                     SoundPlayer.PlayOneShot(paddleSound);
//                     score++;
//                 }
//             }

//             // This is just a dummy functionality -> to be included in vasvi's code and removed from here
//             if (specialTileHit) {
//                 std::cout << "Special Tile Hit! Type: " << specialTileType << std::endl;
//                 // Apply power-up using the function from demo_additional.cpp
//                 switch (specialTileType) {
//                     case 0:
//                         lives = life_powerup(0);
//                         break;
//                     case 1:
//                         apply_power_up(ball.speedX, ball.speedY, paddle.width, 1);
//                         break;
//                     case 2:
//                         apply_power_up(ball.speedX, ball.speedY, paddle.width, 2);
//                         break;
//                     case 3:
//                         apply_power_up(ball.speedX, ball.speedY, paddle.width, 3);
//                         break;
//                     case 4:
//                         apply_power_up(ball.speedX, ball.speedY, paddle.width, 4);
//                         break;
//                 }
//             }

//             if (AreAllTilesDestroyed(tiles, ball, paddle)) {
//                 // std::cout << "All tiles are destroyed! You win!" << std::endl;
//                 // break;
//                 bool all_destroyed = true;
//                 level_up(ball.speedX, ball.speedY, paddle.width);
//                 paddle.x = SCREEN_WIDTH / 2 - paddle.width / 2;
//                 paddle.y = SCREEN_HEIGHT - 50;
//                 ball.x = SCREEN_WIDTH / 2;
//                 ball.y = SCREEN_HEIGHT / 2;
//                 InitializeTiles(tiles);
//             }

//             // Check if ball is out of bounds
//             if (ball.IsOutOfBounds()) {
//                 bool fell = true;
//                 lives = num_lives(fell); // Update number of lives since ball fell

//                 if (lives == 0) {
//                     std::cout << "Game Over!" << std::endl;
//                     std::string playerName;
//                     std::cout << "Enter your name: ";
//                     std::cin >> playerName;
//                     std::ofstream leaderboardFile("leaderboard.csv", std::ios::app); // Open in append mode
//                     if (leaderboardFile.is_open()) 
//                     {
//                         leaderboardFile << playerName << "," << score << "\n"; // Write name and score
//                         leaderboardFile.close();
//                     std::cout << "Your score has been saved to the leaderboard!" << std::endl;
//                     } 
//                     else 
//                     {
//                         std::cerr << "Error: Could not open leaderboard file." << std::endl;
//                     }

//                     break; // End game if no lives are left
//                 } else {
//                     // Reset ball and paddle position to continue the game
//                     paddle.x = SCREEN_WIDTH / 2 - paddle.width / 2;
//                     paddle.y = SCREEN_HEIGHT - 50;
//                     ball.x = SCREEN_WIDTH / 2;
//                     ball.y = SCREEN_HEIGHT / 2;
//                     ball.speedX = 3;
//                     ball.speedY = -3;
//                 }
//             }

//             // Update particles
//             particleSystem.Update();

//             // Render everything
//             glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//             RenderBackground();
//             display_game_stats(score);
//             paddle.Render();
//             ball.Render();
//             for (auto &tile : tiles) {
//                 tile.Render();
//             }
//             particleSystem.Render();
//             FsSwapBuffers();
//             FsSleep(10);
//         }
//         soundPlayer.End();
//     }
//     if(game_state == 1){

//         displayInstructions();

//     }
//     if (game_state == 2) {
//         std::vector<std::string> leaderboard = readLeaderboardFromFile("leaderboard.csv");
//         displayLeaderboard();
//     }
// }


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
    static YsSoundPlayer SoundPlayer;
    static YsSoundPlayer::SoundData paddleSound;
    static YsSoundPlayer::SoundData bg_music;
    srand(time(NULL));
    FsOpenWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);

    bool terminate = false; // Add terminate flag
    int game_state;
    std::string playerName;


    while (!terminate) { // Keep the game running until the terminate flag is set to true
        int game_state = displayMainMenu(); // Tariq's code will handle instructions and leaderboard
        std::cout << "Game State: " << game_state << std::endl;

        if (game_state == 0) { // New Game
            if (playerName.empty()) {
                std::cout << "Enter your name: ";
                std::cin >> playerName;
            }
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
