#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    textEditor.setMultiLine(true);
    textEditor.setTabKeyUsedAsCharacter(true);
    textEditor.setReturnKeyStartsNewLine(true);
    textEditor.setFont(juce::Font(juce::FontOptions(24)));
    textEditor.setText("//<your slang script here>");
    addAndMakeVisible(&textEditor);

    applyButton.setButtonText("Apply");
    applyButton.addListener(this);
    addAndMakeVisible(&applyButton);

    saveFileButton.setButtonText("Save");
    saveFileButton.addListener(this);
    addAndMakeVisible(&saveFileButton);

    loadFileButton.setButtonText("Load");
    loadFileButton.addListener(this);
    addAndMakeVisible(&loadFileButton);

    saveToCloudButton.setButtonText("Save To Cloud");
    saveToCloudButton.addListener(this);
    addAndMakeVisible(&saveToCloudButton);

    loadFromCloudButton.setButtonText("Load From Cloud");
    loadFromCloudButton.addListener(this);
    addAndMakeVisible(&loadFromCloudButton);
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void MainComponent::buttonClicked (juce::Button* button) {
    if (button == &applyButton) {
        char *p = strdup((char*)textEditor.getText().toStdString().c_str());
        applySlangScript(p);
    }
    if (button == &saveFileButton) {
        auto* w = new juce::AlertWindow ("Filename", "Please enter a file name for this script", juce::AlertWindow::AlertIconType::NoIcon);

        w->addTextEditor ("inputField", "", "filename:");
        w->addButton ("OK", 1, juce::KeyPress (juce::KeyPress::returnKey));
        w->addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey));

        w->centreWithSize (400, 200);

        // Wichtig: mit Callback starten
        w->enterModalState (true,
            juce::ModalCallbackFunction::create([w](int result)
            {
                if (result != 0) // OK gedrückt?
                {
                    auto text = w->getTextEditorContents ("inputField");
                    DBG ("Eingabe: " << text);
                }
                delete w; // selbst freigeben
            }),
        true);
    }

}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{

}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* outL = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    auto* outR = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    if (sbc) {
        float* buf = renderBuffer(sbc);

        for(int sample = 0; sample < bufferToFill.numSamples; sample++) {
            if(sample < 512) {
                outL[sample] = buf[sample]*0.5;
                outR[sample] = buf[sample]*0.5;
            }
            else {
                outL[sample] = 0.f;
                outR[sample] = 0.f;
            }
        }
    }

}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::white);

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    textEditor.setBounds(10, 10, 780, 540);
    applyButton.setBounds(50, 557, 100, 35);
	saveFileButton.setBounds(200, 557, 100, 35);
	loadFileButton.setBounds(350, 557, 100, 35);
	saveToCloudButton.setBounds(500, 557, 100, 35);
    loadFromCloudButton.setBounds(650, 557, 100, 35);
}

void MainComponent::applySlangScript(char* script) {
    if (si != NULL) {
        free(si);
    }
    if (sbc != NULL) {
        free(sbc);
    }
    int tokensLength = 0;
    Token *tokens = tokenize(script, &tokensLength);
    std::cout << "Tokens length = " << tokensLength << std::endl;
    si = createSlangInterpreter(tokens, tokensLength);
    interpret(si);
    sbc = createBufferCore(si, 48000, 512);
    printAllVariables(si);
    printAllFunctions(si);
}
