#pragma once

struct SpriteAnimationComponent
{
	SpriteAnimationComponent() = default;

	SpriteAnimationComponent(int frameWidth, int frameHeight, int frameCount, float frameDuration, int row = 0, int startFrame = 0, bool isLooping = true)
		: m_frameWidth(frameWidth),
		  m_frameHeight(frameHeight),
		  m_frameCount(frameCount),
		  m_frameDuration(frameDuration),
		  m_row(row),
		  m_startFrame(startFrame),
		  m_isLooping(isLooping)
	{
	}

	int m_frameWidth{ 0 };
	int m_frameHeight{ 0 };
	int m_frameCount{ 1 };
	float m_frameDuration{ 0.1f };

	float m_elapsedTime{ 0.0f };

	int m_row{ 0 };
	int m_startFrame{ 0 };
	int m_currentFrame{ 0 };

	bool m_isPlaying{ true };
	bool m_isLooping{ true };
};