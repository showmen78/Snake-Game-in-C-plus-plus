#include <iostream>
#include "raylib.h"
#include <raymath.h>
#include <algorithm>
#include <deque>
#include <vector>


// Define global constants and variables
const int GRID_SIZE = 25;           // Number of grid cells per row/column
const int CELL_SIZE = 20;           // Size of each cell in pixels
const int BORDER_OFFSET = 50;       // Offset for the game grid
const Color BG_COLOR = {3, 87, 145,255 };  // Background color
const Color SNAKE_COLOR = {220, 222, 220, 255 }; // Snake color 
const Color FOOD_COLOR = { 200, 50, 50, 255 };  // Food color 



// Helper function to check if a vector exists in a deque
bool IsPositionInDeque(Vector2 position, const std::deque<Vector2>& body) {
    for (const auto& segment : body) {
        if (Vector2Equals(segment, position)) {
            return true;
        }
    }
    return false;
}

// Snake class
class Snake {
public:
    std::deque<Vector2> body = { {9, 10}, {8, 10}, {7, 10} }; // Initial snake segments
    Vector2 direction = { 1, 0 };                          // Initial direction (moving right)
    bool grow = false;                                   // Grow flag to increase snake size

    // Draw the snake
    void Draw() {
        for (const auto& segment : body) {
            Rectangle rect = {
                BORDER_OFFSET + segment.x * CELL_SIZE,
                BORDER_OFFSET + segment.y * CELL_SIZE,
                (float)CELL_SIZE-1, (float)CELL_SIZE-1
            };
            DrawRectangleRec(rect, SNAKE_COLOR);
        }
    }

    // Update the snake's position
    void Update() {
        body.push_front(Vector2Add(body.front(), direction)); // Move head in current direction
        if (!grow) {
            body.pop_back(); // Remove the last segment if not growing
        }
        else {
            grow = false; // Reset grow flag
        }
    }

    // Reset the snake to its initial state
    void Reset(float& MOVE_INTERVAL) {
        body = { {9, 10}, {8, 10}, {7, 10} };
        direction = { 1, 0 };
        MOVE_INTERVAL = 0.2f;
    }
};

// Food class
class Food {
public:
    Vector2 position; // Current position of the food

    // Constructor
    Food(const std::deque<Vector2>& snakeBody) {
        position = GenerateRandomPosition(snakeBody);
    }

    // Draw the food as a rectangle
    void Draw() {
        Rectangle foodRect = {
            BORDER_OFFSET + position.x * CELL_SIZE,
            BORDER_OFFSET + position.y * CELL_SIZE,
            (float)CELL_SIZE, (float)CELL_SIZE
        };
        DrawRectangleRec(foodRect, FOOD_COLOR);
    }

    // Generate a random position for the food
    Vector2 GenerateRandomPosition(const std::deque<Vector2>& snakeBody) {
        Vector2 newPosition;
        do {
            newPosition = { (float) GetRandomValue(0, GRID_SIZE - 1), (float) GetRandomValue(0, GRID_SIZE - 1) };
        } while (IsPositionInDeque(newPosition, snakeBody)); // Ensure food doesn't spawn on the snake
        return newPosition;
    }
};

// Game class
class SnakeGame {
public:
    Snake snake;        // Snake object
    Food food = Food(snake.body); // Food object
    bool isRunning = true; // Game running state
    int score = 0, max_score=0;         // Player's score

    // Draw the game elements
    void Draw() {
        food.Draw();
        snake.Draw();
    }

    // Update the game state
    void Update(float& MOVE_INTERVAL) {
        if (isRunning) {
            snake.Update();
            CheckFoodCollision(MOVE_INTERVAL);
            CheckWallCollision(MOVE_INTERVAL);
            CheckSelfCollision(MOVE_INTERVAL);
        }
    }

    // Check if the snake eats the food
    void CheckFoodCollision(float& MOVE_INTERVAL) {
        if (Vector2Equals(snake.body.front(), food.position)) {
            food.position = food.GenerateRandomPosition(snake.body);
            snake.grow = true;
            score += 1; // Increase score
            max_score = std::max(score, max_score);
            MOVE_INTERVAL -= 0.01f;
            
        }
    }

    // Check if the snake hits the wall
    void CheckWallCollision(float& MOVE_INTERVAL) {
        Vector2 head = snake.body.front();
        if (head.x < 0 || head.x >= GRID_SIZE || head.y < 0 || head.y >= GRID_SIZE) {
            GameOver(MOVE_INTERVAL);
        }
    }

    // Check if the snake collides with itself
    void CheckSelfCollision(float& MOVE_INTERVAL) {
        std::deque<Vector2> bodyWithoutHead = snake.body;
        bodyWithoutHead.pop_front();
        if (IsPositionInDeque(snake.body.front(), bodyWithoutHead)) {
            GameOver(MOVE_INTERVAL);
        }
    }

    // Handle game over scenario
    void GameOver(float& MOVE_INTERVAL) {
        isRunning = false;
        snake.Reset(MOVE_INTERVAL);
        food = Food(snake.body); // Reset food position
        score = 0; // Reset score
    }
};

int main() {
    // Initialize the window
    InitWindow(2 * BORDER_OFFSET + GRID_SIZE * CELL_SIZE, 2 * BORDER_OFFSET + GRID_SIZE * CELL_SIZE, "Snake Game by SHOWMEN DEY");
    SetTargetFPS(60);

    SnakeGame game; // Create the game object
    double lastMoveTime = 0;
    float MOVE_INTERVAL = 0.2; // Time interval between moves

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BG_COLOR); // Clear screen with background color

        // Handle movement timing
        if (GetTime() - lastMoveTime >= MOVE_INTERVAL) {
            game.Update(MOVE_INTERVAL);
            lastMoveTime = GetTime();
        }

        // Handle player input for direction
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
            game.snake.direction = { 0, -1 };
            game.isRunning = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
            game.snake.direction = { 0, 1 };
            game.isRunning = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
            game.snake.direction = { -1, 0 };
            game.isRunning = true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
            game.snake.direction = { 1, 0 };
            game.isRunning = true;
        }

        // Draw game elements
        DrawRectangleLinesEx(
            Rectangle{ (float)BORDER_OFFSET - 5, (float)BORDER_OFFSET - 5,
                      GRID_SIZE * CELL_SIZE + 10, GRID_SIZE * CELL_SIZE + 10 },
            5, SNAKE_COLOR // Draw game boundary
        );
        DrawText("Snake Game", BORDER_OFFSET, 20, 30, SNAKE_COLOR);
        DrawText(TextFormat("Score: %i", game.score), BORDER_OFFSET, BORDER_OFFSET + GRID_SIZE * CELL_SIZE + 10, 20, SNAKE_COLOR);
        DrawText(TextFormat("MaxScore: %i", game.max_score), BORDER_OFFSET+300, BORDER_OFFSET + GRID_SIZE * CELL_SIZE + 10, 20, SNAKE_COLOR);

        game.Draw();

        EndDrawing();
    }

    CloseWindow(); // Close the window
    return 0;
}
