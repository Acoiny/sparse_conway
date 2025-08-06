#pragma once

#include <SFML/Graphics.hpp>
#include <cstddef>
#include <unordered_set>

struct HASH_FUNCTION
{
    std::size_t operator()(const sf::Vector2i &v) const
    {
        return std::size_t(v.x) << 32 | v.y;
    }
};

class Conway
{
  public:
    Conway(const std::string &text, int width = 480, int height = 480);

    int run();

    // private methods
  private:
    void drawCells();
    void drawUi(float dt);

    void handleKey(sf::Event &event, bool pressed);
    void handleMouse(sf::Event &event, bool pressed);
    void handleMouseWheel(sf::Event &event);

    void simulationStep();

    // member variables
  private:
    sf::RenderWindow m_window;

    sf::View m_view;

    std::unordered_set<sf::Vector2i, HASH_FUNCTION> m_cells;
    std::unordered_set<sf::Vector2i, HASH_FUNCTION> m_cells_next;

    bool m_spaceHeld = false;
    bool m_mouseLeftHeld = false;
    bool m_mouseRightHeld = false;
    sf::Vector2i m_movement;
    float m_zoom = 1;

    size_t m_generation = 0;

    sf::Font m_font;
    sf::Text m_text;
};
