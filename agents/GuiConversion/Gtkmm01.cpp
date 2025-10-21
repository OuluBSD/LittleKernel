#ifdef HAVE_GTKMM
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/messagedialog.h>

void Gtkmm01(int test_num) {
    int argc = 1;
    char arg0[] = "gtkmmapp";
    char* argv[] = { arg0 };
    
    if (test_num == 0 || test_num < 0) {
        auto app = Gtk::Application::create(argc, argv, "org.example.helloworld");
        Gtk::Window window;
        window.set_title("Hello World program");
        window.set_default_size(320, 240);
        Gtk::Label label("Hello world!");
        label.set_hexpand(true);
        label.set_vexpand(true);
#if GTKMM_CHECK_VERSION(3,0,0)
        label.set_justify(Gtk::JUSTIFY_CENTER);
#endif
        window.add(label);
        window.show_all();
        app->run(window);
    }

    // 2. Simple events (Button & click -> popup)
    if (test_num == 1 || test_num < 0) {
        auto app = Gtk::Application::create(argc, argv, "org.example.button");
        Gtk::Window window;
        window.set_title("Button program");
        window.set_default_size(320, 240);

        Gtk::Button button("Hello world!");
        button.signal_clicked().connect([&]() {
            Gtk::MessageDialog dlg(window, "Popup message", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
            dlg.run();
        });

        window.add(button);
        window.show_all();
        app->run(window);
    }
}
#else
void Gtkmm01(int) {}
#endif
