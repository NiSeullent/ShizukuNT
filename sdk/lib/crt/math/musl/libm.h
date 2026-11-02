/*
 * PROJECT:     LiberNT CRT library
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Internal libm.h subset used by the musl 1.2.3 sources taken from Wine 11.21 libs/musl
 * COPYRIGHT:   Copyright 2005-2020 Rich Felker, et al.
 *              Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#pragma once

#include "../libm.h"

#define hidden

typedef double matherr_t(int, const char *, double, double, double);

#define EXTRACT_WORDS(hi,lo,d)                    \
do {                                              \
  uint64_t __u = __DOUBLE_BITS(d);                \
  (hi) = __u >> 32;                               \
  (lo) = (uint32_t)__u;                           \
} while (0)

#define INSERT_WORDS(d,hi,lo)                     \
do {                                              \
  (d) = __DOUBLE_FROM_BITS(((uint64_t)(hi)<<32) | (uint32_t)(lo)); \
} while (0)

#define SET_HIGH_WORD(d,hi)                       \
  INSERT_WORDS(d, hi, (uint32_t)__DOUBLE_BITS(d))

hidden double __sin(double,double,int);
hidden double __cos(double,double);
hidden float  __sindf(double);
hidden float  __cosdf(double);

extern int __signgam;
hidden double __lgamma_r(double, int *);
hidden float __lgammaf_r(float, int *);
