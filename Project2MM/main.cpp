#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <algorithm>
#include <sstream>
#include <ctime>

using namespace std;
using namespace sf;

//window size init
const int Window_Width = 800;
const int Window_Height = 400;
const float Gravity = 0.4f; // garvity
const float Jump_Force = -12.0f;
const float Enemy_Speed = 15.0f;
const float Ground_Y = 300;
const float Score_init = 0.5f;


enum GameState
{
	Menu, Playing, Game_over
};

class Player
{
	public:
		RectangleShape HideBlock;
		float Velocity_Y = 0;
		bool isJumping = false;

		Player()
		{
			HideBlock.setSize(Vector2f(50, 50));
			HideBlock.setFillColor(Color::Green);
			HideBlock.setPosition(100, Ground_Y);
		}

		// jump function
		void jump()
		{
			if (!isJumping)
			{
				Velocity_Y = Jump_Force;
				isJumping = true;
			}
		}

		// base function
		void update(float deltatime)
		{
			Velocity_Y += Gravity * deltatime * 60;
			HideBlock.move(0, Velocity_Y * deltatime * 60);

			if (HideBlock.getPosition().y >= Ground_Y)
			{
				HideBlock.setPosition(100, Ground_Y);
				isJumping = false;
			}
		}
};

class Enemy
{
	public:
		RectangleShape Enemies;

		Enemy(float startX)
		{
			Enemies.setSize(Vector2f(40, 50));
			Enemies.setFillColor(Color::Red);
			Enemies.setPosition(startX, Ground_Y);
		}

		void update(float deltatime)
		{
			Enemies.move(-Enemy_Speed * deltatime * 60, 0);
		}

		bool isoffSreen() const
		{
			return Enemies.getPosition().x + Enemies.getPosition().x < 0;
		}
};

class Ground
{
public:
	RectangleShape shape;

	Ground(float width, float height, Color color, float y)
	{
		shape.setSize(Vector2f(width, height));
		shape.setFillColor(color);
		shape.setPosition(0, y);
	}

	void draw(RenderWindow& window)
	{
		window.draw(shape);
	}
};

static bool checkCollision(const Player& player, const Enemy& enemy)
{
	return player.HideBlock.getGlobalBounds().intersects(enemy.Enemies.getGlobalBounds());
}

