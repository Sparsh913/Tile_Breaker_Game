// #include <iostream>
// #include <vector>
// #include <cstdlib>
// #include <ctime>
// #include <cmath>
// #include "fssimplewindow.h"
// #include "yssimplesound.h"
// #include <algorithm>
// #include "rendering_module_main.cpp"
// #include "demo_additional.cpp"
// #include "file_management.cpp"

// //TODO: Implement the following functions in the respective files
// // TODO: Nishanth - Split rendering_module_main.cpp into rendering_module.cpp and physics_module.cpp

// // const int SCREEN_WIDTH = 800;
// // const int SCREEN_HEIGHT = 600;

// int main(){
//     FsOpenWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
//     int game_state = displayMainMenu(); // Tariq's code will handle instructions and leaderboard
//     if (game_state == 0) {
//         srand(static_cast<unsigned int>(time(nullptr)));

//         // Initialize sound files
//         soundPlayer.Start();
//         if (YSOK != paddleSound.LoadWav("bounce.wav")) {
//             std::cerr << "Error: Could not load paddle_hit.wav" << std::endl;
//         }

//         Paddle paddle(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 50, 100, 10, 10);
//         Ball ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 8, 2, -2);

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

        
//         for (int row = 0; row < TILE_ROWS; row++) {
//             for (int col = 0; col < TILE_COLUMNS; col++) {
//                 tiles.emplace_back(startX + col * TILE_WIDTH, startY + row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
//             }
//         }
//         // check whether all the tiles are destroyed or not

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
//                     score = 0;
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

//             bool specialTileHit = false;
//             int score = 0;
//             // Check collision with tiles
//             for (auto &tile : tiles) {
//                 if (!tile.destroyed && ball.x + ball.radius >= tile.x && ball.x - ball.radius <= tile.x + tile.width &&
//                     ball.y + ball.radius >= tile.y && ball.y - ball.radius <= tile.y + tile.height) {
//                     tile.destroyed = true;
//                     ball.speedY = -ball.speedY;
//                     particleSystem.Explode(tile.x + tile.width / 2, tile.y + tile.height / 2);
//                     soundPlayer.PlayOneShot(paddleSound);  // Play tile collision sound
//                     score ++;
//                     if (tile.IsSpecial()) {
//                         specialTileHit = true;
//                     }
//                 }
//             }

//             // Check if all tiles are destroyed
//             if (AreAllTilesDestroyed(tiles, ball, paddle)) {
//                 // std::cout << "All tiles are destroyed! You win!" << std::endl;
//                 // break;
//                 bool all_destroyed = true;
//                 check_tile_status(all_destroyed); // to be defined in vasvi's code
//             }

//             // Check if ball is out of bounds
//             if (ball.IsOutOfBounds()) {
//                 // std::cout << "Game Over!" << std::endl;
//                 bool fell = true;
//                 check_ball_status(fell);
//                 lives = num_lives(); // to be defined in vasvi's code
//                 if (lives == 0) {
//                     std::cout << "Game Over!" << std::endl;
//                     break;
//                 }
//             }

//             // Update particles
//             particleSystem.Update();

//             // Render everything
//             glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//             RenderBackground();
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

//         return 0;

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
// #include "rendering_module_main.h"
// #include "demo_additional.h"
// #include "file_management.h"

#include "rendering_module_main.cpp"
#include "demo_additional.cpp"
#include "file_management.cpp"

//TODO: Implement the following functions in the respective files
// TODO: Nishanth - Split rendering_module_main.cpp into rendering_module.cpp and physics_module.cpp
// static YsSoundPlayer SoundPlayer;
// static YsSoundPlayer::SoundData paddleSound;

// const int SCREEN_WIDTH = 800;
// const int SCREEN_HEIGHT = 600;
// int totalTileWidth = TILE_COLUMNS * TILE_WIDTH;
// int totalTileHeight = TILE_ROWS * TILE_HEIGHT;

