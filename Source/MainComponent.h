#pragma once

#include <JuceHeader.h>
#include <fstream>
#include "/home/sten/AudioProgramming/Slang/slang-lib.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, public juce::Button::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    SlangInterpreter* si;
    SlangBufferCore* sbc;
    juce::TextEditor textEditor;
    juce::Label label;
    juce::TextButton applyButton;
    juce::TextButton saveFileButton;
    juce::TextButton loadFileButton;
    juce::TextButton saveToCloudButton;
    juce::TextButton loadFromCloudButton;
    void buttonClicked (juce::Button* button) override;
    void applySlangScript(char* script);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
