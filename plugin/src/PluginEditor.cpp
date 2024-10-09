/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "ThreeDSynth/PluginProcessor.h"
#include "ThreeDSynth/PluginEditor.h"

namespace {
auto streamToVector (juce::InputStream& stream) {
    using namespace juce;

    std::vector<std::byte> result ((size_t) stream.getTotalLength());
    stream.setPosition (0);
    [[maybe_unused]] const auto bytesRead = stream.read (result.data(), result.size());
    jassert (bytesRead == (ssize_t) result.size());
    return result;
}

const char* getMimeForExtension (const juce::String& extension)
{
    using namespace juce;
    static const std::unordered_map<String, const char*> mimeMap =
    {
        { { "htm"   },  "text/html"                },
        { { "html"  },  "text/html"                },
        { { "txt"   },  "text/plain"               },
        { { "jpg"   },  "image/jpeg"               },
        { { "jpeg"  },  "image/jpeg"               },
        { { "svg"   },  "image/svg+xml"            },
        { { "ico"   },  "image/vnd.microsoft.icon" },
        { { "json"  },  "application/json"         },
        { { "png"   },  "image/png"                },
        { { "css"   },  "text/css"                 },
        { { "map"   },  "application/json"         },
        { { "js"    },  "text/javascript"          },
        { { "woff2" },  "font/woff2"               }
    };

    if (const auto it = mimeMap.find (extension.toLowerCase()); it != mimeMap.end())
        return it->second;

    jassertfalse;
    return "";
}
}

//==============================================================================

namespace ThreeDSynth {

NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    webViewGui{juce::WebBrowserComponent::Options{}
    .withResourceProvider([this](const auto& url){
        return getResource(url);})
        .withNativeIntegrationEnabled()
        .withUserScript(R"(console.log("C++ Backend here: This is run before any other loading happens.");)")
        .withNativeFunction(
            juce::Identifier{"nativeFunction"},
            [this](const juce::Array<juce::var>& args,
            juce::WebBrowserComponent::NativeFunctionCompletion completion){
               nativeFunction(args, std::move(completion)); 
            }
        )} 
{
    juce::ignoreUnused (audioProcessor);


    waveTypeSlider.setRange(0, 2, 1);
    waveTypeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    waveTypeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 0, 0);
    waveTypeSlider.addListener(this);
    addAndMakeVisible(waveTypeSlider);
    
    gainSlider.setRange(-30.0f, 0.0f, 0.01f);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gainSlider.addListener(this);
    addAndMakeVisible(gainSlider);
    gainSlider.setValue(-10.0f);

//  Noise Level
    noiseLevelSlider.setRange(0.0f, 0.9f, 0.01f);
    noiseLevelSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    noiseLevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    noiseLevelSlider.addListener(this);
    addAndMakeVisible(noiseLevelSlider);
    
//  WaveType Label
    waveTypeLabel.setText("Wave Type", juce::dontSendNotification);
    waveTypeLabel.setJustificationType(juce::Justification::centred);
    waveTypeLabel.attachToComponent(&waveTypeSlider, false);
    addAndMakeVisible(waveTypeLabel);
    
//  Gain Label
    gainLabel.setText("Osc Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainSlider, false);
    addAndMakeVisible(gainLabel);

//  Noise Label
    noiseLabel.setText("Noise Level", juce::dontSendNotification);
    noiseLabel.setJustificationType(juce::Justification::centred);
    noiseLabel.attachToComponent(&noiseLevelSlider, false);
    addAndMakeVisible(noiseLabel);

//WebViewGUI

    webViewGui.goToURL("http://localhost:5173/");
    addAndMakeVisible(webViewGui);

//Window
setResizable(true, true);         
setSize (800, 600);

}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
    waveTypeSlider.removeListener(this);
    gainSlider.removeListener(this);
    noiseLevelSlider.removeListener(this);
}

//==============================================================================

