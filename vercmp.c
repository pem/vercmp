/*
** pem 2021-11-14
**
*/

#include <stdlib.h>
#include <errno.h>
#include "vercmp.h"

int
vercmp(const char *v1, const char *v2)
{
    if (v1 == NULL || *v1 == '\0' || v2 == NULL || *v2 == '\0')
    {
        errno = EINVAL;
        return 0;
    }
    while (*v1 != '\0' || *v2 != '\0')
    {
        char *s1 = NULL, *s2 = NULL;
        unsigned long x1 = (*v1 == '\0' ? 0 : strtoul(v1, &s1, 10));
        if (errno != 0)
            return 0;
        unsigned long x2 = (*v2 == '\0' ? 0 : strtoul(v2, &s2, 10));
        if (errno != 0)
            return 0;

        if (s1 != NULL && *s1 != '\0' && *s1 != '.')
        {
            errno = EINVAL;
            return 0;
        }
        if (s2 != NULL && *s2 != '\0' && *s2 != '.')
        {
            errno = EINVAL;
            return 0;
        }
        if (x1 < x2)
            return -1;
        if (x1 > x2)
            return 1;
        if (s1 != NULL)
            v1 = (*s1 == '\0' ? s1 : s1+1);
        if (s2 != NULL)
            v2 = (*s2 == '\0' ? s2 : s2+1);
    }
    return 0;
}

bool
vercmp_eq(const char *v1, const char *v2)
{
    return (vercmp(v1, v2) == 0);
}

bool
vercmp_neq(const char *v1, const char *v2)
{
    return (vercmp(v1, v2) != 0);
}

bool
vercmp_lt(const char *v1, const char *v2)
{
    return (vercmp(v1, v2) == -1);
}

bool
vercmp_le(const char *v1, const char *v2)
{
    return (vercmp(v1, v2) != 1);
}

bool
vercmp_gt(const char *v1, const char *v2)
{
    return (vercmp(v1, v2) == 1);
}

bool
vercmp_ge(const char *v1, const char *v2)
{
    return (vercmp(v1, v2) != -1);
}