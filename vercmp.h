/*
** pem 2021-11-14
**
*/

#pragma once

#include <stdbool.h>

int
vercmp(const char *v1, const char *v2);

bool
vercmp_eq(const char *v1, const char *v2);

bool
vercmp_neq(const char *v1, const char *v2);

bool
vercmp_lt(const char *v1, const char *v2);

bool
vercmp_le(const char *v1, const char *v2);

bool
vercmp_gt(const char *v1, const char *v2);

bool
vercmp_ge(const char *v1, const char *v2);
