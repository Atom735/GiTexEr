#include <iostream>
#include <iomanip> // std::setprecision
#include <cmath>

int main(int argc, char const *argv[])
{
	int f=1;
	double a,b,c;
	while(f) {
		std::cout << "BBe9uTe TPu 4ucJIa a,b,c:" << std::endl;
		std::cin >> a >> b >> c;
		std::cout << std::setprecision(9);
		double D = (b*b)-(4.0*a*c);
		if(D >= 0) {
			// Два действительных корня
			double x1 = ((b+sqrt(D))*0.5)/(-a);
			double x2 = ((b-sqrt(D))*0.5)/(-a);
			std::cout << "KopHu:" << std::endl;
			std::cout << x1 << std::endl;
			std::cout << x2 << std::endl;
		} else {
			// Два кмплексных корня
			double x = (b*0.5)/(-a);
			double y1 = (sqrt(-D)*0.5)/a;
			double y2 = (sqrt(-D)*0.5)/a;
			std::cout << "KopHu:" << std::endl;
			std::cout << x << " + " << y1 << "i" << std::endl;
			std::cout << x << " + " << y2 << "i" << std::endl;
		}

		std::cout << "BBe9uTe 4ucJIo oTJIu4Hoe OT HyJI9I 9JI9I noBTopa nporpaMMbI:" << std::endl;
		std::cin >> f;
	}
	return 0;
}