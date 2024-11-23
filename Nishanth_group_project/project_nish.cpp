#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "fssimplewindow.h"
#include "ysglfontdata.h"

//Creating the physics and backbone of the code

//defining all the initial Values of the game

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define BALL_RADIUS 10
#define INITIAL_BALL_SPEED 3
#define BALL_SPEED_INCREASE 1.05


//Define the structure of the ball and the paddle
struct Paddle {
    double x, y;
};

struct Ball {
    double x, y;
    double dx, dy;
    double speed;
};


//Functions to draw the paddle and the ball in every frame
void DrawPaddle(const Paddle& paddle) {
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2d(paddle.x, paddle.y);
    glVertex2d(paddle.x + PADDLE_WIDTH, paddle.y);
    glVertex2d(paddle.x + PADDLE_WIDTH, paddle.y + PADDLE_HEIGHT);
    glVertex2d(paddle.x, paddle.y + PADDLE_HEIGHT);
    glEnd();
}

void DrawBall(const Ball& ball) {
    glColor3ub(255, 0, 0);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 64; ++i) {
        double angle = (double)i * 6.28318530718 / 64.0;
        double x = ball.x + cos(angle) * BALL_RADIUS;
        double y = ball.y + sin(angle) * BALL_RADIUS;
        glVertex2d(x, y);
    }
    glEnd();
}



//Drawing the score of the player: this is not correct yet
void DrawScore(int score)
{
    glColor3ub(255, 0, 0);  // red
    
    char scoreStr[50];
    sprintf(scoreStr, "Score: %d", score);
    
    glRasterPos2i(10, 20); 
    YsGlDrawFontBitmap12x16(scoreStr);
}


//updating the trajectory of the ball, need to include tiles after that structure or class is added
void UpdateBall(Ball& ball, const Paddle& paddle) {
    ball.x += ball.dx;
    ball.y += ball.dy;

    // Bounce off walls
    if (ball.x - BALL_RADIUS < 0 || ball.x + BALL_RADIUS > WINDOW_WIDTH) {
        ball.dx = -ball.dx;
    }
    if (ball.y - BALL_RADIUS < 0) {
        ball.dy = -ball.dy;
    }

    // Bounce off paddle
    if (ball.y + BALL_RADIUS > paddle.y && 
        ball.x > paddle.x && ball.x < paddle.x + PADDLE_WIDTH) {
        ball.dy = -fabs(ball.dy);
        // Increase ball speed
        ball.speed *= BALL_SPEED_INCREASE;
        ball.dx *= BALL_SPEED_INCREASE;
        ball.dy *= BALL_SPEED_INCREASE;
    }
}



// The Infinite for loop that ties the whole code/fuctions together

int main(void) {
    FsOpenWindow(16, 16, WINDOW_WIDTH, WINDOW_HEIGHT, 1);
    srand(time(NULL));

    Paddle paddle = {WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2, WINDOW_HEIGHT - PADDLE_HEIGHT - 10};
    Ball ball = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 
                 INITIAL_BALL_SPEED * double(rand() % 2 ? 1 : -1), 
                 -INITIAL_BALL_SPEED, INITIAL_BALL_SPEED};

    int score = 0;
    bool gameOver = false;

    while (!gameOver) {
        FsPollDevice();
        int key = FsInkey();
        if (FSKEY_ESC == key) {
            break;
        }

        // Move paddle
        if (FsGetKeyState(FSKEY_LEFT) && paddle.x > 0) {
            paddle.x -= 5;
        }
        if (FsGetKeyState(FSKEY_RIGHT) && paddle.x + PADDLE_WIDTH < WINDOW_WIDTH) {
            paddle.x += 5;
        }

        UpdateBall(ball, paddle);

        // Check for game over
        if (ball.y + BALL_RADIUS > WINDOW_HEIGHT) {
            gameOver = true;
        }

        // Increase score
        score++;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DrawPaddle(paddle);
        DrawBall(ball);

        // Draw score
        DrawScore(score);


        FsSwapBuffers();
        FsSleep(10);
    }
    return 0;
}