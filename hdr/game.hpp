#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
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

    void draw();

    // private methods
  private:
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
    bool m_mouseHeld = false;

    size_t m_generation = 0;
};
