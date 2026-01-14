#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <map>
#include <string>

struct vec2
{
    float x, y;
    vec2() : x(0.0f), y(0.0f) {}
    vec2(float x, float y) : x(x), y(y) {}
};

struct vec2i
{
    int x, y;
    vec2i() : x(0), y(0) {}
    vec2i(int x, int y) : x(x), y(y) {}

    bool operator==(const vec2i &other) const
    {
        return x == other.x && y == other.y;
    }
};
struct vec3
{
    float r, g, b;
    vec3() : r(0.0f), g(0.0f), b(0.0f) {}
    vec3(float r, float g, float b) : r(r), g(g), b(b) {}
};

const int GRID_WIDTH = 20;
const int GRID_HIGHT = 20;
const float UPDATE_INTERVAL = 0.15f;
const float CELL_WIDTH = 2.0f / GRID_WIDTH;
const float CELL_HEIGHT = 2.0f / GRID_HIGHT;

enum class Direction
{
    Up,
    Down,
    Left,
    Right,
    None
};

Direction snakeDirection = Direction::None;
std::vector<vec2i> snake = {vec2i(5, 10), vec2i(4, 10), vec2i(3, 10)};
vec2i fruit;
int score = 0;
bool gameOver = false;
bool gameStarted = false;
float timeSinceLastUpdate = 0.0f;
float snakeSpeed = UPDATE_INTERVAL;
float gameOverTime = 0.0f;

const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;

uniform vec2 uOffSet;
uniform vec2 uScale;

void main()
{
    vec2 position = (aPos * uScale) + uOffSet;
    gl_Position = vec4(position, 0.0, 1.0);
}

)";

const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;

void main()
{
    FragColor = vec4(uColor, 1.0);
}

)";
GLuint shaderProgram;
GLuint VAO, VBO;
GLint uOffSetLocation, uScaleLocation, uColorLocation;

const int FONT_WITH = 5;
const int FONT_HEIGHT = 5;
const int FONT_SPACING = 1;

