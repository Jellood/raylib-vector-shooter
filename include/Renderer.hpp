/**
 * @file Renderer.hpp
 * @brief Declarations for drawing the game state using Raylib.
 *
 * @note Educational Note: Read-Only State (Model-View Separation).
 * Notice that DrawGame takes `const GameState&`. The renderer is strictly forbidden
 * from modifying the game's logic or state. Its only job is to read the data (Model)
 * and display it on the screen (View). This prevents "spaghetti code" where a simple
 * visual effect accidentally changes a player's hitpoints or position.
 */
#pragma once
#include "Types.hpp"

 /**
  * @brief Renders the entire game frame to the active window.
  * @param state A read-only reference to the current state of the game.
  * @param screen Screen dimensions used for centering text and overlay elements.
  */
void DrawGame(const GameState& state, const ScreenSize& screen);