/*
  ==============================================================================

    synthVoice.h
    Created: 20 May 2024 2:08:07pm
    Author:  Erik Jourgensen

  ==============================================================================
*/
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "synthSound.h"
#include "noiseGenerator.h"
#include "waveGenerator.h"

class SynthVoice : public juce::SynthesiserVoice

{
public:
    
    //==============================================================================
    
    SynthVoice() : waveGenerator(), noise(), amplitude(0.25), frequency(440.0), isActive(false), sampleRate(44100.0) {}
    
    ~SynthVoice() override = default;
    
    //==============================================================================

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*>(sound) != nullptr;
    
    }
    //==========================================================================
    
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override
    {
        frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        amplitude = velocity;
        waveGenerator.setFrequency(frequency, sampleRate);
        noise.updateNoiseLevel(0.0f);
        isActive = true;
        std:: cout << "midi note: "<< midiNoteNumber << " freq: " << frequency << std::endl;
    }
    //==========================================================================
    
    void stopNote (float velocity, bool allowTailoff) override
    {
        clearCurrentNote();
        isActive = false;
        
    }
    //==========================================================================
    void pitchWheelMoved (int newPitchWheelValue) override
    {
       
    }
    //==========================================================================
    
    void controllerMoved (int controllerNumber, int newControllerValue) override
    {
        
    }
    //==========================================================================
    
    void renderNextBlock (juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override
    {
        if (!isActive)
            return;
        
        float* channelDataLeft = outputBuffer.getWritePointer(0, startSample);
        float* channelDataRight = outputBuffer.getWritePointer(1, startSample);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            
            float waveSample = waveGenerator.getNextSample() * amplitude;
            
            float noiseSample = noise.getNextSample() * .3;
            
            
            channelDataLeft[sample] = waveSample + noiseSample;
            channelDataRight[sample] = waveSample + noiseSample;
            
        }
    }
    
    void updateWaveType (int newWaveType)
    {
        waveGenerator.setWaveType(static_cast<WaveGenerator::WaveType>(newWaveType));
    }
    
    
    void updateNoiseLevel (float newNoiseLevel)
    {
        noise.updateNoiseLevel(newNoiseLevel);
    }
    
    //==========================================================================
    
    
private:
//    SineWaveGenerator sineWaveGenerator;
    WaveGenerator waveGenerator;
    NoiseGenerator noise;
    
    double amplitude;
    double frequency;
    bool isActive;
    double phase;
    double sampleRate;
};
