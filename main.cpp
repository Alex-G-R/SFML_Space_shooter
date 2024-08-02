#include <iostream>
#include <vector>
#include <cstdlib>

#include <Graphics.hpp>
#include <System.hpp>
#include <Window.hpp>
#include <Audio.hpp>
#include <Network.hpp>


// AMMUNITION
class Ammo
{
public:
    sf::Sprite shape;
    float speed;
    float positionY;

    Ammo(float positionY_p, sf::Texture& texture)
        : speed(10.f)
    {
        this->shape.setTexture(texture);
        this->positionY = positionY_p - this->shape.getGlobalBounds().height/2;
        this->shape.setPosition(20.f, positionY);
    }

    void move()
    {
        this->shape.move(speed, 0);
    }

    bool outOfScreen()
    {
        if(this->shape.getPosition().x + this->shape.getGlobalBounds().width > 1920)
            return true;
        return false;
    }
};

// PLAYER
class Player
{
public:
    sf::Sprite shape;
    sf::Texture playerTex;
    sf::Texture ammoTex;
    std::vector<Ammo> bullets;

    float speed;
    int hp;
    int shooting_timer = 0;

    Player()
        : speed(10.f), hp(100)
    {
        if (!playerTex.loadFromFile("Textures/ship.png"))
        {
            std::cerr << "Error: Could not load playerTex" << std::endl;
            throw("Could not load playerTex");
        }
        if (!ammoTex.loadFromFile("Textures/ammo.png"))
        {
            std::cerr << "Error: Could not load ammoTex" << std::endl;
            throw("Could not load ammoTex");
        }
        this->shape.setTexture(playerTex);
        this->shape.setPosition(20.f, 0.f);
    }

    void shoot()
    {
        Ammo b1(this->shape.getPosition().y + this->shape.getGlobalBounds().height/2, ammoTex);
        bullets.push_back(b1);
    }
};;

int main()
{
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML Works!", sf::Style::Default);
    window.setFramerateLimit(120);


    Player player;

    // Health bar
    sf::RectangleShape hp_bar;
    hp_bar.setFillColor(sf::Color::Red);
    hp_bar.setSize(sf::Vector2f(500.f, 50.f));
    hp_bar.setPosition(window.getSize().x/2 - hp_bar.getSize().x/2, 3.f);
    // Hp bar empty
    sf::RectangleShape hp_bar_empty;
    hp_bar_empty.setFillColor(sf::Color::Black);
    hp_bar_empty.setSize(sf::Vector2f(500.f, 50.f));
    hp_bar_empty.setPosition(window.getSize().x/2 - hp_bar.getSize().x/2, 3.f);
    // Hp bar outline
    sf::RectangleShape hp_bar_outline;
    hp_bar_outline.setFillColor(sf::Color(190, 190, 190, 255));
    hp_bar_outline.setSize(sf::Vector2f(506.f, 56.f));
    hp_bar_outline.setPosition(window.getSize().x/2 - hp_bar.getSize().x/2 - 3.f, 0.f);

    // Enemy
    sf::Sprite enemy;
    sf::Texture enemyTex;
    if (!enemyTex.loadFromFile("Textures/enemy.png"))
        throw("Could not lad enemyTex");
    enemy.setTexture(enemyTex);
    int enemy_spawn_timer = 0;
    std::vector<sf::Sprite> enemies;

    int score = 0;

    // Font
    sf::Font font;
    if(!font.loadFromFile("Fonts/Chopsic.otf"))
        throw("Could not load font");

    sf::Text scoreUI;
    scoreUI.setFont(font);
    scoreUI.setCharacterSize(24);
    scoreUI.setColor(sf::Color::White);
    scoreUI.setStyle(sf::Text::Bold);
    scoreUI.setPosition(10.f, 10.f);
    scoreUI.setString("Score: " + std::to_string(score));

    while(window.isOpen() && player.hp > 0)
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }

        // Update

        // update score
        scoreUI.setString("Score: " + std::to_string(score));

        // move player with the mouse
        player.shape.setPosition(player.shape.getPosition().x,sf::Mouse::getPosition(window).y);

        // shooting
        if(player.shooting_timer < 10)
            player.shooting_timer++;

        if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && player.shooting_timer >= 10)
        {
            player.shooting_timer = 0;
            player.shoot();
        }

        // Move bullets and delete out of bound
        for(size_t i = 0; i < player.bullets.size(); i++)
        {
            player.bullets[i].move();
            if(player.bullets[i].outOfScreen())
                player.bullets.erase(player.bullets.begin() + i);
        }

        // Enemies
        if(enemy_spawn_timer < 30)
            enemy_spawn_timer++;

        if(enemy_spawn_timer >= 30)
        {
            enemy.setPosition(window.getSize().x, rand() % int(window.getSize().y - enemy.getGlobalBounds().height));
            enemies.push_back(sf::Sprite(enemy));
            enemy_spawn_timer = 0;
        }

        for(size_t i = 0; i < enemies.size(); i++)
        {
            enemies[i].move(-4.f, 0.f);

            if(enemies[i].getPosition().x + enemy.getGlobalBounds().width < 100)
            {
                enemies.erase(enemies.begin() + i);
                player.hp = player.hp - 10;
            }
        }

        // Colision
        for(size_t i = 0; i < enemies.size(); i++)
        {
            if(player.shape.getGlobalBounds().intersects(enemies[i].getGlobalBounds()))
            {
                enemies.erase(enemies.begin() + i);
                player.hp = player.hp - 20;
            }
        }

        for(size_t i = 0; i < enemies.size(); i++)
        {
            for(size_t k = 0; k < player.bullets.size(); k++)
            {
                if(enemies[i].getGlobalBounds().intersects(player.bullets[k].shape.getGlobalBounds()))
                {
                    enemies.erase(enemies.begin() + i);
                    player.bullets.erase(player.bullets.begin() + k);
                    score++;
                    break;
                }
            }
        }

        // Hp bar
        hp_bar.setSize(sf::Vector2f(static_cast<float>(player.hp*5), hp_bar.getSize().y));
        if(hp_bar.getSize().x < 0)
            hp_bar.setSize(sf::Vector2f(0.f, hp_bar.getSize().y));


        // Draw
        window.clear();

        // Draw stuff

        for(size_t i = 0; i < player.bullets.size(); i++)
        {
            window.draw(player.bullets[i].shape);
        }
        window.draw(player.shape);

        for(size_t i = 0; i < enemies.size(); i++)
        {
            window.draw(enemies[i]);
        }

        // UI
        window.draw(scoreUI);
        // hp bar
        window.draw(hp_bar_outline);
        window.draw(hp_bar_empty);
        window.draw(hp_bar);

        window.display();
    }

    return 0;
}

