/**
 * @file main.cpp
 * @brief Entry point and main game loop for the Raylib Shooter game.
 */
#include <raylib.h>
#include <algorithm>
#include "Types.hpp"
#include "GameLogic.hpp"
#include "Renderer.hpp"

int main() {
	constexpr ScreenSize screen{ 800, 600 };
	InitWindow(screen.width, screen.height, "Raylib Shooter");
	SetTargetFPS(144);

	GameState state{};
	ResetGame(state, screen);

	// The Main Game Loop (Input -> Update -> Render)
	while (!WindowShouldClose()) {

		// Educational Note: Delta Time Clamping.
		// GetFrameTime() returns how much time passed since the last frame.
		// If the user drags the window, the OS pauses the app. When it resumes, 
		// GetFrameTime() might be huge (e.g., 2.0 seconds). If we multiply enemy speed 
		// by 2 seconds, they will teleport through walls or the player.
		// We use std::min(..., 0.05f) to clamp the maximum step size. If a lag spike happens,
		// the game slows down visually instead of breaking physics.
		const float dt = std::min(GetFrameTime(), 0.05f);

		if (!state.isGameOver) {
			// Spawn Timer Logic
			state.enemySpawnTimer += dt;
			if (state.enemySpawnTimer >= state.spawnInterval) {
				// If we successfully spawn an enemy, subtract the interval (keeps overflow timing accurate).
				// If we fail (pool is full), cap it so it spawns immediately when a slot opens up.
				if (SpawnEnemy(state.enemies, screen)) {
					state.enemySpawnTimer -= state.spawnInterval;
				}
				else {
					state.enemySpawnTimer = state.spawnInterval;
				}
			}

			// Update Phase
			UpdatePlayer(state.player, dt, screen);
			UpdateEnemy(state.enemies, state.player.pos, dt);
			TryFireBullet(state.bullets, state.player, dt);
			UpdateBullets(state.bullets, dt, screen);

			// Collision Phase
			CheckBulletEnemyCollisions(state.bullets, state.enemies, state.score);
			CheckEnemyPlayerCollision(state.enemies, state.player);

			// Dynamic Difficulty (Scaling)
			// The more score you have, the faster enemies spawn.
			// std::max ensures it never drops below 0.3 seconds per spawn.
			state.spawnInterval = std::max(0.3f, 2.0f - static_cast<float>(state.score) * 0.05f);

			// Check Loss Condition
			if (state.player.hp <= 0) {
				state.isGameOver = true;
			}
		}
		else {
			// When game over, pause physics and wait for 'R' input.
			if (IsKeyPressed(KEY_R)) {
				ResetGame(state, screen);
			}
		}

		// Draw Phase (happens every frame regardless of game over state)
		DrawGame(state, screen);
	}

	CloseWindow();
	return 0;
}