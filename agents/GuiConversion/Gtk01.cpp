#ifdef HAVE_GTK
#include <gtk/gtk.h>

static void on_button_clicked(GtkWidget* /*button*/, gpointer user_data) {
    GtkWidget* parent = GTK_WIDGET(user_data);
    GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Popup message");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void Gtk01(int test_num) {
    int argc = 1;
    char arg0[] = "gtkapp";
    char* argv[] = { arg0 };
    if (test_num == 0 || test_num < 0) {
		gtk_init(&argc, &argv);
		GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(window), "Hello World program");
		gtk_window_set_default_size(GTK_WINDOW(window), 320, 240);
		GtkWidget *label = gtk_label_new("Hello world!");
		gtk_container_add(GTK_CONTAINER(window), label);
		g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        gtk_widget_show_all(window);
        gtk_main();
    }

    // 2. Simple events (Button & click -> popup)
    if (test_num == 1 || test_num < 0) {
        gtk_init(&argc, &argv);
        GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Button program");
        gtk_window_set_default_size(GTK_WINDOW(window), 320, 240);
        GtkWidget *button = gtk_button_new_with_label("Hello world!");
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), window);
        g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        gtk_container_add(GTK_CONTAINER(window), button);
        gtk_widget_show_all(window);
        gtk_main();
    }
}
#else
void Gtk01(int) {}
#endif
