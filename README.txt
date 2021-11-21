A simple C library with functions for comparing standard version strings on
the form "x.y...", like "1.2", "2.3.4", "10.9.0.77", etc.

This code works for an arbitrary number of fields, and treats short form as
equal to forms expanded with zero fields, i.e. "1.2" is equal to "1.2.0" for
instance.

The base functions are vercmp() which returns -1, 0, or 1, for less than, equal
to, or greater than, and verncmp() which does the same, but only compares the
first 'n' fields. This might be useful when for instance the last field does
not indicate a functional difference but is a revision or build number.

For convenience, functions on the form vercmp_X() and verncmp_X() are provided
as well, where X is eq, neq, lt, le, gt, and ge; for "equal", "not equal",
"less than", and so on.

Only digits and dots are accepted, no letters. If a malformed string is given,
errno is set to EINVAL or ERANGE and 0 is returned. However, a trailing dot is
accepted. (Mostly for coding convenience, treating it as an error would be
awkward.)
