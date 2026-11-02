/*
 * PROJECT:     LiberNT CRT library
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     exp() reporting range errors to a caller-supplied callback instead of _matherr
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include "libm.h"

static const double
exp_max_x = 0x1.62e42fefa39efp+9,
exp_zero_x = -0x1.74910d52d3051p+9,
exp_subnormal_x = -0x1.6232bdd7abcd2p+9;

double __cdecl __exp(double x, matherr_t matherr)
{
	double half;

	if (isnan(x))
		return matherr(_DOMAIN, "exp", x, 0, x);
	if (isinf(x))
		return x < 0 ? 0.0 : x;
	if (x > exp_max_x)
		return matherr(_OVERFLOW, "exp", x, 0, INFINITY);
	if (x < exp_zero_x)
		return matherr(_UNDERFLOW, "exp", x, 0, 0.0);
	if (x < exp_subnormal_x) {
		half = exp(0.5 * x);
		return matherr(_UNDERFLOW, "exp", x, 0, half * half);
	}
	return exp(x);
}
