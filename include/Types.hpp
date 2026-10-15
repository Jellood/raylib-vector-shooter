/**
 * @file Types.hpp
 * @brief Core data structures and state definitions for the game.
 *
 * @note Architecture Note: Object Pooling vs Dynamic Allocation.
 * In this project, we use std::array with a fixed MAX_COUNT for entities (Enemies, Bullets)
 * instead of dynamically resizing containers like std::vector. This acts as a simple
 * "Object Pool". In game dev, allocating memory during the game loop (via new/malloc)
 * causes performance spikes and memory fragmentation. Reusing pre-allocated memory
 * (checking the `isActive` flag) is a fundamental optimization technique.
 */
#pragma once
#include "Math.hpp"
#include <array>
#include <cstddef>

 /**
  * @struct ScreenSize
  * @brief Represents the dimensions of the game window.
  */
struct ScreenSize {
	int width = 800;  ///< Window width in pixels
	int height = 600; ///< Window height in pixels

	/**
	 * @brief Returns the width as a float for continuous math operations.
	 */
	constexpr float WidthF() const noexcept { return static_cast<float>(width); }

	/**
	 * @brief Returns the height as a float for continuous math operations.
	 */
	constexpr float HeightF() const noexcept { return static_cast<float>(height); }
};

/**
 * @struct Player
 * @brief Represents the player character and its properties.
 *
 * @note All balancing constants are defined as `static constexpr` inside the struct.
 * This keeps the global namespace clean, makes them evaluated at compile-time,
 * and puts all tweakable values in one central place.
 */
struct Player {
	static constexpr float SPEED = 300.0f;          ///< Movement speed in pixels per second
	static constexpr float RADIUS = 20.0f;          ///< Collision radius
	static constexpr int MAX_HP = 3;                ///< Maximum health points
	static constexpr float BARREL_LENGTH = 25.0f;   ///< Visual and logical length of the gun barrel
	static constexpr float AIM_DEADZONE_SQ = 16.0f; ///< Squared deadzone distance to prevent aiming jitter near the center
	static constexpr float FIRE_COOLDOWN = 0.15f;   ///< Minimum time between shots

	Vec2 pos{};                             ///< Current position in the world
	Vec2 aim{ 1.0f, 0.0f };                 ///< Current aiming direction (normalized)
	int hp = MAX_HP;                        ///< Current health points
	float shootTimer = FIRE_COOLDOWN;       ///< Timer to track weapon cooldown

	/**
	 * @brief Calculates the exact world position of the barrel end.
	 * @return Coordinates of the barrel tip for bullet spawning and rendering.
	 */
	constexpr Vec2 GetBarrelEnd() const noexcept {
		return Vec2{
			pos.x + aim.x * BARREL_LENGTH,
			pos.y + aim.y * BARREL_LENGTH
		};
	}
};

/**
 * @struct Enemy
 * @brief Represents an enemy entity.
 */
struct Enemy {
	static constexpr float SPEED = 100.0f;          ///< Movement speed in pixels per second
	static constexpr float RADIUS = 20.0f;          ///< Collision radius
	static constexpr float EPSILON_SQ = 0.0001f;    ///< Small value to prevent jittering when reaching the target
	static constexpr std::size_t MAX_COUNT = 32;    ///< Size of the pre-allocated object pool

	bool isActive = false;  ///< Flag determining if the object is currently "alive" in the pool
	Vec2 pos{};             ///< Current position in the world
	int damage = 1;         ///< Damage dealt to the player on collision
};

/**
 * @struct Bullet
 * @brief Represents a projectile fired by the player.
 */
struct Bullet {
	static constexpr float SPEED = 600.0f;          ///< Travel speed in pixels per second
	static constexpr float RADIUS = 5.0f;           ///< Collision radius
	static constexpr float OFFSCREEN_MARGIN = 50.0f;///< Distance off-screen before despawning and returning to the pool
	static constexpr std::size_t MAX_COUNT = 32;    ///< Size of the pre-allocated object pool

	bool isActive = false;  ///< Flag determining if the bullet is currently in flight
	Vec2 pos{};             ///< Current position in the world
	Vec2 dir{};             ///< Direction of travel (normalized)
};

/**
 * @struct GameState
 * @brief Encapsulates the entire state of a game session.
 * @details Grouping all state variables in one struct makes it trivially easy
 * to reset the game, save/load state, or pass data to the renderer.
 */
struct GameState {
	Player player{};                                     ///< The player entity
	std::array<Enemy, Enemy::MAX_COUNT> enemies{};       ///< Fixed-size pool of enemies
	std::array<Bullet, Bullet::MAX_COUNT> bullets{};     ///< Fixed-size pool of bullets

	int score = 0;                  ///< Current player score
	float enemySpawnTimer = 0.0f;   ///< Timer tracking enemy spawn delays
	float spawnInterval = 2.0f;     ///< Current time required between enemy spawns (decreases as score goes up)
	bool isGameOver = false;        ///< Flag indicating if the player has died
};