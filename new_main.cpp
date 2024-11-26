#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "fssimplewindow.h"
#include "yssimplesound.h"
#include "rendering_module.h"
#include "physics_module.h"
#include "demo_additional.cpp"
#include "file_management.cpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main() {
    FsOpenWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
    int game_state = displayMainMenu();
    if (game_state == 0) {
        srand(static_cast<unsigned int>(time(nullptr)));

        YsSoundPlayer soundPlayer;
        YsSoundPlayer::SoundData paddleSound;
        soundPlayer.Start();
        if (YSOK != paddleSound.LoadWav("bounce.wav")) {
            std::cerr << "Error: Could not load paddle_hit.wav" << std::endl;
        }

        Paddle paddle(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 50, 100, 10, 10);
        Ball ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 8, 2, -2);
        ball.LoadSounds("bounce.wav");

        std::vector<Tile> tiles;
        ParticleSystem particleSystem;

        initializeTiles(tiles, SCREEN_WIDTH, SCREEN_HEIGHT);

        int lives = num_lives(false);
        int score = 0;

        while (true) {
            FsPollDevice();
            int key = FsInkey();
            if (key == FSKEY_ESC) {
                int mid_state = displayPauseMenu();
                if (mid_state == 0) {
                    break;
                } 
                else if (mid_state == 1) {
                    continue;
                } 
                else if (mid_state == 2) {
                    ResetGame(ball, paddle, tiles);
                    score = 0;
                    lives = num_lives(true);
                }
            }

            updateGameState(key, paddle, ball, tiles, particleSystem, soundPlayer, paddleSound);

            bool specialTileHit = false;
            for (auto &tile : tiles) {
                if (!tile.destroyed && checkCollision(ball, tile)) {
                    tile.destroyed = true;
                    ball.speedY = -ball.speedY;
                    particleSystem.Explode(tile.x + tile.width / 2, tile.y + tile.height / 2);
                    soundPlayer.PlayOneShot(paddleSound);
                    score++;
                    if (tile.IsSpecial()) {
                        specialTileHit = true;
                        //apply_power_up(getRandomPowerUp());
                    }
                }
            }

            if (AreAllTilesDestroyed(tiles)) {
                bool all_destroyed = true;
                check_tile_status(all_destroyed); // to be defined in vasvi's code
            }

            if (ball.IsOutOfBounds()) {
                // std::cout << "Game Over!" << std::endl;
                bool fell = true;
                check_ball_status(fell);
                lives = num_lives(); // to be defined in vasvi's code
                if (lives == 0) {
                    std::cout << "Game Over!" << std::endl;
                    break;
                }
            }

            //check_power_up_timer();

            renderGame(paddle, ball, tiles, particleSystem);
            display_lives_on_screen(lives);
            display_score_on_screen(score);

            FsSwapBuffers();
            FsSleep(10);
        }
        soundPlayer.End();
    }

    return 0;
}