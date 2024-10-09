juce_add_plugin(ThreeDSynth
    COMPANY_NAME ErikJourgensen
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE JOUR
    PLUGIN_CODE TDSN 
    FORMATS AU VST3 Standalone
)

        target_sources(ThreeDSynth
            PRIVATE 
            src/PluginEditor.cpp 
            src/PluginProcessor.cpp 
            src/DSP/noiseGenerator.h
            src/DSP/synthSound.h
            src/DSP/synthVoice.h 
            src/DSP/waveGenerator.h
        )

        target_include_directories(ThreeDSynth PRIVATE include src)

        target_link_libraries(ThreeDSynth
        PRIVATE 
        juce::juce_audio_utils
        juce::juce_gui_extra
        PUBLIC 
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        )

        target_compile_definitions(ThreeDSynth 
        PUBLIC 
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        )
        

#WHEN YOU ADD THREE.JS---------------------

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../libs/juce/modules/juce_gui_extra/native/javascript" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/UI/public/js/juce/")
