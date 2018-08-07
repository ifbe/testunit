#include<stdio.h>
double triangle_girth(
        double x1,double y1,
        double x2,double y2,
        double x3,double y3);
double triangle_area(
        double x1,double y1,
        double x2,double y2,
        double x3,double y3);
void triangle_incircle(
        double x1,double y1,
        double x2,double y2,
        double x3,double y3,
        double* x,double* y,double* r);
void triangle_circumcircle(
        double x1,double y1,
        double x2,double y2,
        double x3,double y3,
        double* x,double* y,double* r);
void main()
{
	double x,y,r;
	double girth = triangle_girth(
		3.7, 1.9,
		-1.6, -3.3,
		9.8, 2.4
	);
	double area = triangle_area(
		3.7, 1.9,
		-1.6, -3.3,
		9.8, 2.4
	);
	printf("%lf,%lf\n", girth, area);

	triangle_incircle(
		3.7, 1.9,
		-1.6, -3.3,
		9.8, 2.4,
		&x,&y,&r
	);
	printf("%lf,%lf,%lf\n",x,y,r);

	triangle_circumcircle(
		3.7, 1.9,
		-1.6, -3.3,
		9.8, 2.4,
		&x,&y,&r
	);
	printf("%lf,%lf,%lf\n",x,y,r);
}
