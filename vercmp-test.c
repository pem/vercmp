/*
** pem 2021-11-14
**
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "vercmp.h"

typedef struct v_s
{
    char *v1, *v2;
    int cmp;
    int eno;
    bool eq, neq, lt, le, gt, ge;
} v_t;

static v_t Versions[] =
    {
     /*                        cmp errno   eq    neq     lt     le     gt     ge */
     { "0"       , "0"        ,  0,  0,  true, false, false,  true, false,  true },
     { "7"       , "7"        ,  0,  0,  true, false, false,  true, false,  true },
     { "12345"   , "12345"    ,  0,  0,  true, false, false,  true, false,  true },
     { "0.17.99" , "0.17.100" , -1,  0, false,  true,  true,  true, false, false },
     { "0.17.100", "0.17.99"  ,  1,  0, false,  true, false, false,  true,  true },
     { "1.2"     , "1.2"      ,  0,  0,  true, false, false,  true, false,  true },
     { "1.2"     , "1.3"      , -1,  0, false,  true,  true,  true, false, false },
     { "1.3"     , "1.2"      ,  1,  0, false,  true, false, false,  true,  true },
     { "1.2"     , "10.2"     , -1,  0, false,  true,  true,  true, false, false },
     { "10.2"    , "1.2"      ,  1,  0, false,  true, false, false,  true,  true },
     { "2.1.0"   , "2.1.3"    , -1,  0, false,  true,  true,  true, false, false },
     { "2.1.3"   , "2.1.0"    ,  1,  0, false,  true, false, false,  true,  true },
     { "3.1"     , "3.1.0"    ,  0,  0,  true, false, false,  true, false,  true },
     { "3.1.0"   , "3.1"      ,  0,  0,  true, false, false,  true, false,  true },
     { "3.1"     , "3.1.0.0"  ,  0,  0,  true, false, false,  true, false,  true },
     { "3.1.0.0" , "3.1"      ,  0,  0,  true, false, false,  true, false,  true },
     { "4"       , "4.0.0.0"  ,  0,  0,  true, false, false,  true, false,  true },
     { "4.0.0.0" , "4"        ,  0,  0,  true, false, false,  true, false,  true },
     { "4"       , "4.0.0.1"  , -1,  0, false,  true,  true,  true, false, false },
     { "4.0.0.1" , "4"        ,  1,  0, false,  true, false, false,  true,  true },
     { "5.0"     , "5.0.0"    ,  0,  0,  true, false, false,  true, false,  true },
     { "5.0.0"   , "5.0"      ,  0,  0,  true, false, false,  true, false,  true },
     { "5.0"     , "5.0.1"    , -1,  0, false,  true,  true,  true, false, false },
     { "5.0.1"   , "5.0"      ,  1,  0, false,  true, false, false,  true,  true },
     /* We accept a trailing dot, mostly because treating it as an error is tricky
        and would make the error detection code even more awkward than it already is. */
     { "1."      , "1"        ,  0,  0,  true, false, false,  true, false,  true },
     { "2"       , "2."       ,  0,  0,  true, false, false,  true, false,  true },
     { "3."      , "3."       ,  0,  0,  true, false, false,  true, false,  true },
     { "9.2"     , "9.2."     ,  0,  0,  true, false, false,  true, false,  true },
     { "9.2."    , "9.2"      ,  0,  0,  true, false, false,  true, false,  true },
     /* Error cases */
     { ""        , ""         ,  0, EINVAL },
     { ""        , "0"        ,  0, EINVAL },
     { "0"       , ""         ,  0, EINVAL },
     { ""        , "6"        ,  0, EINVAL },
     { "6"       , ""         ,  0, EINVAL },
     { "7x"      , "7"        ,  0, EINVAL },
     { "7"       , "7x"       ,  0, EINVAL },
     { ".8"      , "8"        ,  0, EINVAL },
     { "9..2"    , "9.2"      ,  0, EINVAL },
     { "9.2.."   , "9.2"      ,  0, EINVAL },
     { "9.2"     , "9.2.."    ,  0, EINVAL },
     { "5.7"     , "5.7x"     ,  0, EINVAL },
     { "5.7x"    , "5.7"      ,  0, EINVAL },
     { "5a.7b"   , "5c.7d"    ,  0, EINVAL },
     { "5x.y7z"  , "q5q.qq7"  ,  0, EINVAL },
     { "foo"     , "bar"      ,  0, EINVAL },
     { "-3.3"    , "3.3"      ,  0, EINVAL },
     { "+3.3"    , "3.3"      ,  0, EINVAL },
     { "184467440737095516151", "18446744073709551615", 0, ERANGE },
     { NULL      , NULL       ,  0, EINVAL } /* Sentinel, and we also test this case */
    };

static void
test(const char *v1, const char *v2, v_t *expected)
{
    char s1[32], s2[32];

    if (v1 == NULL)
        snprintf(s1, sizeof(s1), "NULL");
    else
        snprintf(s1, sizeof(s1), "\"%s\"", v1);
    if (v2 == NULL)
        snprintf(s2, sizeof(s2), "NULL");
    else
        snprintf(s2, sizeof(s2), "\"%s\"", v2);
    errno = 0;
    int c = vercmp(v1, v2);
    if (expected->cmp == c)
        printf("%-12s - %-12s : %2d", s1, s2, c);
    else
    {
        printf("%-12s - %-12s : returned %d, expected %d\n", s1, s2, c, expected->cmp);
        return;
    }
    if (expected->eno != errno)
        printf("  errno %d, expected %d\n", errno, expected->eno);
    else if (expected->eno != 0)
        printf("  errno=%d (ok)\n", errno);
    else
        printf(" %3s %3s %3s %3s %3s %3s\n",
               (vercmp_eq(v1, v2) == expected->eq ? "ok" : "!!"),
               (vercmp_neq(v1, v2) == expected->neq ? "ok" : "!!"),
               (vercmp_lt(v1, v2) == expected->lt ? "ok" : "!!"),
               (vercmp_le(v1, v2) == expected->le ? "ok" : "!!"),
               (vercmp_gt(v1, v2) == expected->gt ? "ok" : "!!"),
               (vercmp_ge(v1, v2) == expected->ge ? "ok" : "!!"));
}

int
main(int argc, char **argv)
{
    if (argc == 3)
    {
        errno = 0;
        int c = vercmp(argv[1], argv[2]);
        printf("%d, errno=%d\n", c, errno);
        exit(0);
    }
    /*       0         1         2         3         4         5
             012345678901234567890123456789012345678901234567890123456789
            "0"          - "0"          :  0   +   +   +   +   +   +
    */
    printf("                             cmp  eq neq  lt  le  gt  ge\n");
    v_t *v;
    for (v = Versions ; v->v1 != NULL ; v++)
        test(v->v1, v->v2, v);
    test(v->v1, v->v2, v);      /* The NULL, NULL case */
    exit(0);
}
