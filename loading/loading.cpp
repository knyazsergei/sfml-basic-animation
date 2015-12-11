#include <SFML/Graphics.hpp>
#include <iterator>
#include <vector>
#include <list>
#include <string>
#include <iostream>
#include "str_switch.h"

using namespace sf;
using namespace std;


namespace Ani {
	class Animation {
	private:
		Text * m_obj;
		float speed = 0;

		string m_numOfAnimation;
		float m_param;
		float m_duration;
		float m_delta;
		float m_delay = 0;
		float m_mathing;
		bool wait = false;
	public:
		bool active = true;

		Animation(Text * object, string numOfAnimation, float param, float duration, float delay) {
			m_obj = object;
			m_numOfAnimation = numOfAnimation;
			m_duration = duration;
			m_delay = delay;
			m_param = param;
			SWITCH (numOfAnimation) {
			CASE("rotate"):
				if (delay > 0) {
					wait = true;
				}
				else {
					speed = param / duration;
					m_mathing = object->getRotation() + m_param;
				}				
				break;
			CASE("opacity"): 
				if (delay > 0) {
					wait = true;
				}
				else {
					speed = (param - int(object->getColor().a)) / duration;
					m_mathing = param;
				}
				break;
			CASE("left"):
			CASE("top"):
			CASE("right"):
			CASE("bottom"):
				speed = param / duration;
				break;
			CASE("zoom") :
				if (delay > 0) {
					wait = true;
				}
				else {
					speed = (param - object->getScale().x) / duration;
					m_mathing = param;
					break;
				}
			}
		}


		void play(float delta) {
			m_delta = delta;

			if (wait && m_delay == 0) {
				wait = false;
				speed = Animation(m_obj, m_numOfAnimation, m_param, m_duration, 0).speed;
				m_mathing = Animation(m_obj, m_numOfAnimation, m_param, m_duration, 0).m_mathing;
			}

			if (m_delay == 0) {
				update();
			}
			else {
				if (delta > m_delay) {
					delta -= m_delay;
					m_delay = 0;
					if (wait && m_delay == 0) {
						wait = false;
						speed = Animation(m_obj, m_numOfAnimation, m_param, m_duration, 0).speed;
						m_mathing = Animation(m_obj, m_numOfAnimation, m_param, m_duration, 0).m_mathing;
					}
					update();
				}
				else {
					m_delay -= delta;
				}
			}
		}

		void update() {
			bool end = false;
			if (m_duration < m_delta) {
				m_duration = 0;
				m_delta -= m_duration;
				end = true;
			}
			else {
				m_duration -= m_delta;
			}
			if (m_duration == 0) {
				active = false;
				end = true;
			}
			SWITCH(m_numOfAnimation) {
				CASE("rotate"):
					rotate(end);
					break;
				CASE("opacity"):
					opacity(end);
					break;
				CASE("left"):
					left(end);
					break;
				CASE("top") :
					top(end);
					break;
				CASE("right") :
					right(end);
					break;
				CASE("bottom") :
					bottom(end);
					break;
				CASE("zoom") :
					zoom(end);
					break;
			}
		}

		void rotate(bool end) {
			if (end) {
				m_obj->setRotation(m_mathing);
			}
			else {
				m_obj->rotate(speed * m_delta);
			}
		}
		void left(bool end) {
			m_obj->move(speed * m_delta * Vector2f(1, 0));
		}
		void right(bool end) {
			m_obj->move(speed * m_delta * Vector2f(-1, 0));
		}
		void top(bool end) {
			m_obj->move(speed * m_delta * Vector2f(0, 1));
		}
		void bottom(bool end) {
			m_obj->move(speed * m_delta * Vector2f(0, -1));
		}
		void opacity(bool end) {
			float newAlpha = int(m_obj->getColor().a) + speed * m_delta;
			if (newAlpha < 0) {
				newAlpha = 0;
			}
			else if (newAlpha > 255) {
				newAlpha = 255;
			}
			if (end) {
				m_obj->setColor(Color(m_obj->getColor().r, m_obj->getColor().g, m_obj->getColor().b, sf::Uint8(m_mathing)));
			}
			else {
				m_obj->setColor(Color(m_obj->getColor().r, m_obj->getColor().g, m_obj->getColor().b, sf::Uint8(newAlpha)));
			}
		}
		void zoom(bool end) {
			if (end) {
				m_obj->setScale(Vector2f(m_mathing, m_mathing));
			}
			else {
				Vector2f newZoom = m_obj->getScale() + Vector2f(speed * m_delta, speed * m_delta);
				m_obj->setScale(newZoom);
			}
		}
	};

