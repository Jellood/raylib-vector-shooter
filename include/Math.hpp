/**
 * @file Math.hpp
 * @brief Custom mathematical utilities and 2D vector structures.
 *
 * @note Educational Note: Why create a custom Vec2 instead of using Raylib's Vector2?
 * This is an educational project. Writing fundamental mathematical structures and
 * operations (like vector normalization and length calculations) from scratch helps
 * to deeply understand how game math works under the hood, rather than relying on
 * pre-built framework functions. It builds a solid foundation for computer graphics.
 */
#pragma once
#include <cmath>

 /**
  * @struct Vec2
  * @brief A lightweight 2D vector representation.
  */
struct Vec2 {
	float x = 0.0f; ///< X axis coordinate
	float y = 0.0f; ///< Y axis coordinate

	/**
	 * @brief Calculates the squared length of the vector.
	 *
	 * @details Under-the-hood Concept:
	 * Calculating a square root (std::sqrt) is an expensive CPU operation.
	 * For collision detection, instead of calculating the exact distance, we compare
	 * the *squared distance* against the *squared radius*. This yields the exact
	 * same logical result but avoids the costly sqrt operation, which is a standard
	 * optimization technique in game development.
	 *
	 * @return The squared length (x^2 + y^2).
	 */
	constexpr float LengthSq() const noexcept {
		return x * x + y * y;
	}

	/**
	 * @brief Calculates the actual length (magnitude) of the vector.
	 * @note Uses std::sqrt. Prefer LengthSq() when exact length isn't strictly necessary.
	 * @return The actual length.
	 */
	float Length() const noexcept {
		return std::sqrt(LengthSq());
	}

	/**
	 * @brief Normalizes the vector (scales its length to exactly 1.0) while keeping direction.
	 *
	 * @details Used heavily for defining movement and shooting directions.
	 * We check if lengthSq > 0.00000001f to prevent fatal divide-by-zero errors
	 * in cases where the vector is zero (e.g., player is not pressing any movement keys).
	 */
	void Normalize() noexcept {
		const float lengthSq = LengthSq();
		if (lengthSq > 0.00000001f) {
			// Multiplying by the inverse is computationally slightly faster than division.
			const float invLength = 1.0f / std::sqrt(lengthSq);
			x *= invLength;
			y *= invLength;
		}
		else {
			x = 0.0f;
			y = 0.0f;
		}
	}

	// Operator overloads for clean vector arithmetic.

	constexpr Vec2 operator+(const Vec2& other) const noexcept {
		return Vec2{ x + other.x, y + other.y };
	}

	constexpr Vec2 operator-(const Vec2& other) const noexcept {
		return Vec2{ x - other.x, y - other.y };
	}

	constexpr Vec2 operator*(float scalar) const noexcept {
		return Vec2{ x * scalar, y * scalar };
	}
};