int main() {
	RenderWindow window(VideoMode(Window_Width, Window_Height), "Project 2MM");
	window.setFramerateLimit(90);

	//init time 
	srand(time(NULL));

	Event ev;
	Player player;
	Ground ground(Window_Width, 20, Color(139, 69, 19), Ground_Y + 50);
	GameState gamestate = Menu;
	vector<Enemy> enemys;
	Clock spawn_c, delta_c, score_c;
	int score = 0;
	

	// Load Font 
	Font font;
	if (!font.loadFromFile("Font/MinecraftItalic-R8Mo.otf"))
	{
		cout << "Failled to load font!" << endl;
		return -1;
	}

	//Load Mous 
	Vector2i  mousePos;

	//set update score (Ui text)
	Text scoreText("", font, 25);
	scoreText.setFillColor(Color::Black);
	scoreText.setPosition(Vector2f(10, 10));

	//Load BGM1;
	Music BGM1;
	if (!BGM1.openFromFile("BGM/C418 - Sweden.ogg"))
	{
		cout << "Error loading music file!" << endl;
		return -1;
	}
	BGM1.setLoop(true);
	BGM1.setVolume(2);

	//Load BGM2;
	Music BGM2;
	if (!BGM2.openFromFile("BGM/Rainy_day.ogg"))
	{
		cout << "Error loading music file!" << endl;
		return -1;
	}
	BGM2.setLoop(true);
	BGM2.setVolume(2);

	//Load Sound_click_buttom
	SoundBuffer buffer1;
	if (!buffer1.loadFromFile("Sound/ButtonPlate Click.wav"))
	{
		cout << "Error loading sound file!" << std::endl;
		return -1;
	}
	Sound sound1;
	sound1.setBuffer(buffer1);
	sound1.setVolume(3);

	//Load Sound_jump 
	SoundBuffer buffer2;
	if (!buffer2.loadFromFile("Sound/Mario Jump.wav"))
	{
		cout << "Error loading sound file!" << std::endl;
		return -1;
	}
	Sound sound2;
	sound2.setBuffer(buffer2);
	sound2.setVolume(5);

	//starts button set
	Text BstatsText("", font, 30);//txet
	BstatsText.setFillColor(Color::Black);
	BstatsText.setPosition(300,250);
	RectangleShape starts_B(Vector2f(200, 100));//buttim
	starts_B.setFillColor(Color::Cyan);
	starts_B.setPosition(300, 250);

	//Text Manu
	Text S_Text("Welcome", font,40);
	S_Text.setFillColor(Color::Black);
	S_Text.setPosition(320, 100);

	//game over button
	Text BoverText("Return to menu", font, 30);//txet
	BoverText.setFillColor(Color::Black);
	BoverText.setPosition(300, 150);
	RectangleShape over_B(Vector2f(200, 100));//buttim
	over_B.setFillColor(Color::Blue);
	over_B.setPosition(300, 250);
	
	//Text game over
	Text O_Text("Gave Over", font, 40);
	O_Text.setFillColor(Color::Black);
	O_Text.setPosition(150, 60);

	// mous 
	Mouse mo;

	while (window.isOpen())
	{
		float delta_time = delta_c.restart().asSeconds();

		while (window.pollEvent(ev))
		{

			//close window
			if (ev.type == Event::Closed || (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Escape))
			{
				window.close();
			}

			//game stast
			if (gamestate == Menu && ev.type == Event::MouseButtonPressed)
			{
				//BGM1.play();
				if (ev.mouseButton.button == Mouse::Left)
				{
					mousePos = Mouse::getPosition(window);
				
					if (starts_B.getGlobalBounds().contains(mousePos.x, mousePos.y))
					{
						starts_B.setFillColor(Color::Green);
						sound1.play();
						gamestate = Playing;
						enemys.clear();
						spawn_c.restart();
						score_c.restart();
						score = 0;
					}
				}
			}

			if (gamestate == Playing)
			{
				//BGM2.play();
				if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Space)
				{
					player.jump();
					sound2.play();
				}
			}

			if (gamestate == Game_over && ev.type == Event::MouseButtonPressed)
			{
				if (ev.mouseButton.button == Mouse::Left)
				{
					mousePos = Mouse::getPosition(window);
					if (over_B.getGlobalBounds().contains(mousePos.x, mousePos.y))
					{
						sound1.play();
						gamestate = Menu;
					}
					
				}
			}
		}

			if (gamestate == Playing)
			{
				player.update(delta_time);

				//craeta enemy 
				if (spawn_c.getElapsedTime().asSeconds() > rand()%10+0.9/*set spawn enemy*/)
				{
					enemys.push_back(Enemy(Window_Width));
					spawn_c.restart();
				}

				for (auto& enemy :enemys)
				{
					enemy.update(delta_time);
					enemys.erase(remove_if(enemys.begin(), enemys.end(), [](const Enemy& e)
					{return e.isoffSreen(); }), enemys.end());
				}

				//brock 
				for (const auto& enemy : enemys)
				{
					if (checkCollision(player,enemy))
					{
						gamestate = Game_over;
						break;
					}
				}

				//addscora everytime 2 seconed
				if (score_c.getElapsedTime().asSeconds() > Score_init)
				{
					score++;
					score_c.restart();

					stringstream score_s;
					score_s << "Score : " << score;
					scoreText.setString(score_s.str());
				}
			}

			window.clear(Color::White);
			if (gamestate == Menu)
			{
				window.draw(BstatsText);
				window.draw(starts_B);
				window.draw(S_Text);
			}

			else if (gamestate == Playing)
			{
				window.draw(player.HideBlock);
				for (const auto& enemy : enemys)
				{
					window.draw(enemy.Enemies);
				}
				window.draw(scoreText);
				ground.draw(window);
			}

			else if (gamestate == Game_over)
			{
				window.draw(O_Text);
				window.draw(BoverText);
				window.draw(over_B);
			}
		window.display();
	}
	return 0;
}
