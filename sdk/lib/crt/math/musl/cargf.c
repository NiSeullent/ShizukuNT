#include "complex_impl.h"

float cargf(_Fcomplex z)
{
	float x = crealf(z), y = cimagf(z);

	if (isnan(x) || isnan(y))
		return x + y;
	return atan2f(y, x);
}
