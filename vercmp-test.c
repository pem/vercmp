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
    size_t n;
    int cmp;
    int eno;
    bool eq, neq, lt, le, gt, ge;
} v_t;

static v_t Versions[] =
    {
     /*                                  cmp errno   eq    neq     lt     le     gt     ge */
     { "0",        "0",         SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "7",        "7",         SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "12345",    "12345",     SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "0.17.99",  "0.17.100",  SIZE_MAX, -1,  0, false,  true,  true,  true, false, false },
     { "0.17.100", "0.17.99",   SIZE_MAX,  1,  0, false,  true, false, false,  true,  true },
     { "1.2",      "1.2",       SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "1.2",      "1.3",       SIZE_MAX, -1,  0, false,  true,  true,  true, false, false },
     { "1.3",      "1.2",       SIZE_MAX,  1,  0, false,  true, false, false,  true,  true },
     { "1.2",      "10.2",      SIZE_MAX, -1,  0, false,  true,  true,  true, false, false },
     { "10.2",     "1.2",       SIZE_MAX,  1,  0, false,  true, false, false,  true,  true },
     { "2.1.0",    "2.1.3",     SIZE_MAX, -1,  0, false,  true,  true,  true, false, false },
     { "2.1.3",    "2.1.0",     SIZE_MAX,  1,  0, false,  true, false, false,  true,  true },
     { "3.1",      "3.1.0",     SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "3.1.0",    "3.1",       SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "3.1",      "3.1.0.0",   SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "3.1.0.0",  "3.1",       SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "4",        "4.0.0.0",   SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "4.0.0.0",  "4",         SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "4",        "4.0.0.1",   SIZE_MAX, -1,  0, false,  true,  true,  true, false, false },
     { "4.0.0.1",  "4",         SIZE_MAX,  1,  0, false,  true, false, false,  true,  true },
     { "5.0",      "5.0.0",     SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "5.0.0",    "5.0",       SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "5.0",      "5.0.1",     SIZE_MAX, -1,  0, false,  true,  true,  true, false, false },
     { "5.0.1",    "5.0",       SIZE_MAX,  1,  0, false,  true, false, false,  true,  true },
     /* We accept a trailing dot, mostly because treating it as an error is tricky
        and would make the error detection code even more awkward than it already is. */
     { "1.",       "1",         SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "2",        "2.",        SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "3.",       "3.",        SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "9.2",      "9.2.",      SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     { "9.2.",     "9.2",       SIZE_MAX,  0,  0,  true, false, false,  true, false,  true },
     /* Limited to 'n' fields */
     { "3",        "7",         0,         0,  0,  true, false, false,  true, false,  true },
     { "3",        "7",         1,        -1,  0, false,  true,  true,  true, false, false },
     { "7",        "3",         1,         1,  0, false,  true, false, false,  true,  true },
     { "2.1.19",   "2.1.32",    2,         0,  0,  true, false, false,  true, false,  true },
     { "2.1.19",   "2.1.32",    3,        -1,  0, false,  true,  true,  true, false, false },
     { "9.7",      "9.7.102",   2,         0,  0,  true, false, false,  true, false,  true },
     { "9.7.102",  "9.7",       2,         0,  0,  true, false, false,  true, false,  true },
     { "6",        "6.2.17",    1,         0,  0,  true, false, false,  true, false,  true },
     { "6.2.17",   "6",         1,         0,  0,  true, false, false,  true, false,  true },
     { "6",        "6.2.17",    2,        -1,  0, false,  true,  true,  true, false, false },
     { "6.2.17",   "6",         2,         1,  0, false,  true, false, false,  true,  true },
     /* Note that it will not parse beyond the limit, so this will not return an error */
     { "9.8.foo",  "9.8.bar",   2,         0,  0,  true, false, false,  true, false,  true },
     /* Error cases */
     { "",         "",          SIZE_MAX,  0, EINVAL },
     { "",         "0",         SIZE_MAX,  0, EINVAL },
     { "0",        "",          SIZE_MAX,  0, EINVAL },
     { "",         "6",         SIZE_MAX,  0, EINVAL },
     { "6",        "",          SIZE_MAX,  0, EINVAL },
     { "7x",       "7",         SIZE_MAX,  0, EINVAL },
     { "7",        "7x",        SIZE_MAX,  0, EINVAL },
     { ".8",       "8",         SIZE_MAX,  0, EINVAL },
     { "9..2",     "9.2",       SIZE_MAX,  0, EINVAL },
     { "9.2..",    "9.2",       SIZE_MAX,  0, EINVAL },
     { "9.2",      "9.2..",     SIZE_MAX,  0, EINVAL },
     { "5.7",      "5.7x",      SIZE_MAX,  0, EINVAL },
     { "5.7x",     "5.7",       SIZE_MAX,  0, EINVAL },
     { "5a.7b",    "5c.7d",     SIZE_MAX,  0, EINVAL },
     { "5x.y7z",   "q5q.qq7",   SIZE_MAX,  0, EINVAL },
     { "foo",      "bar",       SIZE_MAX,  0, EINVAL },
     { "-3.3",     "3.3",       SIZE_MAX,  0, EINVAL },
     { "+3.3",     "3.3",       SIZE_MAX,  0, EINVAL },
     { "184467440737095516151", "18446744073709551615", SIZE_MAX, 0, ERANGE },
     { NULL,       NULL,        SIZE_MAX,  0, EINVAL } /* Sentinel, and we also test this case */
    };

static void
test(v_t *testcase)
{
    char s1[32], s2[32], sn[8];
    const char *v1 = testcase->v1;
    const char *v2 = testcase->v2;
    size_t n = testcase->n;

    if (v1 == NULL)
        snprintf(s1, sizeof(s1), "NULL");
    else
        snprintf(s1, sizeof(s1), "\"%s\"", v1);
    if (v2 == NULL)
        snprintf(s2, sizeof(s2), "NULL");
    else
        snprintf(s2, sizeof(s2), "\"%s\"", v2);
    if (n < SIZE_MAX)
        snprintf(sn, sizeof(sn), "%lu", (unsigned long)n);
    else
        snprintf(sn, sizeof(sn), " ");
    errno = 0;
    int c = (n < SIZE_MAX ? verncmp(v1, v2, n) : vercmp(v1, v2));
    if (testcase->cmp == c)
        printf("%-12s - %-12s %s : %2d", s1, s2, sn, c);
    else
    {
        printf("%-12s - %-12s %s : returned %d, expected %d\n",
               s1, s2, sn, c, testcase->cmp);
        return;
    }
    if (testcase->eno != errno)
        printf("  errno %d, expected %d\n", errno, testcase->eno);
    else if (testcase->eno != 0)
        printf("  errno=%d (ok)\n", errno);
    else if (testcase->n < SIZE_MAX)
        printf(" %3s %3s %3s %3s %3s %3s\n",
               (verncmp_eq(v1, v2, n) == testcase->eq ? "ok" : "!!"),
               (verncmp_neq(v1, v2, n) == testcase->neq ? "ok" : "!!"),
               (verncmp_lt(v1, v2, n) == testcase->lt ? "ok" : "!!"),
               (verncmp_le(v1, v2, n) == testcase->le ? "ok" : "!!"),
               (verncmp_gt(v1, v2, n) == testcase->gt ? "ok" : "!!"),
               (verncmp_ge(v1, v2, n) == testcase->ge ? "ok" : "!!"));
    else
        printf(" %3s %3s %3s %3s %3s %3s\n",
               (vercmp_eq(v1, v2) == testcase->eq ? "ok" : "!!"),
               (vercmp_neq(v1, v2) == testcase->neq ? "ok" : "!!"),
               (vercmp_lt(v1, v2) == testcase->lt ? "ok" : "!!"),
               (vercmp_le(v1, v2) == testcase->le ? "ok" : "!!"),
               (vercmp_gt(v1, v2) == testcase->gt ? "ok" : "!!"),
               (vercmp_ge(v1, v2) == testcase->ge ? "ok" : "!!"));
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
            "0"          - "0"         n :  0  ok  ok  ok  ok  ok  ok
    */
    printf("                               cmp  eq neq  lt  le  gt  ge\n");
    v_t *v;
    for (v = Versions ; v->v1 != NULL ; v++)
        test(v);
    test(v);                    /* The NULL, NULL case */
    exit(0);
}
