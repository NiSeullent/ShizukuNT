#include "complex_impl.h"

double carg(_Dcomplex z)
{
	double x = creal(z), y = cimag(z);

	if (isnan(x) || isnan(y))
		return x + y;
	return atan2(y, x);
}
