/*
  ==============================================================================

    synthSound.h
    Created: 20 May 2024 2:11:02pm
    Author:  Erik Jourgensen

  ==============================================================================
*/
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>


class SynthSound : public juce::SynthesiserSound

{
public:
    
SynthSound() = default;
~SynthSound() override = default;
    
     bool appliesToNote (int  /*midiNoteNumber*/) override
    {
        return true;
    }
    
    bool appliesToChannel (int /*midiChannel*/) override
    {
        
        return true;
    }
};
