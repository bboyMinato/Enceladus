#pragma once
#include <type_traits>

template<typename T>
struct Vector2
{
	T x{};
	T y{};

	constexpr Vector2() = default;

	constexpr Vector2(T x, T y)
		: x(x), y(y)
	{
	}

	constexpr Vector2 operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}

	constexpr Vector2 operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}

	constexpr Vector2 operator*(T scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}

	constexpr Vector2& operator+=(const Vector2& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	constexpr Vector2& operator-=(const Vector2& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	constexpr Vector2& operator*=(T scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	constexpr bool operator==(const Vector2& other) const = default;
};

using Vector2i = Vector2<int>;
using Vector2f = Vector2<float>;