/**
 * @file GameLogic.hpp
 * @brief Declarations of core game logic, physics updates, and collision checks.
 *
 * @note Educational Note: Separation of Concerns.
 * Even in a learning project, it is crucial to separate the "Update" step (GameLogic)
 * from the "Draw" step (Renderer). This file contains purely logical operations
 * (math, physics, timers). It does not know anything about colors, pixels, or screen drawing.
 * This structure mirrors how professional game engines handle the main loop.
 */
#pragma once
#include "Types.hpp"

 /**
  * @brief Updates the player's position based on keyboard input and calculates aiming.
  * @param player Reference to the player object.
  * @param dt Delta time (time elapsed since last frame) to ensure framerate-independent movement.
  * @param screen Screen dimensions to clamp the player's position within bounds.
  */
void UpdatePlayer(Player& player, float dt, const ScreenSize& screen);

/**
 * @brief Attempts to spawn a new enemy from the object pool.
 * @details Selects a random edge of the screen and places an inactive enemy there.
 * @param enemies The pool of enemy objects.
 * @param screen Screen dimensions to calculate off-screen spawn coordinates.
 * @return True if an enemy was successfully spawned, false if the pool is full.
 */
bool SpawnEnemy(std::array<Enemy, Enemy::MAX_COUNT>& enemies, const ScreenSize& screen);

/**
 * @brief Moves all active enemies towards the target position (usually the player).
 * @param enemies The pool of enemy objects.
 * @param targetPos The coordinate the enemies should seek.
 * @param dt Delta time for framerate-independent movement.
 */
void UpdateEnemy(std::array<Enemy, Enemy::MAX_COUNT>& enemies, const Vec2& targetPos, float dt);

/**
 * @brief Attempts to fire a bullet if the player is clicking and the weapon is off cooldown.
 * @details Finds the first inactive bullet in the pool, sets its position to the barrel tip,
 * and assigns it the player's current aim direction.
 * @param bullets The pool of bullet objects.
 * @param player Reference to the player object.
 * @param dt Delta time to update the player's internal shoot cooldown timer.
 */
void TryFireBullet(std::array<Bullet, Bullet::MAX_COUNT>& bullets, Player& player, float dt);

/**
 * @brief Moves active bullets and despawns them if they travel too far off-screen.
 * @param bullets The pool of bullet objects.
 * @param dt Delta time for framerate-independent movement.
 * @param screen Screen dimensions used to calculate culling boundaries.
 */
void UpdateBullets(std::array<Bullet, Bullet::MAX_COUNT>& bullets, float dt, const ScreenSize& screen);

/**
 * @brief Performs Circle-to-Circle collision detection between bullets and enemies.
 * @details If a collision is detected, both entities are marked inactive (returned to their pools),
 * and the player's score is incremented.
 * @param bullets The pool of bullet objects.
 * @param enemies The pool of enemy objects.
 * @param score Reference to the score counter.
 */
void CheckBulletEnemyCollisions(std::array<Bullet, Bullet::MAX_COUNT>& bullets, std::array<Enemy, Enemy::MAX_COUNT>& enemies, int& score);

/**
 * @brief Performs Circle-to-Circle collision detection between enemies and the player.
 * @details If a collision occurs, the enemy is despawned and the player takes damage.
 * @param enemies The pool of enemy objects.
 * @param player Reference to the player object.
 */
void CheckEnemyPlayerCollision(std::array<Enemy, Enemy::MAX_COUNT>& enemies, Player& player);

/**
 * @brief Resets the game to its initial state (full HP, centered position, empty entity pools).
 * @param state Reference to the global game state object.
 * @param screen Screen dimensions used to calculate the starting center position.
 */
void ResetGame(GameState& state, const ScreenSize& screen);