std::map<char, std::vector<int>> fontMap = {
    {' ', {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0}},
    {'A', {0,1,1,0,0, 1,0,0,1,0, 1,1,1,1,0, 1,0,0,1,0, 1,0,0,1,0}},
    {'B', {1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0}},
    {'C', {0,1,1,1,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 0,1,1,1,0}},
    {'D', {1,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 1,1,1,0,0}},
    {'E', {1,1,1,1,0, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,0,0, 1,1,1,1,0}},
    {'F', {1,1,1,1,0, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,0,0, 1,0,0,0,0}},
    {'G', {0,1,1,1,0, 1,0,0,0,0, 1,0,1,1,0, 1,0,0,1,0, 0,1,1,1,0}},
    {'H', {1,0,0,1,0, 1,0,0,1,0, 1,1,1,1,0, 1,0,0,1,0, 1,0,0,1,0}},
    {'I', {1,1,1,0,0, 0,1,0,0,0, 0,1,0,0,0, 0,1,0,0,0, 1,1,1,0,0}},
    {'J', {0,0,1,1,0, 0,0,0,1,0, 0,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'K', {1,0,0,1,0, 1,0,1,0,0, 1,1,0,0,0, 1,0,1,0,0, 1,0,0,1,0}},
    {'L', {1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,1,1,1,0}},
    {'M', {1,0,0,0,1, 1,1,0,1,1, 1,0,1,0,1, 1,0,0,0,1, 1,0,0,0,1}},
    {'N', {1,0,0,0,1, 1,1,0,0,1, 1,0,1,0,1, 1,0,0,1,1, 1,0,0,0,1}},
    {'O', {0,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'P', {1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,0,0,0, 1,0,0,0,0}},
    {'Q', {0,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,1,0,0, 0,1,0,1,0}},
    {'R', {1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,1,0,0, 1,0,0,1,0}},
    {'S', {0,1,1,1,0, 1,0,0,0,0, 0,1,1,0,0, 0,0,0,1,0, 1,1,1,0,0}},
    {'T', {1,1,1,1,1, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0}},
    {'U', {1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'V', {1,0,0,0,1, 1,0,0,0,1, 0,1,0,1,0, 0,1,0,1,0, 0,0,1,0,0}},
    {'W', {1,0,0,0,1, 1,0,0,0,1, 1,0,1,0,1, 1,0,1,0,1, 0,1,0,1,0}},
    {'X', {1,0,0,0,1, 0,1,0,1,0, 0,0,1,0,0, 0,1,0,1,0, 1,0,0,0,1}},
    {'Y', {1,0,0,0,1, 0,1,0,1,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0}},
    {'Z', {1,1,1,1,1, 0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,1,1,1,1}},
    {'0', {0,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'1', {0,0,1,0,0, 0,1,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,1,1,1,0}},
    {'2', {0,1,1,0,0, 1,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,1,1,1,0}},
    {'3', {1,1,1,0,0, 0,0,0,1,0, 0,1,1,0,0, 0,0,0,1,0, 1,1,1,0,0}},
    {'4', {0,0,1,1,0, 0,1,0,1,0, 1,0,0,1,0, 1,1,1,1,1, 0,0,0,1,0}},
    {'5', {1,1,1,1,0, 1,0,0,0,0, 1,1,1,0,0, 0,0,0,1,0, 1,1,1,0,0}},
    {'6', {0,1,1,0,0, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'7', {1,1,1,1,0, 0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,0,0,0,0}},
    {'8', {0,1,1,0,0, 1,0,0,1,0, 0,1,1,0,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'9', {0,1,1,0,0, 1,0,0,1,0, 0,1,1,1,0, 0,0,0,1,0, 0,1,1,0,0}},
    {':', {0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0}},
    {'-', {0,0,0,0,0, 0,0,0,0,0, 1,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0}},
    {'.', {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,1,0,0}}
};
void SpawnFruit();
void InitGame();
void ResetGame();
void KeyCallBackfun(GLFWwindow *window, int key, int scanCode, int action, int mods);
void DrawCell(const vec2i &position, const vec3 &color);
void DrawChar(char c, float x, float y, float scale, const vec3 &color);
void DrawText(const std::string &text, float x, float y, float scale, const vec3 &color);
void RenderGame(GLFWwindow *window);
void UpdateGame(float deltaTime);
void DrawBorder();
void DrawSnake();
void DrawScore();
void DrawGameOver();
void DrawStartScreen();
void DrawAnimatedGameOverBorder();

int main()
{
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    if (!glfwInit())
    {
        std::cerr << "GLFW init failed\n";
        return -1;
    }

    // macOS core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window =
        glfwCreateWindow(800, 600, "SnakeGame", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    glfwSetKeyCallback(window, KeyCallBackfun);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader error:\n"<< infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader error:\n" << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader link error:\n"<< infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    uOffSetLocation = glGetUniformLocation(shaderProgram, "uOffSet");
    uScaleLocation = glGetUniformLocation(shaderProgram, "uScale");
    uColorLocation = glGetUniformLocation(shaderProgram, "uColor");

    float vertices[] = {
        -0.5f, -0.5f,
        0.5f,  -0.5f,
        -0.5f,  0.5f,
        0.5f,   0.5f,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    InitGame();
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        glfwPollEvents();
        UpdateGame(deltaTime);
        RenderGame(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void UpdateGame(float deltaTime)
{
    if (gameOver)
    {
        gameOverTime += deltaTime;
        return;
    }
    if (gameStarted && !gameOver)
    {
        timeSinceLastUpdate += deltaTime;
        if (timeSinceLastUpdate >= snakeSpeed)
        {
            timeSinceLastUpdate = 0.0f;
            vec2i newHead = snake[0];

            switch (snakeDirection)
            {
            case Direction::Up:
                newHead.y++;
                break;
            case Direction::Down:
                newHead.y--;
                break;
            case Direction::Left:
                newHead.x--;
                break;
            case Direction::Right:
                newHead.x++;
                break;

            case Direction::None:
                return;
            }
            if (newHead.x < 0 || newHead.x >= GRID_WIDTH ||
                newHead.y < 0 || newHead.y >= GRID_HIGHT)
            {

                gameOver = true;
                return;
            }
            for (const auto &segment : snake)
            {
                if (newHead == segment)
                {
                    gameOver = true;
                    return;
                }
            }
            snake.insert(snake.begin(), newHead);
            if (newHead == fruit)
            {
                score += 10;
                SpawnFruit();

                if (score % 50 == 0 && snakeSpeed > 0.05f)
                {
                    snakeSpeed -= 0.01f;
                }
            }
            else
            {
                snake.pop_back();
            }
        }
    }
}

void RenderGame(GLFWwindow *window)
{
    glClearColor(0.08f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    DrawBorder();

    if (!gameStarted)
    {
        DrawStartScreen();
    }
    else if (gameOver)
    {

        DrawGameOver();
    }
    else
    {
        DrawSnake();
        DrawScore();
    }

    glBindVertexArray(0);
    glfwSwapBuffers(window);
}

void DrawCell(const vec2i &position, const vec3 &color)
{
    vec2 offsSet(-1.0f + position.x * CELL_WIDTH + CELL_WIDTH * 0.5f,
                 -1.0f + position.y * CELL_HEIGHT + CELL_HEIGHT * 0.5f);
    vec2 scale(CELL_WIDTH * 0.9f, CELL_HEIGHT * 0.9f);

    glUniform3f(uColorLocation, color.r, color.g, color.b);
    glUniform2f(uOffSetLocation, offsSet.x, offsSet.y);
    glUniform2f(uScaleLocation, scale.x, scale.y);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void DrawChar(char c, float x, float y, float scale, const vec3 &color)
{
    c = std::toupper((c));
    auto it = fontMap.find(c);
    if (it == fontMap.end())
    {
        it = fontMap.find(' ');
    }

    const std::vector<int> &bitmap = it->second;
    float charWidth = FONT_WITH * scale;
    float charHeight = FONT_HEIGHT * scale;

    for (int i = 0; i < FONT_HEIGHT; i++)
    {
        for (int j = 0; j < FONT_WITH; j++)
        {
            if (bitmap[i * FONT_WITH + j])
            {
                vec2 offset(x + j * scale - charWidth / 2.0f,
                            y - i * scale + charHeight / 2.0f);

                glUniform3f(uColorLocation, color.r, color.g, color.b);
                glUniform2f(uOffSetLocation, offset.x, offset.y);
                glUniform2f(uScaleLocation, scale, scale);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
        }
    }
}

void DrawText(const std::string &text, float x, float y, float scale, const vec3 &color)
{
    float charWith = FONT_WITH * scale;
    float spacing = FONT_SPACING * scale;
    float tolalWidth = text.size() * (charWith + spacing) - spacing;

    float startX = x - tolalWidth / 2.0f;
    for (size_t i = 0; i < text.size(); i++)
    {
        DrawChar(text[i], startX + i * (charWith + spacing), y, scale, color);
    }
}

void DrawBorder()
{
    vec3 borderColor(0.3f, 0.3f, 0.5f);

    for (int x = -1; x <= GRID_WIDTH; x++) // top
    {
        DrawCell(vec2i(x, GRID_HIGHT), borderColor);
    }

    for (int x = -1; x <= GRID_WIDTH; x++) // bottom
    {
        DrawCell(vec2i(x, -1), borderColor);
    }

    for (int y = -1; y <= GRID_HIGHT; y++) // Left
    {
        DrawCell(vec2i(-1, y), borderColor);
    }

    for (int y = -1; y <= GRID_HIGHT; y++) // Right
    {
        DrawCell(vec2i(GRID_WIDTH, y), borderColor);
    }

    vec3 gridColor(0.082f, 0.106f, 0.329f);

    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HIGHT; y++)
        {
            if ((x + y) % 2 == 0)
            {
                DrawCell(vec2i(x, y), gridColor);
            }
        }
    }
}

void DrawSnake()
{
    vec3 headColor(0.0f, 0.95f, 0.3f); // snake head color
    vec3 bodyColor(0.0f, 0.7f, 0.1f);  // snake body color

    for (size_t i = 1; i < snake.size(); i++)
    {
        float factor = static_cast<float>(i) / snake.size();

        vec3 segmentcolor(
            bodyColor.r * (1.0f - factor) + 0.1f * factor,
            bodyColor.g * (1.0f - factor) + 0.1f * factor,
            bodyColor.b * (1.0f - factor));
        DrawCell(snake[i], segmentcolor);
    }
    DrawCell(snake[0], headColor);           // draw snake
    DrawCell(fruit, vec3(1.0f, 0.3f, 0.3f)); // fruit color
}

void DrawScore()
{
    std::string scoreText = "SCORE: " + std::to_string(score);
    DrawText(scoreText, 0.0f, 0.9f, 0.012f, vec3(0.9f, 0.9f, 0.9f));
}

void DrawGameOver()
{
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HIGHT; y++)
        {
            DrawCell(vec2i(x, y), vec3(0.2f, 0.1f, 0.1f));
        }
    }
    DrawAnimatedGameOverBorder();
    DrawText("GAME OVER", 0.0f, 0.2f, 0.025f, vec3(0.9f, 0.2f, 0.2f));

    std::string scoreText = "SCORE : " + std::to_string(score);
    DrawText(scoreText, 0.0f, 0.0f, 0.018f, vec3(0.9f, 0.9f, 0.9f));

    DrawText("PRESS R TO RESTART", 0.0f, -0.2f, 0.015f, vec3(0.8f, 0.8f, 0.8f));
}

void DrawStartScreen()
{
    DrawText("SNAKE GAME", 0.0f, 0.3f, 0.025f, vec3(0.2f, 0.8f, 0.3f)); // title

    DrawText("USE ARROW KEY TO MOVE", 0.0f, 0.0f, 0.012f, vec3(0.9f, 0.9f, 0.9f));
    DrawText("EAT THE RED FRUIT TO GROW", 0.0f, -0.1f, 0.012f, vec3(0.9f, 0.9f, 0.9f));
    DrawText("AVOID WALLS AND YOURSELF", 0.0f, -0.2f, 0.012f, vec3(0.9f, 0.9f, 0.9f));
    DrawText("PRESS ANY KEY TO START", 0.0f, -0.4f, 0.012f, vec3(0.8f, 0.8f, 0.2f));
}

void SpawnFruit()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> distX(0, GRID_WIDTH - 1);
    std::uniform_int_distribution<> distY(0, GRID_HIGHT - 1);

    while (true)
    {
        vec2i newFruit(distX(gen), distY(gen));
        bool validPosition = true;

        for (const auto &segment : snake)
        {
            if (segment == newFruit)
            {
                validPosition = false;
                break;
            }
        }
        if (validPosition)
        {
            fruit = newFruit;
            break;
        }
    }
}

void InitGame()
{
    ResetGame();
    SpawnFruit();
}

void ResetGame()
{
    snake = {vec2i(5, 10), vec2i(4, 10), vec2i(3, 10)};
    snakeDirection = Direction::None;
    gameOver = false;
    gameStarted = false;
    score = 0;
    timeSinceLastUpdate = 0.0f;
    gameOverTime = 0.0f;
    snakeSpeed = UPDATE_INTERVAL;
}

void KeyCallBackfun(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (!gameStarted && key != GLFW_KEY_R)
        {
            gameStarted = true;
            snakeDirection = Direction::Right;
            return;
        }
    }
    if (gameOver && key == GLFW_KEY_R)
    {
        ResetGame();
        SpawnFruit();
        return;
    }
    if (!gameOver && gameStarted)
    {
        switch (key)
        {
        case GLFW_KEY_UP:
        {
            if (snakeDirection != Direction::Down)
                snakeDirection = Direction::Up;
        }
        break;
        case GLFW_KEY_DOWN:
        {
            if (snakeDirection != Direction::Up)
                snakeDirection = Direction::Down;
        }
        break;
        case GLFW_KEY_LEFT:
        {
            if (snakeDirection != Direction::Right)
                snakeDirection = Direction::Left;
        }
        break;
        case GLFW_KEY_RIGHT:
        {
            if (snakeDirection != Direction::Left)
                snakeDirection = Direction::Right;
        }
        break;
        default:
            break;
        }
    }
}

void DrawAnimatedGameOverBorder()
{
    float pulse = 0.5f + 0.5f * sin(gameOverTime * 6.0f);

    vec3 borderColor(
        0.6f + 0.4f * pulse,
        0.1f,
        0.1f);

    for (int x = 0; x < GRID_WIDTH; x++)
    {
        DrawCell(vec2i(x, GRID_HIGHT - 1), borderColor);
        DrawCell(vec2i(x, 0), borderColor);
    }

    for (int y = 0; y < GRID_HIGHT; y++)
    {
        DrawCell(vec2i(0, y), borderColor);
        DrawCell(vec2i(GRID_WIDTH - 1, y), borderColor);
    }
}
