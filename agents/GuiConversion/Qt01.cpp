#ifdef HAVE_QT
#include <QApplication>
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QMessageBox>

void Qt01(int test_num) {
	int argc = 1;
	char arg0[] = "qtapp";
	char* argv[] = { arg0 };
	
	if (test_num == 0 || test_num < 0) {
		QApplication app(argc, argv);
		QLabel lbl("Hello world!");
		lbl.setAlignment(Qt::AlignCenter);
		lbl.setWindowTitle("Hello World program");
		lbl.resize(320, 240);
		lbl.show();
		app.exec();
	}

	// 2. Simple events (Button & click -> popup)
	if (test_num == 1 || test_num < 0) {
		QApplication app(argc, argv);
		QWidget window;
		window.setWindowTitle("Button program");
		window.resize(320, 240);
		QPushButton btn("Hello world!", &window);
		btn.setGeometry(30, 30, 100, 30);
		QObject::connect(&btn, &QPushButton::clicked, [&](){
			QMessageBox::information(&window, QString(), "Popup message");
		});
		window.show();
		app.exec();
	}
}
#else
void Qt01(int) {}
#endif
