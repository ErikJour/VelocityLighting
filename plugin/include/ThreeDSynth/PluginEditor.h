/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include<juce_gui_extra/juce_gui_extra.h>
#include "ThreeDSynth/PluginProcessor.h"

//==============================================================================
/**
*/

namespace ThreeDSynth {

class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    explicit NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;
  
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:

    using Resource = juce::WebBrowserComponent::Resource;  

    std::optional<Resource> getResource (const juce::String& url);

    void nativeFunction (const juce::Array<juce::var>& args,
                         juce::WebBrowserComponent::NativeFunctionCompletion completion);

    juce::WebBrowserComponent webViewGui;
    
    NewProjectAudioProcessor& audioProcessor;

    juce::Slider waveTypeSlider;
    juce::Slider gainSlider;
    juce::Slider noiseLevelSlider;
    juce::Label  waveTypeLabel;
    juce::Label  gainLabel;
    juce::Label  noiseLabel;

    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent midiKeyboardComponent;

  
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
}
