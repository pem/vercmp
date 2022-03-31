/*
** pem 2021-11-14
**
*/

#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include "vercmp.h"

int
verncmp(const char *v1, const char *v2, size_t n)
{
    if (v1 == NULL || !isdigit(*v1) || v2 == NULL || !isdigit(*v2))
        goto einval;

    while (n-- && (*v1 != '\0' || *v2 != '\0'))
    {
        if ((*v1 != '\0' && !isdigit(*v1)) || (*v2 != '\0' && !isdigit(*v2)))
            goto einval;

        char *s1 = NULL, *s2 = NULL;
        unsigned long x1 = (*v1 == '\0' ? 0 : strtoul(v1, &s1, 10));
        if (errno != 0)
            return 0;
        unsigned long x2 = (*v2 == '\0' ? 0 : strtoul(v2, &s2, 10));
        if (errno != 0)
            return 0;

        if (s1 != NULL && *s1 != '\0' && *s1 != '.')
            goto einval;
        if (s2 != NULL && *s2 != '\0' && *s2 != '.')
            goto einval;

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

 einval:
    errno = EINVAL;
    return 0;
}

bool
verncmp_eq(const char *v1, const char *v2, size_t n)
{
    return (verncmp(v1, v2, n) == 0);
}

bool
verncmp_neq(const char *v1, const char *v2, size_t n)
{
    return (verncmp(v1, v2, n) != 0);
}

bool
verncmp_lt(const char *v1, const char *v2, size_t n)
{
    return (verncmp(v1, v2, n) == -1);
}

bool
verncmp_le(const char *v1, const char *v2, size_t n)
{
    return (verncmp(v1, v2, n) != 1);
}

bool
verncmp_gt(const char *v1, const char *v2, size_t n)
{
    return (verncmp(v1, v2, n) == 1);
}

bool
verncmp_ge(const char *v1, const char *v2, size_t n)
{
    return (verncmp(v1, v2, n) != -1);
}


int
vercmp(const char *v1, const char *v2)
{
    return verncmp(v1, v2, SIZE_MAX);
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
