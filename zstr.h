/**
 * copyright 2002 Bryce "Zooko" Wilcox-O'Hearn
 * mailto:zooko@zooko.com
 *
 * See the end of this file for the free software, open source license (BSD-style).
 *
 * About this library:
 *
 * This library defines a simple struct with a length and a pointer to byte.  
 * You can use it in place of, or along with, traditional C null-terminated 
 * char*'s and the <string.h> functions.  The advantages of "lengthed" strings 
 * over null-terminated strings are well known: they can store binary data, they 
 * can sometimes be faster, and it can sometimes be easier to avoid buffer 
 * overflow errors when using a library like this one.  (For example, see 
 * `zcat()', which is faster and safer than <string.h>'s `strcat()' or 
 * `strncat()', and even than most alternatives such as OpenBSD's `strlcat()'.)
 *
 *
 * About macros:
 *
 * Many of these functions have macro versions, which are named the same name as 
 * the function but in ALL UPPERCASE letters.  The macro version may be faster 
 * (unless your C compiler was clever enough to efficiently inline the function 
 * version), but it suffers from "the macro problem" that the argument 
 * expressions get evaluated more than once, which means that if any of the 
 * arguments have side-effects those side-effects will occur more than once.  
 * For example, suppose that `zs' is an array of zstrs.  Then if you write:
 * int i = 0;
 * int eq = z_eq(zs[i++], zs[i++])
 * it will compare the first and second elements of the array, but if you write:
 * int i = 0;
 * int eq = Z_EQ(zs[i++], zs[i++])
 * then the preprocessor will expand it to:
 * int i = 0;
 * int eq = ((zs[i++].len==zs[i++].len)&&(!memcmp(zs[i++].buf, zs[i++].buf, zs[i++].len)))
 * which isn't right at all!
 *
 * The macro versions also omit the assertion checking that the function 
 * versions have.  In general, you probably ought to just use the function 
 * version until you have identified a particular performance hotspot that could 
 * benefit from another couple of CPU cycles of speed, and then use the macro 
 * version there, being careful to avoid side-effects in the argument 
 * expressions of the macro.
 *
 * (By the way, if the NDEBUG flag is set, then libzstr will use the function 
 * versions even if you specified the all-uppercase name of the macro version.)
 *
 *
 * About memory exhaustion:
 *
 * What should your program do if it tries to allocate more memory and is told 
 * by the operating system that there is no more memory available?  Unless you 
 * have some unusual requirements and an exceptionally clever recovery scheme, 
 * the right thing to do is probably to print an error message and exit.  If you 
 * define the flag Z_EXHAUST_ABORT before #including the "zutil.h" header file, 
 * then libzstr will do just that whenever it gets memory allocation failure.  
 * If you do not define that flag, then libzstr will return a "null" value of 
 * some sort when it gets memory allocation failure.  The specifics of that 
 * behavior are given in the documentation of each function that can incur 
 * memory allocation failure.
 */
#ifndef _INCL_zstr_h
#define _INCL_zstr_h

static char const* const zstr_h_cvsid = "$Id: zstr.h,v 1.2 2002/10/27 20:18:03 zooko Exp $";

static int const zstr_vermaj = 0;
static int const zstr_vermin = 2;
static int const zstr_vermicro = 1;
static char const* const zstr_vernum = "0.2.1";

#include <stdlib.h>
#include <string.h>

#define Z_EXHAUST_EXIT
#include "zutil.h" /* XXX add docs about where to get libzutil */

typedef struct {
	size_t len; /* the length of the string (not counting the null-terminating character) */
	zbyte* buf; /* pointer to the first byte */
} zstr;

typedef struct {
	size_t len; /* the length of the string (not counting the null-terminating character) */
	const zbyte* buf; /* pointer to the first byte */
} czstr;

   /** commonly used functions */

/**
 * Add z2 onto the end of z1, reallocating z1 to be big enough.
 *
 * @param z1 this will be realloc'ed which means you cannot use it after calling `zcat()'!  Use the return value instead.
 * @param z2 the string to be appended onto the end of z1
 *
 * @return the new `z1'
 *
 * On  malloc failure (if not Z_EXHAUST_EXIT) then it will return a zstr with 
 * its `.buf' member set to NULL and its `.len' member set to 0.  In that case 
 * z1 is completely unchanged (and can safely be used as if this function had 
 * not been called).
 */
