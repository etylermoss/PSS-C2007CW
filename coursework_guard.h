#ifndef __COURSEWORK_H__
#define __COURSEWORK_H__

/* NOTE: This header file exists because coursework.h does not use an include
 * guard, and pollutes the namespace with its own version of bool, so without
 * this file, stdbool could not be used.
*/

#ifndef __GOOD_BOOL
#define __GOOD_BOOL
#undef bool
#undef false
#undef true
#endif

#define bool coursework_bad_bool
#include "coursework.h"
#undef bool

#ifdef __GOOD_BOOL
#define bool _Bool
#define false 0
#define true 1
#endif

#endif