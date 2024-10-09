/*
  ==============================================================================

    noiseGenerator.h
    Created: 23 May 2024 3:48:30pm
    Author:  Erik Jourgensen

  ==============================================================================
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class NoiseGenerator
{
public:
    
    NoiseGenerator(): noiseLevel(0.2f) {}
    ~NoiseGenerator() = default;
    
    float getNextSample()
    {
        return (random.nextFloat() * 2.0f - 1.0f) * noiseLevel;;
    }
    
    void updateNoiseLevel(float newNoiseLevel)
    {
        noiseLevel = newNoiseLevel;
    }
private:
    juce::Random random;
    float noiseLevel;
    
};
