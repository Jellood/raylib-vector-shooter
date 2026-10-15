/**
 * @file GameLogic.cpp
 * @brief Implementation of the game logic, physics, and state updates.
 */
#include "GameLogic.hpp"
#include <raylib.h>
#include <algorithm>

void UpdatePlayer(
	Player& player,
	float dt,
	const ScreenSize& screen
) {
	Vec2 moveInput{};

	if (IsKeyDown(KEY_W)) moveInput.y -= 1.0f;
	if (IsKeyDown(KEY_S)) moveInput.y += 1.0f;
	if (IsKeyDown(KEY_D)) moveInput.x += 1.0f;
	if (IsKeyDown(KEY_A)) moveInput.x -= 1.0f;

	// Educational Note: Why do we normalize movement input?
	// If a player presses 'W' (1.0) and 'D' (1.0), the diagonal vector length is ~1.41 (sqrt(2)).
	// Without normalization, the player would move 41% faster diagonally! 
	// Normalizing restricts the length to exactly 1.0, ensuring consistent speed in all directions.
	moveInput.Normalize();

	player.pos.x += moveInput.x * Player::SPEED * dt;
	player.pos.y += moveInput.y * Player::SPEED * dt;

	// We use std::clamp instead of writing multiple if-statements.
	// It nicely locks the player inside the screen bounds, taking the radius into account 
	// so the character sprite doesn't visually bleed over the window edge.
	player.pos.x = std::clamp(player.pos.x, Player::RADIUS, screen.WidthF() - Player::RADIUS);
	player.pos.y = std::clamp(player.pos.y, Player::RADIUS, screen.HeightF() - Player::RADIUS);

	const Vector2 mousePos = GetMousePosition();
	Vec2 toMouse{ mousePos.x - player.pos.x, mousePos.y - player.pos.y };

	// Deadzone prevents aiming jitter when the mouse cursor is exactly on top of the player.
	if (toMouse.LengthSq() > Player::AIM_DEADZONE_SQ) {
		toMouse.Normalize();
		player.aim = toMouse;
	}
}

bool SpawnEnemy(
	std::array<Enemy, Enemy::MAX_COUNT>& enemies,
	const ScreenSize& screen
) {
	for (auto& enemy : enemies) {
		if (enemy.isActive) continue;

		// Select a random edge (0: Top, 1: Bottom, 2: Left, 3: Right)
		const int side = GetRandomValue(0, 3);
		switch (side) {
		case 0: // Top edge
			enemy.pos.x = static_cast<float>(GetRandomValue(0, screen.width));
			enemy.pos.y = -Enemy::RADIUS;
			break;
		case 1: // Bottom edge
			enemy.pos.x = static_cast<float>(GetRandomValue(0, screen.width));
			enemy.pos.y = screen.HeightF() + Enemy::RADIUS;
			break;
		case 2: // Left edge
			enemy.pos.x = -Enemy::RADIUS;
			enemy.pos.y = static_cast<float>(GetRandomValue(0, screen.height));
			break;
		case 3: // Right edge
			enemy.pos.x = screen.WidthF() + Enemy::RADIUS;
			enemy.pos.y = static_cast<float>(GetRandomValue(0, screen.height));
			break;
		default:
			break;
		}

		enemy.isActive = true;
		return true; // Successfully spawned
	}
	return false; // The pool is full
}

void UpdateEnemy(
	std::array<Enemy, Enemy::MAX_COUNT>& enemies,
	const Vec2& targetPos,
	float dt
) {
	for (auto& enemy : enemies) {
		if (!enemy.isActive) continue;

		Vec2 dirToPlayer{ targetPos - enemy.pos };

		// Educational Note: EPSILON_SQ check prevents "shaking".
		// If the enemy reaches the exact pixel of the player, normalization might flip rapidly,
		// causing visual jitter. Checking if they are slightly apart solves this.
		if (dirToPlayer.LengthSq() > Enemy::EPSILON_SQ) {
			dirToPlayer.Normalize();
			enemy.pos.x += dirToPlayer.x * Enemy::SPEED * dt;
			enemy.pos.y += dirToPlayer.y * Enemy::SPEED * dt;
		}
	}
}