int main(){
    static YsSoundPlayer SoundPlayer;
    static YsSoundPlayer::SoundData paddleSound;
    srand(time(NULL));
    FsOpenWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
    int game_state = displayMainMenu(); // Tariq's code will handle instructions and leaderboard
    if (game_state == 0) {
        srand(static_cast<unsigned int>(time(nullptr)));

        // Initialize sound files
        soundPlayer.Start();
        if (YSOK != paddleSound.LoadWav("bounce.wav")) {
            std::cerr << "Error: Could not load paddle_hit.wav" << std::endl;
        }
        
        Paddle paddle(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 50, 100, 10, 10);
        Ball ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 8, 2, -2);

        // Load ball's wall collision sound
        ball.LoadSounds("bounce.wav");

        std::vector<Tile> tiles;
        ParticleSystem particleSystem;

        // Initialize tiles
        int totalTileWidth = TILE_COLUMNS * TILE_WIDTH;
        int totalTileHeight = TILE_ROWS * TILE_HEIGHT;

        int startX = (SCREEN_WIDTH - totalTileWidth) / 2;  // Center horizontally
        int startY = 50;  // Fixed vertical position

        int lives = num_lives(false); // to be defined in vasvi's code; the ball has not fallen yet output initial number of lives

        
        // Randomly select 5 unique positions for special tiles
        std::set<int> specialTilePositions;
        while (specialTilePositions.size() < 5) {
            int position = std::rand() % (TILE_ROWS * TILE_COLUMNS);
            specialTilePositions.insert(position);
        }

        // Define colors for the special tiles
        float specialColors[5][3] = {
            {0.0f, 1.0f, 0.0f},  // Green
            {1.0f, 1.0f, 0.0f},  // Yellow
            {0.0f, 0.0f, 1.0f},  // Blue
            {1.0f, 0.5f, 0.0f},  // Orange
            {1.0f, 0.0f, 1.0f}   // Magenta
        };

        int tileIndex = 0;
        int specialIndex = 0;
        for (int row = 0; row < TILE_ROWS; row++) {
            for (int col = 0; col < TILE_COLUMNS; col++) {
                bool isSpecial = specialTilePositions.count(tileIndex) > 0;
                int rewardType = isSpecial ? specialIndex : 0;  // Assign a unique reward type to each special tile
                float* color = isSpecial ? specialColors[specialIndex++] : new float[3]{1.0f, 0.0f, 0.0f};  // Assign color
                tiles.emplace_back(startX + col * TILE_WIDTH, startY + row * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, isSpecial, rewardType, color);
                tileIndex++;
            }
        }
        // check whether all the tiles are destroyed or not

        while (true) {
            FsPollDevice();
            int key = FsInkey();
            if (key == FSKEY_ESC) {
                // break;
                int mid_state = displayPauseMenu(); // to be defined in tariq's code
                // exit game
                if (mid_state == 0) {
                    break;
                }
                // resume game
                else if (mid_state == 1) {
                    continue;
                }
                // start new game
                else if (mid_state == 2) {
                    // break;
                    ResetGame(ball, paddle, tiles);
                    // score = 0;
                    lives = num_lives(true); // to be defined in vasvi's code; the ball has fallen output updated number of lives
                }
            }

            if (key == FSKEY_LEFT) {
                paddle.MoveLeft();
            }
            if (key == FSKEY_RIGHT) {
                paddle.MoveRight();
            }

            ball.Move();

            // Check collision with paddle
            if (ball.CheckPaddleCollision(paddle)) {
                soundPlayer.PlayOneShot(paddleSound);  // Play paddle collision sound
            }

            bool specialTileHit = false;
            int specialTileType = -1;
            int score = 0;
            for (auto &tile : tiles) {
                if (!tile.destroyed && ball.x + ball.radius >= tile.x && ball.x - ball.radius <= tile.x + tile.width &&
                    ball.y + ball.radius >= tile.y && ball.y - ball.radius <= tile.y + tile.height) {
                    specialTileHit = tile.isSpecial;
                    specialTileType = tile.Hit();
                    ball.speedY = -ball.speedY;
                    particleSystem.Explode(tile.x + tile.width / 2, tile.y + tile.height / 2);
                    SoundPlayer.PlayOneShot(paddleSound);
                    score++;
                }
            }

            // This is just a dummy functionality -> to be included in vasvi's code and removed from here
            if (specialTileHit) {
                std::cout << "Special Tile Hit! Type: " << specialTileType << std::endl;
                // Process reward/penalty
                switch (specialTileType) {
                    case 0: /* Green tile logic */ break;
                    case 1: /* Yellow tile logic */ break;
                    case 2: /* Blue tile logic */ break;
                    case 3: /* Orange tile logic */ break;
                    case 4: /* Magenta tile logic */ break;
                }
            }

            // Check if all tiles are destroyed
            if (AreAllTilesDestroyed(tiles, ball, paddle)) {
                // std::cout << "All tiles are destroyed! You win!" << std::endl;
                // break;
                bool all_destroyed = true;
                level_up(); // to be defined in vasvi's code
            }

            // Check if ball is out of bounds
            if (ball.IsOutOfBounds()) {
                // std::cout << "Game Over!" << std::endl;
                bool fell = true;
                lives = num_lives(fell); // to be defined in vasvi's code
                if (lives == 0) {
                    std::cout << "Game Over!" << std::endl;
                    break;
                }
            }

            // Update particles
            particleSystem.Update();

            // Render everything
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            RenderBackground();
            paddle.Render();
            ball.Render();
            for (auto &tile : tiles) {
                tile.Render();
            }
            particleSystem.Render();
            FsSwapBuffers();
            FsSleep(10);
        }
        soundPlayer.End();
    }

        return 0;

}