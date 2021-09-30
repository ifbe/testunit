#include <iostream>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSlider>
#include <QtWidgets/QPushButton>
#include <QtWebEngineWidgets/QWebEngineView>




class MyMainWindow : public QMainWindow
{
public:
    MyMainWindow();
};
MyMainWindow::MyMainWindow()
{
	setWindowTitle("test title");
	resize(1024, 512);

	auto web = new QWebEngineView(this);
	web->setGeometry(0, 32, 1024, 512-32);
	web->setUrl(QUrl("http://www.baidu.com"));

	auto line = new QLineEdit("test line", this);
	line->setGeometry(256, 0, 512, 32);

	auto btnback = new QPushButton("back", this);
	btnback->setGeometry(0, 0, 256, 32);

	auto btnnext = new QPushButton("next", this);
	btnnext->setGeometry(768, 0, 256, 32);
/*
	auto lcd = new QLCDNumber(2, this);
	lcd->setGeometry(0, 512-64, 256, 64);

	auto slider = new QSlider(Qt::Horizontal, this);
	slider->setGeometry(256, 512-64, 256, 64);
        slider->setRange(0, 99);
        slider->setValue(0);

	connect(slider,SIGNAL(valueChanged(int)),lcd,SLOT(display(int)));
*/
	connect(line,SIGNAL(returnPressed(int)),web,SLOT(reload()));
	//connect(this,&QWebEngineView::urlChanged,this,&WebBrowser::OnUrlChanged);
	//connect(web,SIGNAL(UrlChanged),web,SLOT(onNewPage()));
}
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	qDebug() << "@graph_init\n";

	MyMainWindow wnd;
	wnd.show();

	return app.exec();
}
