#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QMainWindow wnd;
	wnd.setWindowTitle("test title");
	wnd.resize(300, 140);

	QLabel label("test label", &wnd);
	label.setGeometry(100, 50, 160, 30);

	wnd.show();

	return app.exec();
}
