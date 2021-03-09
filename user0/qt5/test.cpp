#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLCDNumber>
class MyMainWindow : public QMainWindow
{
public:
    MyMainWindow();
};
MyMainWindow::MyMainWindow()
{
	setWindowTitle("test title");
	resize(512, 256);

	QLabel* label = new QLabel("test label", this);
	label->setGeometry(128, 0, 256, 64);

	QLCDNumber* lcd = new QLCDNumber(2, this);
	lcd->setGeometry(128, 64, 256, 128);

	QSlider *slider = new QSlider(Qt::Horizontal, this);
	slider->setGeometry(128, 192, 256, 64);
        slider->setRange(0, 99);
        slider->setValue(0);

	connect(slider,SIGNAL(valueChanged(int)),lcd,SLOT(display(int)));
}
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	MyMainWindow wnd;
	wnd.show();

	return app.exec();
}
