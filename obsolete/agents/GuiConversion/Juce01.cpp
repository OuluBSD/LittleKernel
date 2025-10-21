#ifdef HAVE_JUCE
#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;

struct CenteredLabel : public Component {
    Label label;
    CenteredLabel() {
        addAndMakeVisible(label);
        label.setText("Hello world!", dontSendNotification);
        label.setJustificationType(Justification::centred);
    }
    void resized() override { label.setBounds(getLocalBounds()); }
};

struct HelloWindow : public DocumentWindow {
    HelloWindow() : DocumentWindow("Hello World program",
                                   Colours::lightgrey,
                                   DocumentWindow::closeButton) {
        setUsingNativeTitleBar(true);
        setResizable(false, false);
        setContentOwned(new CenteredLabel(), true);
        centreWithSize(320, 240);
        setVisible(true);
    }
    void closeButtonPressed() override {
        if (auto* mm = MessageManager::getInstance())
            mm->stopDispatchLoop();
    }
};

void Juce01(int test_num) {
    if (test_num == 0 || test_num < 0) {
        ScopedJuceInitialiser_GUI gui;
        HelloWindow win;
        if (auto* mm = MessageManager::getInstance())
            mm->runDispatchLoop();
    }

    // 2. Simple events (Button & click -> popup)
    if (test_num == 1 || test_num < 0) {
        struct ButtonContent : public Component {
            TextButton btn { "Hello world!" };
            ButtonContent() {
                addAndMakeVisible(btn);
                btn.onClick = []{
                    AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon,
                                                     String(),
                                                     "Popup message");
                };
            }
            void resized() override { btn.setBounds(30, 30, 100, 30); }
        };

        struct ButtonWindow : public DocumentWindow {
            ButtonWindow() : DocumentWindow("Button program",
                                           Colours::lightgrey,
                                           DocumentWindow::closeButton) {
                setUsingNativeTitleBar(true);
                setResizable(false, false);
                setContentOwned(new ButtonContent(), true);
                centreWithSize(320, 240);
                setVisible(true);
            }
            void closeButtonPressed() override {
                if (auto* mm = MessageManager::getInstance())
                    mm->stopDispatchLoop();
            }
        };

        ScopedJuceInitialiser_GUI gui;
        ButtonWindow win;
        if (auto* mm = MessageManager::getInstance())
            mm->runDispatchLoop();
    }
}
#else
void Juce01(int) {}
#endif
