#include <iostream>
#include <raylib.h>

using namespace std;

// Define color
Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

// Initialize player scores
int player_score = 0;
int cpu_score = 0;

// Ball class
class Ball
{
public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    // Draw the ball
    void Draw()
    {
        DrawCircle(x, y, radius, Yellow);
    }

    // Update the ball's position
    void Update(Sound s)
    {
        x += speed_x;
        y += speed_y;

        // Check for collision with top or bottom of the screen
        if (y + radius >= GetScreenHeight() || y - radius <= 0)
        {
            speed_y *= -1; // Reverse the y direction
        }

        // Check for collision with the right edge (CPU wins)
        if (x + radius >= GetScreenWidth())
        {
            PlaySound(s);
            cpu_score++; // Increase CPU score
            ResetBall(); // Reset the ball's position
        }

        // Check for collision with the left edge (Player wins)
        if (x - radius <= 0)
        {
            PlaySound(s);
            player_score++; // Increase player score
            ResetBall();    // Reset the ball's position
        }
    }

    // Reset the ball's position and randomize its speed
    void ResetBall()
    {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = {-1, 1};
        speed_x *= speed_choices[GetRandomValue(0, 1)];
        speed_y *= speed_choices[GetRandomValue(0, 1)];
    }
};

// Paddle class
class Paddle
{
protected:
    // Limit the paddle's movement within the screen boundaries
    void LimitMovement()
    {
        if (y <= 0)
        {
            y = 0;
        }
        if (y + height >= GetScreenHeight())
        {
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;
    float width, height;
    int speed;

    // Draw the paddle
    void Draw()
    {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
    }

    // Update the paddle's position based on user input
    void Update()
    {
        if (IsKeyDown(KEY_UP))
        {
            y -= speed; // Move paddle up
        }
        if (IsKeyDown(KEY_DOWN))
        {
            y += speed; // Move paddle down
        }
        LimitMovement(); // Limit paddle movement within screen boundaries
    }
};

// CPU Paddle class inheriting from Paddle class
class CpuPaddle : public Paddle
{
private:
    int reaction_delay = 30; // Reaction delay frames
    int current_delay = 0;   // Current delay counter

public:
    // Update the CPU paddle's position based on the ball's y position
    void Update(int ball_y)
    {
        if (current_delay > 0)
        {
            current_delay--; // Decrement the delay counter
        }
        else
        {
            if (y + height / 2 > ball_y)
            {
                y = y - speed; // Move paddle up
            }
            if (y + height / 2 < ball_y)
            {
                y = y + speed; // Move paddle down
            }
            LimitMovement(); // Limit paddle movement within screen boundaries
        }
    }

    // Reset the reaction delay counter
    void ResetReactionDelay()
    {
        current_delay = reaction_delay;
    }
};

Ball ball;
Paddle player;
CpuPaddle cpu;

int main()
{
    const int screen_width = 1000;
    const int screen_height = 620;
    Sound ballSound;
    Sound wallSound;

    // Initialize audio device
    InitAudioDevice();

    // Load sound files
    ballSound = LoadSound("../sounds/ball.mp3");
    wallSound = LoadSound("../sounds/wall.mp3");

    // Initialize window
    InitWindow(screen_width, screen_height, "PingPong - PC Edition");

    // Set target frames per second (FPS)
    SetTargetFPS(60);

    // Initialize ball properties
    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 10;
    ball.speed_y = 10;

    // Initialize player paddle properties
    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 11;

    // Initialize CPU paddle properties
    cpu.height = 120;
    cpu.width = 25;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 5;

    bool game_started = false; // Game start flag
    bool game_over = false;    // Game over flag

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            game_started = true;
        }

        if (game_started && !game_over)
        {
            BeginDrawing();

            ClearBackground(Dark_Green);

            if (IsKeyPressed(KEY_ESCAPE))
            {
                game_over = true;
            }

            if (!game_over)
            {
                // Updating
                ball.Update(wallSound);
                player.Update();
                cpu.Update(ball.y);

                // Checking for collisions with player and CPU paddles
                if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height}))
                {
                    PlaySound(ballSound);
                    ball.speed_x *= -1; // Reverse the ball's x direction
                }

                if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height}))
                {
                    PlaySound(ballSound);
                    ball.speed_x *= -1; // Reverse the ball's x direction
                }

                // Drawing
                DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Green);
                DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);
                DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
                ball.Draw();
                cpu.Draw();
                player.Draw();
                DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
                DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);
            }
            else
            {
                // Game over state
                DrawText("Game Over!", screen_width / 2 - MeasureText("Game Over!", 40) / 2, screen_height / 2 - 40, 40, WHITE);
                DrawText("Press Enter to Restart", screen_width / 2 - MeasureText("Press Enter to Restart", 20) / 2, screen_height / 2, 20, WHITE);

                if (IsKeyPressed(KEY_ENTER))
                {
                    // Reset game
                    game_over = false;
                    game_started = false;
                    player_score = 0;
                    cpu_score = 0;
                }
            }

            EndDrawing();
        }
        else
        {
            // Intro screen
            BeginDrawing();

            ClearBackground(Dark_Green);
            DrawText("PingPong", screen_width / 2 - MeasureText("Pingpong", 60) / 2, screen_height / 2 - 60, 60, WHITE);
            DrawText("Press Enter to Start", screen_width / 2 - MeasureText("Press Enter to Start", 20) / 2, screen_height / 1.9, 20, WHITE);
            DrawText("Created by Rafay Khattak", screen_width / 2 - MeasureText("Press Enter to Start", 20) / 2, screen_height / 40, 20, WHITE);

            if (IsKeyPressed(KEY_ENTER))
            {
                game_started = true;
            }

            EndDrawing();
        }
    }

    // Unload sound files and close audio device
    UnloadSound(ballSound);
    UnloadSound(wallSound);
    CloseAudioDevice();

    // Close window
    CloseWindow();

    return 0;
}