	class Animations {
	public:
		list<Animation> stack;

		void add(Text * object, string numOfAnimation, float param, float duration, float delay) {
			Animation buffer(object, numOfAnimation, param, duration, delay);
			stack.push_back(buffer);
		}

		bool play(float delta) {
			list<Animation>::iterator it;
			it = stack.begin();
			while (it != stack.end()) {
				(*it).play(delta);
				if (!(*it).active) {
					it = stack.erase(it);
				}
				else {
					it++;
				}
			}
			if (stack.empty()) {
				return true;
			}
			else {
				return false;
			}
		}
	};
}

class Loading {
private:
	Font * font;
	vector<Text> textObjects;
	Ani::Animations animations;
public:
	string m_text;

	Loading(string text, Vector2f position) {
		font = new Font;
		if (!font->loadFromFile("Helvetica.otf"))
		{
			cout << "error with loading font";
		}
		m_text = text;
		makeTextObjects(position);
		addAnimations();
	}

	~Loading() {
		delete font;
	}

	void draw(RenderWindow & window) {
		vector<Text>::iterator iterator;
		iterator = textObjects.begin();

		while (iterator != textObjects.end()) {
			window.draw((*iterator));
			iterator++;
		}
	}
	bool animate(float delta) {
		return animations.play(delta);
	}
private:
	void makeTextObjects(Vector2f position) {
		textObjects.reserve(m_text.length());
		Text buffer;
		buffer.setColor(Color(0, 0, 0));
		buffer.setFont(*font);
		float left = position.x;
		float top = position.y;
		static const int CHAR_SHIFT = 4; //Сдвиг букв относительно друг-друга
		for (unsigned int i = 0; i < m_text.length(); i++) {
			buffer.setString(m_text[i]);
			buffer.setPosition(left, top);
			textObjects.push_back(buffer);
			left += buffer.getGlobalBounds().width + CHAR_SHIFT;
		}
	}
public:
	void addAnimations() {
		float delay = 200; //Задержка анимации каждой буквы
		int i = m_text.length();
		vector<Text>::iterator iterator;
		iterator = textObjects.begin();
		while (iterator != textObjects.end()) {
			animations.add(&(*iterator), "zoom", 0.1f, 500, i * delay);
			animations.add(&(*iterator), "zoom", 1, 500, 500 + i * delay);
			
			animations.add(&(*iterator), "left", 100.f, 1000, 0 + i * delay);
			animations.add(&(*iterator), "top", 100.f, 1000, 1000 + i * delay);
			animations.add(&(*iterator), "right", 100.f, 2500, 2000 + i * delay);
			animations.add(&(*iterator), "bottom", 100.f, 5000, 4500 + i * delay);
			animations.add(&(*iterator), "rotate", -200.f, 350, 0 + i * delay);
			animations.add(&(*iterator), "rotate", 200.f, 500, 350 + i * delay);
			animations.add(&(*iterator), "rotate", -360.f, 500, 850 + i * delay);
			animations.add(&(*iterator), "rotate", 360.f, 3000, 2000 + i * delay);
			animations.add(&(*iterator), "rotate", -360.f, 3000, 9000 + i * delay);
			animations.add(&(*iterator), "opacity", 50, 1000, 2000 + i * delay);
			animations.add(&(*iterator), "opacity", 250, 1000, 3000 + i * delay);
			animations.add(&(*iterator), "opacity", 0, 1000, 9500 + i * delay);
			animations.add(&(*iterator), "opacity", 255, 1000, 11000 + i * delay);
			
			iterator++;
			i--;
		}
	}
};

int main()
{
	VideoMode vidMode;
	vidMode.width = 800;
	vidMode.height = 600;

	ContextSettings settings;
	settings.antialiasingLevel = 8;
	RenderWindow window(vidMode, "First iteration", Style::Default, settings);
	window.setVerticalSyncEnabled(true);

	Loading loading("loading...", Vector2f(300, 200));

	Clock clock;
	float time;
	while (window.isOpen()) {
		window.clear(Color::White);
		time = float(clock.getElapsedTime().asMilliseconds());
		clock.restart();
		if (loading.animate(time)) {
			loading.addAnimations();
		}
		loading.draw(window);
		window.display();
	}
    return 0;
}

