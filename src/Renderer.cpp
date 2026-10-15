/**
 * @file Renderer.cpp
 * @brief Implementation of rendering routines using Raylib functions.
 */
#include "Renderer.hpp"
#include <raylib.h>

 /**
  * @note Architecture Note: Anonymous Namespace.
  * Everything inside an anonymous namespace is only visible within this specific .cpp file.
  * It's modern C++'s equivalent of the 'static' keyword for functions. We use it to hide
  * helper functions (like type converters) from the rest of the project, avoiding naming collisions.
  */
namespace {
	/**
	 * @brief Converts our custom logic Vec2 to Raylib's Vector2 for drawing.
	 * @details This is the translation layer. Our core game doesn't know Raylib types exist,
	 * so we convert them right before rendering.
	 */
	constexpr Vector2 ToRaylibVec(const Vec2& v) noexcept {
		return Vector2{ v.x, v.y };
	}
}

void DrawGame(
	const GameState& state,
	const ScreenSize& screen
) {
	BeginDrawing();
	ClearBackground(DARKGRAY);

	// 1. UI Layer
	DrawFPS(10, 10);
	DrawText(TextFormat("X: %.1f | Y: %.1f", state.player.pos.x, state.player.pos.y), 10, 30, 20, RED);
	DrawText(TextFormat("HP: %d", state.player.hp), 10, 50, 20, BLUE);
	DrawText(TextFormat("SCORE: %d", state.score), 10, 70, 20, ORANGE);

	// 2. Player Layer
	DrawCircleV(ToRaylibVec(state.player.pos), Player::RADIUS, RED);
	const Vec2 barrel = state.player.GetBarrelEnd();
	DrawLineEx(ToRaylibVec(state.player.pos), ToRaylibVec(barrel), 3.0f, BLUE);

	// 3. Entities Layer
	for (const auto& enemy : state.enemies) {
		if (enemy.isActive) {
			DrawCircleV(ToRaylibVec(enemy.pos), Enemy::RADIUS, ORANGE);
		}
	}

	for (const auto& bullet : state.bullets) {
		if (bullet.isActive) {
			DrawCircleV(ToRaylibVec(bullet.pos), Bullet::RADIUS, GREEN);
		}
	}

	// 4. Overlay Layer (Game Over Screen)
	if (state.isGameOver) {
		// Fade creates a transparent overlay, dimming the active background.
		DrawRectangle(0, 0, screen.width, screen.height, Fade(BLACK, 0.7f));

		const char* gameOverText = "Game Over, press R to restart";
		constexpr int fontSize = 40;
		const int textWidth = MeasureText(gameOverText, fontSize);

		// Centering math for text
		DrawText(
			gameOverText,
			(screen.width - textWidth) / 2,
			(screen.height - fontSize) / 2,
			fontSize,
			RED
		);
	}

	EndDrawing();
}