#include "game.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>

constexpr int IDLE_FRAMERATE = 30;

Conway::Conway(const std::string &text, int width, int height)
    : m_window(sf::VideoMode(width, height), text, sf::Style::Close),
      m_view(m_window.getView())
{
    m_window.setFramerateLimit(IDLE_FRAMERATE);

    std::string fontPath = "/usr/share/fonts/truetype/DejaVuSans.ttf";

    if (!m_font.loadFromFile(fontPath))
    {
        throw std::runtime_error(
            std::format("Unable to load font {}", fontPath));
    }

    m_text.setScale(0.8, 0.8);
    m_text.setFont(m_font);
    m_text.setFillColor(sf::Color::White);
}

void Conway::drawCells()
{
    // m_window.setView(m_view);

    sf::RectangleShape rect({1, 1});

    m_uiData.visibleCells = 0;

    for (auto &c : m_cells)
    {
        auto pointOnScreen = m_window.mapCoordsToPixel(sf::Vector2f(c));
        int x = pointOnScreen.x;
        int y = pointOnScreen.y;

        // skip drawing all cells that are not visible
        if (x < 0 || y < 0 || uint(x) > m_window.getSize().x ||
            uint(y) > m_window.getSize().y)
            continue;

        rect.setPosition(c.x, c.y);
        m_window.draw(rect);
        m_uiData.visibleCells++;
    }
}

void Conway::drawUi(float dt)
{
    std::string mouseTool;

    switch (m_uiData.mouseTool)
    {
    case MouseTool::ONE_ONE:
        mouseTool = "1x1";
        break;
    case MouseTool::HUND_HUND:
        mouseTool = "100x100";
        break;
    case MouseTool::DEL_ONE_ONE:
        mouseTool = "del 1x1";
        break;
    case MouseTool::DEL_HUND_HUND:
        mouseTool = "del 100x100";
        break;
    default:
        mouseTool = "invalid!";
        break;
    }

    m_window.setView(m_window.getDefaultView());
    m_text.setString("FPS: " + std::to_string(int(1 / dt)) +
                     "\nCells: " + std::to_string(m_uiData.visibleCells) + '/' +
                     std::to_string(m_cells.size()) + "\nTool: " + mouseTool);
    m_window.draw(m_text);
}

int Conway::run()
{
    sf::Clock timer;
    timer.restart();

    float acc = 0;

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

        float dt = timer.getElapsedTime().asSeconds();
        timer.restart();

        m_window.setView(m_view);

        if (m_mouseLeftHeld)
        {
            executeTool(m_uiData.mouseTool);
        }

        if (m_spaceHeld)
        {
            simulationStep();
        }

        // accumulate the passed time, and 60 times per second
        // allow movement
        acc += dt;
        if (acc > (1.f / 60))
        {
            acc = 0;
            m_view.move(m_movement.x * m_zoom, m_movement.y * m_zoom);
        }

        m_window.clear();
        drawCells();
        drawUi(dt);
        m_window.display();
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

    // reserve a multiple, because each cell COULD provide 8 possibles
    // but probably will have less
    possibles.reserve(m_cells.size() * 4);

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
    m_cells_next.reserve(m_cells.size());

    m_uiData.generation++;
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

        // unlimited framerate when not running
        if (m_spaceHeld)
            m_window.setFramerateLimit(0);
        else
            m_window.setFramerateLimit(IDLE_FRAMERATE);
        break;
    case Key::W:
        m_movement.y = pressed ? -10 : 0;
        break;
    case Key::A:
        m_movement.x = pressed ? -10 : 0;
        break;
    case Key::S:
        m_movement.y = pressed ? 10 : 0;
        break;
    case Key::D:
        m_movement.x = pressed ? 10 : 0;
        break;
    // if the 'C' key is pressed, do one simulation step
    case Key::C:
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
        m_mouseLeftHeld = pressed;
        break;
    case sf::Mouse::Button::Right:
    {
        if (!pressed)
            break;
        int val = (int)m_uiData.mouseTool;
        val++;
        val %= (int)MouseTool::MOUSE_TOOL_MAX_VALUE;
        m_uiData.mouseTool = (MouseTool)val;
    }
    break;
    default:
        break;
    }
}

void Conway::handleMouseWheel(sf::Event &event)
{
    int delta = -event.mouseWheelScroll.delta;

    float zoomValue = 1 + (delta * 0.1);
    m_zoom *= zoomValue;
    m_view.zoom(zoomValue);
}

void Conway::executeTool(MouseTool tool)
{
    const auto &pos = sf::Vector2i(
        m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window)));
    switch (tool)
    {
    case MouseTool::ONE_ONE:
        m_cells.insert(pos);
        break;
    case MouseTool::HUND_HUND:
        for (int x = pos.x - 50; x < pos.x + 50; x++)
            for (int y = pos.y - 50; y < pos.y + 50; y++)
            {
                if (std::rand() % 2)
                    m_cells.insert({x, y});
            }
        break;
    case MouseTool::DEL_ONE_ONE:
        m_cells.erase(pos);
        break;
    case MouseTool::DEL_HUND_HUND:
        for (int x = pos.x - 50; x < pos.x + 50; x++)
            for (int y = pos.y - 50; y < pos.y + 50; y++)
                m_cells.erase({x, y});
        break;
    default:

        break;
    }
}