void TryFireBullet(
	std::array<Bullet, Bullet::MAX_COUNT>& bullets,
	Player& player,
	float dt
) {
	player.shootTimer = std::min(player.shootTimer + dt, Player::FIRE_COOLDOWN);

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && player.shootTimer >= Player::FIRE_COOLDOWN) {
		for (auto& bullet : bullets) {
			if (bullet.isActive) continue;

			bullet.isActive = true;
			bullet.dir = player.aim;
			bullet.pos = player.GetBarrelEnd(); // Spawn bullet at the tip of the gun
			player.shootTimer = 0.0f;           // Reset cooldown
			break;
		}
	}
}

void UpdateBullets(
	std::array<Bullet, Bullet::MAX_COUNT>& bullets,
	float dt,
	const ScreenSize& screen
) {
	// We add an offscreen margin so bullets don't despawn immediately when touching the border.
	// They travel a bit further out, ensuring the visual projectile fully leaves the screen smoothly.
	const float minX = -Bullet::OFFSCREEN_MARGIN;
	const float maxX = screen.WidthF() + Bullet::OFFSCREEN_MARGIN;
	const float minY = -Bullet::OFFSCREEN_MARGIN;
	const float maxY = screen.HeightF() + Bullet::OFFSCREEN_MARGIN;

	for (auto& bullet : bullets) {
		if (!bullet.isActive) continue;

		bullet.pos.x += bullet.dir.x * Bullet::SPEED * dt;
		bullet.pos.y += bullet.dir.y * Bullet::SPEED * dt;

		// Despawn bullet if it goes outside the padded screen area
		if (bullet.pos.x > maxX || bullet.pos.x < minX ||
			bullet.pos.y > maxY || bullet.pos.y < minY) {
			bullet.isActive = false;
		}
	}
}

void CheckBulletEnemyCollisions(
	std::array<Bullet, Bullet::MAX_COUNT>& bullets,
	std::array<Enemy, Enemy::MAX_COUNT>& enemies,
	int& score
) {
	constexpr float collisionDist = Enemy::RADIUS + Bullet::RADIUS;
	constexpr float collisionDistSq = collisionDist * collisionDist;

	// Brute-force O(N*M) collision checking.
	// For small pools (32x32 = 1024 checks max), this is extremely fast.
	// If pools were much larger (e.g., 10,000 entities), we would need spatial partitioning (Quadtrees, Grids).
	for (auto& enemy : enemies) {
		if (!enemy.isActive) continue;

		for (auto& bullet : bullets) {
			if (!bullet.isActive) continue;

			const Vec2 diff{ enemy.pos - bullet.pos };
			// Using squared distance to bypass the heavy std::sqrt operation!
			if (diff.LengthSq() < collisionDistSq) {
				enemy.isActive = false;
				bullet.isActive = false;
				score++;
				break; // One bullet hits one enemy, so we stop checking this enemy
			}
		}
	}
}

void CheckEnemyPlayerCollision(
	std::array<Enemy, Enemy::MAX_COUNT>& enemies,
	Player& player
) {
	constexpr float collisionDist = Enemy::RADIUS + Player::RADIUS;
	constexpr float collisionDistSq = collisionDist * collisionDist;

	for (auto& enemy : enemies) {
		if (!enemy.isActive) continue;

		const Vec2 diff{ player.pos - enemy.pos };
		if (diff.LengthSq() < collisionDistSq) {
			enemy.isActive = false; // "Kamikaze" behaviour: enemy destroys itself on hit
			player.hp = std::max(0, player.hp - enemy.damage);
		}
	}
}

void ResetGame(
	GameState& state,
	const ScreenSize& screen
) {
	state.player.hp = Player::MAX_HP;
	state.player.pos.x = screen.WidthF() * 0.5f;
	state.player.pos.y = screen.HeightF() * 0.5f;
	state.player.aim = Vec2{ 1.0f, 0.0f };
	state.player.shootTimer = Player::FIRE_COOLDOWN;

	for (auto& enemy : state.enemies) {
		enemy.isActive = false;
	}
	for (auto& bullet : state.bullets) {
		bullet.isActive = false;
	}

	state.enemySpawnTimer = 0.0f;
	state.spawnInterval = 2.0f;
	state.score = 0;
	state.isGameOver = false;
}