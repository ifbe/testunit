#include<math.h>
#define u64 unsigned long long
#define squareroot sqrt
double squareroot(double);




double triangle_girth(		//周长
	double x1,double y1,
	double x2,double y2,
	double x3,double y3)
{
	double a = squareroot( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
	double b = squareroot( (x1-x3)*(x1-x3) + (y1-y3)*(y1-y3) );
	double c = squareroot( (x2-x3)*(x2-x3) + (y2-y3)*(y2-y3) );
	return a+b+c;
}
double triangle_area(		//面积
	double x1,double y1,
	double x2,double y2,
	double x3,double y3)
{
	double a = (x1*y3 + x2*y1 + x3*y2 - x1*y2 - x2*y3 - x3*y1)/2;
	if(a<0)a = -a;
	return a;
}
double triangle_maxang(		//最大角
	double x1,double y1,
	double x2,double y2,
	double x3,double y3)
{
	return 3.141592653/2;
}




void triangle_incirclecenter(		//内心
	double x1,double y1,
	double x2,double y2,
	double x3,double y3,
	double* x,double* y)
{
	double a = squareroot( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
	double b = squareroot( (x1-x3)*(x1-x3) + (y1-y3)*(y1-y3) );
	double c = squareroot( (x2-x3)*(x2-x3) + (y2-y3)*(y2-y3) );
	double p = (a+b+c)/2;
	double s = squareroot( p*(p-a)*(p-b)*(p-c) );
	*x = (a*x1 + b*x2 + c*x3)/(a+b+c);
	*y = (a*y1 + b*y2 + c*y3)/(a+b+c);
}
void triangle_circumcirclecenter(	//外心
	double x1,double y1,
	double x2,double y2,
	double x3,double y3,
	double* x,double* y)
{
	double t1 = x1*x1 + y1*y1;
	double t2 = x2*x2 + y2*y2;
	double t3 = x3*x3 + y3*y3;
	double temp = x1*y2 + x2*y3 + x3*y1 - x1*y3 - x2*y1 - x3*y2;

	*x = (t2*y3 + t1*y2 + t3*y1 - t2*y1 - t3*y2 - t1*y3)/temp/2;
	*y = (t3*x2 + t2*x1 + t1*x3 - t1*x2 - t2*x3 - t3*x1)/temp/2;
}
void triangle_gravitycenter(		//重心
	double x1,double y1,
	double x2,double y2,
	double x3,double y3,
	double* x,double* y)
{
	*x = (x1+x2+x3)/3;
	*y = (y1+y2+y3)/3;
}
void triangle_orthocenter(		//垂心
	double x1,double y1,
	double x2,double y2,
	double x3,double y3,
	double* x,double* y)
{
	*x = (x1+x2+x3)/3;
	*y = (y1+y2+y3)/3;
}
void triangle_escenter(		//旁心
	double x1,double y1,
	double x2,double y2,
	double x3,double y3,
	double* x,double* y)
{
	*x = (x1+x2+x3)/3;
	*y = (y1+y2+y3)/3;
}




void triangle_incircle(		//内切圆
	double x1,double y1,
	double x2,double y2,
	double x3,double y3,
	double* x,double* y,double* r)
{
	double a = squareroot( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
	double b = squareroot( (x1-x3)*(x1-x3) + (y1-y3)*(y1-y3) );
	double c = squareroot( (x2-x3)*(x2-x3) + (y2-y3)*(y2-y3) );
	double p = (a+b+c)/2;
	double s = squareroot( p*(p-a)*(p-b)*(p-c) );
	*x = (a*x1 + b*x2 + c*x3)/(a+b+c);
	*y = (a*y1 + b*y2 + c*y3)/(a+b+c);
	*r = 2*s/(a+b+c);
}
void triangle_circumcircle(		//外接圆
	double x1,double y1,
	double x2,double y2,
	double x3,double y3,
	double* x,double* y,double* r)
{
	double t1 = x1*x1 + y1*y1;
	double t2 = x2*x2 + y2*y2;
	double t3 = x3*x3 + y3*y3;
	double temp = x1*y2 + x2*y3 + x3*y1 - x1*y3 - x2*y1 - x3*y2;

	*x = (t2*y3 + t1*y2 + t3*y1 - t2*y1 - t3*y2 - t1*y3)/temp/2;
	*y = (t3*x2 + t2*x1 + t1*x3 - t1*x2 - t2*x3 - t3*x1)/temp/2;
	*r = squareroot( (x[0]-x1)*(x[0]-x1) + (y[0]-y1)*(y[0]-y1) );
}
