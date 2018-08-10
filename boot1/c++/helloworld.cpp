#include <iostream>
using namespace std;


class helloworld
{
public:
	helloworld()
	{
		cout << "++++++" << endl;
	}
	~helloworld()
	{
		cout << "------" << endl;
	}
};
int main(int argc, char** argv)
{
	helloworld haha;
	cout << "@@@@@@" << endl;
}
