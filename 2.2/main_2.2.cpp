#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <map>

using namespace std;
using namespace sf;

constexpr int WINDOW_WIDTH{800}, WINDOW_HEIGHT{600}, NUMBER_OF_BALLS{5}, COLORS{3};
constexpr float BALL_RADIUS{40.f}, BALL_INDENT{200.f}, VELOCITY_X{200.f}, VELOCITY_Y{80.f};

int colors[NUMBER_OF_BALLS][COLORS] = {
    {0, 141, 15},
    {242, 0, 0},
    {237, 255, 0},
    {2, 78, 128},
    {120, 141, 0}};

struct Ball
{
    CircleShape shape;
    Vector2f velocity{-VELOCITY_X, -VELOCITY_Y};

    Ball(int color1, int color2, int color3)
    {
        shape.setPosition(1 + rand() % (800 - 40), 1 + rand() % (600 - 40));
        shape.setRadius(BALL_RADIUS);
        shape.setFillColor(Color(color1, color2, color3));
        shape.setOrigin(BALL_RADIUS, BALL_RADIUS);
    }

    void updateBall(const float &dT)
    {
        Vector2f position = shape.getPosition();
        position += velocity * dT;

        if ((position.x + BALL_RADIUS >= WINDOW_WIDTH) && (velocity.x > 0))
            velocity.x = -velocity.x;

        if ((position.x - BALL_RADIUS < 0) && (velocity.x < 0))
            velocity.x = -velocity.x;

        if ((position.y + BALL_RADIUS >= WINDOW_HEIGHT) && (velocity.y > 0))
            velocity.y = -velocity.y;

        if ((position.y - BALL_RADIUS < 0) && (velocity.y < 0))
            velocity.y = -velocity.y;

        shape.setPosition(position);
    }
};

void initBricks(vector<Ball> &myBall)
{
    for (int element = 0; element < NUMBER_OF_BALLS; ++element)
        myBall.emplace_back(colors[element][0], colors[element][1], colors[element][2]);
}

float getModuleVector(Vector2f arg)
{
    return sqrt(arg.x * arg.x + arg.y * arg.y);
}

void updateDate(vector<Ball> &myBall, Clock &clock)
{
    const float deltaTime = clock.restart().asSeconds();

    for (auto &ball : myBall)
        ball.updateBall(deltaTime);

    for (int i = 0; i < NUMBER_OF_BALLS; i++)
    {
        Vector2f positionBall = myBall.at(i).shape.getPosition();
        positionBall += myBall.at(i).velocity * deltaTime;
        for (int j = i + 1; j < NUMBER_OF_BALLS; j++)
        {
            Vector2f positionBall2 = myBall.at(j).shape.getPosition();
            Vector2f positionDelta = positionBall - positionBall2;
            float vectorLength = sqrt(positionDelta.x * positionDelta.x + positionDelta.y * positionDelta.y);
            if (vectorLength < BALL_RADIUS * 2) // произошло столкновение
            {
                Vector2f deltaSpeed = myBall.at(i).velocity - myBall.at(j).velocity;
                Vector2f deltaSpeed1 = myBall.at(j).velocity - myBall.at(i).velocity;
                Vector2f deltaPosition = positionBall - positionBall2;
                Vector2f deltaPosition1 = positionBall2 - positionBall;
                float module = getModuleVector(deltaPosition) * getModuleVector(deltaPosition);
                float module1 = getModuleVector(deltaPosition1) * getModuleVector(deltaPosition1);
                float dot = deltaSpeed.x * deltaPosition.x + deltaSpeed.y * deltaPosition.y;
                float dot1 = deltaSpeed1.x * deltaPosition1.x + deltaSpeed1.y * deltaPosition1.y;
                float drob = dot / module;
                float drob1 = dot1 / module1;
                Vector2f direction = deltaSpeed - (drob * deltaPosition);
                Vector2f direction1 = deltaSpeed1 - (drob1 * deltaPosition1);
                float speedLength1 = getModuleVector(myBall.at(i).velocity);
                float speedLength2 = getModuleVector(myBall.at(j).velocity);
                float direction1Length = getModuleVector(direction);
                float direction2Length = getModuleVector(direction1);
                Vector2f normalizeDir1 = {direction.x / direction1Length, direction.y / direction1Length};
                Vector2f normalizeDir2 = {direction1.x / direction2Length, direction1.y / direction2Length};
                myBall.at(i).velocity = speedLength2 * normalizeDir1;
                myBall.at(j).velocity = speedLength1 * normalizeDir2;
            }
        }
    }
}

void pollEvents(RenderWindow &window)
{
    Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
        case Event::Closed:
            window.close();
            break;
        default:
            break;
        }
    }
}

void redrawWorld(vector<Ball> &myBall, RenderWindow &window)
{
    window.setFramerateLimit(120);
    window.clear(Color::Black);
    for (auto &ball : myBall)
        window.draw(ball.shape);
    window.display();
}

int main()
{
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Mass clashes");
    Clock clock;

    vector<Ball> balls;
    initBricks(balls);

    while (window.isOpen())
    {
        pollEvents(window);
        updateDate(balls, clock);
        redrawWorld(balls, window);
    }
}