void NewProjectAudioProcessorEditor::resized()
{
    int width = getWidth();
    int height = getHeight();

    auto bounds = getLocalBounds();

    // Set the WebView GUI to take up the top 2/3 of the height
    webViewGui.setBounds(bounds.removeFromTop(height * 2 / 3));

    // The remaining height for the bottom section where sliders will be placed
    int bottomSectionHeight = height - webViewGui.getHeight();

    // Calculate the width for each slider, with padding to center them
    int padding = 20;  // Add some padding between sliders
    int sliderWidth = (width - 2 * padding) / 3;  // Adjust for padding
    int sliderHeight = bottomSectionHeight * 0.6;  // Use 60% of the bottom section height for sliders

    // Calculate Y position for centering the sliders within the bottom section
    int sliderY = webViewGui.getBottom() + (bottomSectionHeight - sliderHeight) / 2;

    // Set the bounds for each slider, including the label spacing
    waveTypeSlider.setBounds(padding, sliderY, sliderWidth, sliderHeight);
    gainSlider.setBounds(padding + sliderWidth, sliderY, sliderWidth, sliderHeight);
    noiseLevelSlider.setBounds(padding + 2 * sliderWidth, sliderY, sliderWidth, sliderHeight);

    // Ensure labels are attached and visible (assuming labels are attached to sliders)
    waveTypeLabel.setBounds(waveTypeSlider.getX(), waveTypeSlider.getBottom(), sliderWidth, 20);  // Adjust height for labels
    gainLabel.setBounds(gainSlider.getX(), gainSlider.getBottom(), sliderWidth, 20);
    noiseLabel.setBounds(noiseLevelSlider.getX(), noiseLevelSlider.getBottom(), sliderWidth, 20);
}


void NewProjectAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &waveTypeSlider)
    {
        int waveType = static_cast<int>(slider->getValue());
        audioProcessor.setWaveType(waveType);
    }
    
    
    if (slider == &gainSlider)
        {
            double gainValueInDb = gainSlider.getValue();
            double gainValueLinear = pow(10.0, gainValueInDb / 20.0);  // Convert dB to linear
            
            audioProcessor.setGain(gainValueLinear);
            
            std::cout << "Gain (dB): " << gainValueInDb << ", Gain (Linear): " << gainValueLinear << std::endl;
        }
        
        
    if (slider == &noiseLevelSlider)
    {
        double noiseLevelValue = noiseLevelSlider.getValue();
        audioProcessor.setNoiseLevel(noiseLevelValue);
        
        std:: cout << "noise level: " << noiseLevelValue << std::endl;
    }  
  }

    auto NewProjectAudioProcessorEditor::getResource (const juce::String& url) -> std::optional<Resource> {

    std::cout << url << std::endl;

    static const auto resourceFileRoot = juce::File{R"(/Users/ejourgensen/Desktop/Erik/Programming/Audio Dev/Three_D_Synth/plugin/UI/public)"};

    const auto resourceToRetrieve = url == "/" ? "index.html" : url.fromFirstOccurrenceOf ("/", false, false);

    const auto resource = resourceFileRoot.getChildFile(resourceToRetrieve).createInputStream();

    if (resource){
        const auto extension = resourceToRetrieve.fromLastOccurrenceOf(".", false, false);
        return Resource{streamToVector(*resource), getMimeForExtension(extension)};
    }

    return std::nullopt;
}

void NewProjectAudioProcessorEditor::nativeFunction(const juce::Array<juce::var>& args,
                                                     juce::WebBrowserComponent::NativeFunctionCompletion completion)
{
    // Ensure there's a valid argument for frequency
    if (args.size() > 0 && args[0].isDouble())
    {
        double uiGain = args[0];  

        audioProcessor.setGain(static_cast<float>(uiGain));

        gainSlider.setValue(uiGain, juce::dontSendNotification);

        completion("Parameter updated to " + juce::String(uiGain));
    }
    else
    {
        completion("Invalid gain value");
    }
}
}