#pragma once

enum class Emotion: int
{
	Unidentified = -1,
	Joy          = 0,
	Confidence   = 1,
	Fear         = 2,
	Surprised    = 3,
	Sadness      = 4,
	Disgust      = 5,
	Anger        = 6,
	Anticipation = 7,
};

inline int emotionToInt(Emotion emotion)
{
	switch (emotion)
	{
		case Emotion::Unidentified:  return -1;
		case Emotion::Joy:           return 0;
		case Emotion::Confidence:    return 1;
		case Emotion::Fear:          return 2;
		case Emotion::Surprised:     return 3;
		case Emotion::Sadness:       return 4;
		case Emotion::Disgust:       return 5;
		case Emotion::Anger:         return 6;
		case Emotion::Anticipation:  return 7;
	}
	return -1;
}

enum class Intensity: int
{
	Unidentified = -1,
	Small  = 0,
	Mid    = 1,
	Strong = 2,
};

inline int intensityToInt(Intensity intensity)
{
	switch (intensity)
	{
		case Intensity::Unidentified:  return -1;
		case Intensity::Small:         return 0;
		case Intensity::Mid:           return 1;
		case Intensity::Strong:        return 2;
	}
	return -1;
}

