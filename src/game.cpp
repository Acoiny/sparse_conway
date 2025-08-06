#include "game.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <unordered_set>
#include <utility>

Conway::Conway(const std::string &text, int width, int height)
    : m_window(sf::VideoMode(width, height), text), m_view(m_window.getView())
{
}

void Conway::draw()
{
    m_window.clear();

    sf::RectangleShape rect({1, 1});

    for (auto &c : m_cells)
    {
        rect.setPosition(c.x, c.y);
        m_window.draw(rect);
    }

    m_window.display();
}

int Conway::run()
{
    while (m_window.isOpen())
    {
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                m_window.close();

            // keyboard events
            if (event.type == sf::Event::KeyPressed)
                handleKey(event, true);
            else if (event.type == sf::Event::KeyReleased)
                handleKey(event, false);

            // mouse events
            if (event.type == sf::Event::MouseButtonPressed)
                handleMouse(event, true);
            else if (event.type == sf::Event::MouseButtonReleased)
                handleMouse(event, false);

            if (event.type == sf::Event::MouseWheelScrolled)
                handleMouseWheel(event);
        }

        if (m_mouseHeld)
        {
            const auto &pos = sf::Vector2i(
                m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)));
            m_cells.insert(pos);
        }

        if (m_spaceHeld)
        {
            simulationStep();
        }

        draw();
    }

    return 0;
}

void Conway::simulationStep()
{
    // steps:
    // 1. Iterate over currently alive cells and count their alive neighbours
    // 2. get their dead neighbours, they could possibly come alive next step
    // 3. count the alive neighbours of the possibles
    std::unordered_set<sf::Vector2i, HASH_FUNCTION> possibles;

    // 1. step
    for (auto &cell : m_cells)
    {
        int alive_neighbours = 0;

        for (int x = cell.x - 1; x <= cell.x + 1; x++)
        {
            for (int y = cell.y - 1; y <= cell.y + 1; y++)
            {
                sf::Vector2i nb{x, y};

                // skip itself
                if (nb == cell)
                    continue;

                // 2. step
                if (m_cells.contains(nb))
                    alive_neighbours++;
                else
                    possibles.insert(nb);
            }
        }

        // rule 1. alive cell, with less than two neigbours dies
        // rule 2. live cell, with 2 or 3 continues living
        // rule 3. live cell, with > 3 neighbours dies
        if (alive_neighbours == 2 || alive_neighbours == 3)
        {
            m_cells_next.insert(cell);
        }
    }

    // 3. step
    for (auto &cell : possibles)
    {
        int alive_neighbours = 0;

        for (int x = cell.x - 1; x <= cell.x + 1; x++)
        {
            for (int y = cell.y - 1; y <= cell.y + 1; y++)
            {
                sf::Vector2i nb{x, y};

                // skip itself
                // (technically unnecassary, but save one contains() call)
                if (nb == cell)
                    continue;

                // 2. step
                if (m_cells.contains(nb))
                    alive_neighbours++;
            }
        }

        if (alive_neighbours == 3)
            m_cells_next.insert(cell);
    }

    // now just swap the two sets
    std::swap(m_cells, m_cells_next);
    m_cells_next.clear();
    // m_cells_next.reserve(m_cells.size());

    m_generation++;
}

/**
 * If the space key is held down, the game will run a simulation step
 * each timestep.
 *
 * If the left mousebutton is held, the game will place a cell
 * at the mouse cursor.
 */
void Conway::handleKey(sf::Event &event, bool pressed)
{
    using Key = sf::Keyboard::Key;

    switch (event.key.code)
    {
    case Key::Space:
        m_spaceHeld = pressed;
        break;
    // if the 'S' key is pressed, do one simulation step
    case Key::S:
        if (!pressed)
            return;
        simulationStep();
        break;
    default:
        break;
    }
}

void Conway::handleMouse(sf::Event &event, bool pressed)
{
    switch (event.mouseButton.button)
    {
    case sf::Mouse::Button::Left:
        m_mouseHeld = pressed;
        break;
    default:
        break;
    }
}

void Conway::handleMouseWheel(sf::Event &event)
{
    int delta = -event.mouseWheelScroll.delta;

    float zoomValue = 1 + (delta * 0.1);
    m_view.zoom(zoomValue);
    m_window.setView(m_view);
}
