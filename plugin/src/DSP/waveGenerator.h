/*
  ==============================================================================

    waveGenerator.h
    Created: 26 May 2024 7:26:26am
    Author:  Erik Jourgensen

  ==============================================================================
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class WaveGenerator
{
public:
    enum class WaveType { Sine, Sawtooth, Square };
    
    WaveGenerator(): phase(0.0f), increment(0.0f) {}
    ~WaveGenerator() = default;
    
    void setFrequency(float frequency, float sampleRate)
    {
        increment = frequency / sampleRate;
    }
    
    void setWaveType(WaveType type)
    {
        waveType = type;
    }
    
    float getNextSample()
    {
        float currentSample = 0;
        
        if (waveType == WaveType::Sine){
            
            currentSample = std::sin(phase * 2.0 * M_PI);
        }
        else if (waveType == WaveType::Sawtooth){
            
            currentSample = 2.0f * phase - 1.0f;
        }
        else if (waveType == WaveType::Square){
            currentSample = (phase < 0.5f) ? 1.0 : -1.0;
        }
        
            phase += increment;
        
            if (phase >= 1.0f)
                phase -= 1.0f;
        
        return currentSample;
    }
    
private:
    float phase;
    float increment;
    WaveType waveType;
    juce::Random random;
};