zstr
zcat(zstr z1, const czstr z2);

/**
 * @return the length of the zstr, in bytes, not counting the null terminating 
 *     character.
 *
 * Actually I recommend that you just write `z.len' yourself, which is faster 
 * and IMO clearer.
 */
size_t
zstrlen(const czstr z);

/**
 * @return 1 if the strings are identical, else 0
 */
int
z_eq(const czstr z1, const czstr z2);
int
Z_EQ(const czstr z1, const czstr z2);

/**
 * @return <0 if z1<z2, 0 if z1==z2, or >0 if z1>z2 (same as `strcmp()')
 */
int
z_cmp(const czstr z1, const czstr z2);

/**
 * The human-readable representation is: if a byte is a printable ASCII character other than `\', 
 * then just show it, if it is a '\', then show double backslash: '\\', else the byte is not a 
 * printable ASCII character, so show its value in lowercase hexadecimal notation e.g. `\xFF'.
 *
 * @return a newly allocated zstr containing a human-readable representation of z.
 */
zstr
repr(const czstr z);

/**
 * @return a zstr pointing to `cs'.
 *
 * @precondition `cs' must not be NULL.
 */
zstr
cs_as_z(char*const cs);
zstr
CS_AS_Z(char*const cs);

/**
 * (You should use `cs_as_cz()' instead of `cs_as_z()' if the argument is a string literal.)
 *
 * @return a czstr pointing to `cs'.
 *
 * @precondition `cs' must not be NULL.
 */
czstr
cs_as_cz(const char*const cs);
czstr
CS_AS_CZ(const char*const cs);

/**
 * @return a czstr of `z' (shallow copy).
 */
czstr
cz(const zstr z);

/**
 * Allocates space, copies the contents of `cs' and returns a zstr pointing to 
 * the result.
 *
 * On  malloc failure (if not Z_EXHAUST_EXIT) then it will return a zstr with 
 * its `.buf' member set to NULL and its `.len' member set to 0.
 *
 * @precondition `cs' must not be NULL.
 */
zstr 
new_z_from_cs(const char* cs);

/**
 * Allocates space.
 *
 * On  malloc failure (if not Z_EXHAUST_EXIT) then it will return a "null" zstr 
 * with its `.buf' member set to NULL and its `.len' member set to 0.
 */
zstr 
new_z(const size_t len);

/**
 * Allocates space, copies the first `len' bytes of `cs' and returns a zstr 
 * pointing to the result.
 *
 * On  malloc failure (if not Z_EXHAUST_EXIT) then it will return a zstr with 
 * its `.buf' member set to NULL and its `.len' member set to 0.
 *
 * @precondition `cs' must not be NULL.
 * @precondition `len' must not be 0.
 */
zstr 
new_z_from_cs_and_len(const char*const cs, size_t len);

/**
 * Free the memory used by the zstr.
 *
 * Alternately you could write `free(z.buf)'.
 *
 * @precondition z.buf must not be NULL.: z.buf != NULL
 *
 * (This precondition is here only to help you find memory mismanagement in your code.)
 */
void
free_z(zstr z);

/*** macro definitions ***/

#ifdef NDEBUG
#define zstrlen(z) ((z).len)
#define Z_EQ(z1, z2) (((z1).len==(z2).len)&&(!memcmp((z1).buf, (z2).buf, (z1).len)))
#define CS_AS_Z(cs) ((zstr){ strlen(cs), cs };)
#define CS_AS_CZ(cs) ((czstr){ strlen((cs)), (cs) };)
/* Please avert your gaze.  We are now going to trick the compiler into converting a zstr directly into a czstr by dint of casting into a union and then taking its `czstr' element. */
#define free_z(z) (free((void*)(z).buf))
#endif /* #ifdef NDEBUG */

#endif /* #ifndef _INCL_zstr_h */

#if 0 /* Whoops this doesn't compile with gcc 2.95!  --Zooko 2002-10-27 */
#define cz(z) ((union { zstr zs; czstr czs; }){ (z) }.czs)
#endif /* Whoops this doesn't compile with gcc 2.95!  --Zooko 2002-10-27 */

/**
 * Copyright (c) 2002 Bryce "Zooko" Wilcox-O'Hearn
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software to deal in this software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of this software, and to permit
 * persons to whom this software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of this software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
