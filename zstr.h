/**
 * copyright 2002, 2003 Bryce "Zooko" Wilcox-O'Hearn
 * mailto:zooko@zooko.com
 *
 * See the end of this file for the free software, open source license (BSD-style).
 *
 * About this library:
 *
 * This library defines a simple struct with a length and a pointer to byte.  
 * You can use it in place of, or alongside, traditional C null-terminated 
 * char*'s and the <string.h> functions.  The advantages of "lengthed" strings 
 * over null-terminated strings are well known: they can store binary data, they 
 * can sometimes be faster, and they can often be safer with regard to buffer 
 * overflow errors.  For example, see zcat(), which is safer than <string.h>'s 
 * strcat() or strncat(), and even than alternatives such as OpenBSD's 
 * strlcat().
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
 * int eq = zeq(zs[i++], zs[i++])
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
 * benefit from saving another couple of CPU cycles, and then use the macro 
 * version there, being careful to avoid side-effects in the argument 
 * expressions of the macro.
 *
 * (By the way, if the NDEBUG flag is unset, then libzstr will use the 
 * function versions even if you specified the all-uppercase name of the macro 
 * version.)
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

static char const* const zstr_h_cvsid = "$Id: zstr.h,v 1.7 2004/02/03 21:06:04 zooko Exp $";

static int const zstr_vermaj = 0;
static int const zstr_vermin = 9;
static int const zstr_vermicro = 6;
static char const* const zstr_vernum = "0.9.6";

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define Z_EXHAUST_EXIT
#include "zutil.h" /* http://sf.net/projects/libzutil */

/**
 * A zstr is simply an unsigned int length and a pointer to a buffer of 
 * unsigned chars.
 */
typedef struct {
	size_t len; /* the length of the string (not counting the null-terminating character) */
	zbyte* buf; /* pointer to the first byte */
} zstr;

/**
 * A zstr is simply an unsigned int length and a pointer to a buffer of 
 * const unsigned chars.
 */
typedef struct {
	size_t len; /* the length of the string (not counting the null-terminating character) */
	const zbyte* buf; /* pointer to the first byte */
} czstr;

   /** commonly used functions */

/**
 * Add z2 onto the end of z1, reallocating z1 to be big enough.
 *
 * @param z1 this will be realloc'ed which means you cannot use it after 
 *     calling zcat()!  Use the return value instead.
 * @param z2 the string to be appended onto the end of z1
 *
 * @return the new z1
 *
 * On  malloc failure (if not Z_EXHAUST_EXIT) then it will return a zstr with 
 * its .buf member set to NULL and its .len member set to 0.  In that case z1 
 * is completely unchanged (and can safely be used as if this function had not 
 * been called).
 */
zstr
zcat(zstr z1, czstr z2);

/**
 * Copy z2 and return copy in newly allocated zstr..
 *
 * @param z1
 *
 * @return the new zstr
 *
 * On  malloc failure (if not Z_EXHAUST_EXIT) then it will return a zstr with 
 * its .buf member set to NULL and its .len member set to 0.
 */
zstr
zdup(czstr z1);

/**
 * @return the length of the zstr, in bytes, not counting the null terminating 
 *     character.
 *
 * Or you can just write "z.len".
 */
size_t
zstrlen(czstr z);

/**
 * @return 1 if the strings are identical, else 0
 */
int
zeq(czstr z1, czstr z2);
int
ZEQ(czstr z1, czstr z2);

/**
 * @return <0 if z1<z2, 0 if z1==z2, or >0 if z1>z2 (same as strcmp())
 */
int
zcmp(czstr z1, czstr z2);

/**
 * The human-readable representation is: if a byte is a printable ASCII character other than `\', 
 * then just show it.  If it is a '\', then show double backslash: '\\'.  Else 
 * the byte is not a printable ASCII character, so show its value in lowercase 
 * hexadecimal notation with a preceding backslash, e.g. `\xFF'.
 * (This is like the Python human-readable representation.)
 *
 * @return a newly allocated zstr containing a human-readable representation 
 *     of z.
 */
zstr
repr(czstr z);

/**
 * @return a zstr pointing to cs.
 *
 * @precondition cs must not be NULL.
 */
zstr
cs_as_z(char* cs);
zstr
CS_AS_Z(char* cs);

/**
 * (You should use cs_as_cz() instead of cs_as_z() if the argument is a string 
 * literal.)
 *
 * @return a czstr pointing to cs.
 *
 * @precondition cs must not be NULL.
 */
czstr
cs_as_cz(const char* cs);
czstr
CS_AS_CZ(const char* cs);

/**
 * @return a cs pointing to a czstr's buffer.  (It could be NULL.)
 */
const char*
cz_as_cs(czstr cz);
const char*
CZ_AS_CS(czstr cz);

/**
 * @return a czstr of z (shallow copy).
 */
czstr
cz(zstr z);

/**
 * Allocates space, copies the contents of cs and returns a zstr pointing to 
 * the result.
 *
 * On  malloc failure (if not Z_EXHAUST_EXIT) then it will return a zstr with 
 * its .buf member set to NULL and its .len member set to 0.
 *
 * @precondition cs must not be NULL.
 */
zstr 
new_z_from_cs(const char* cs);

/**
 * Allocates space.
 *
 * On  malloc failure (if not Z_EXHAUST_EXIT) then it will return a zstr with 
 * its .buf member set to NULL and its .len member set to 0.
 */
zstr 
new_z(size_t len);

/**
 * Allocates space, copies the first len bytes of cs and returns a zstr 
 * pointing to the result.
 *
 * On  malloc failure (if not Z_EXHAUST_EXIT) then it will return a zstr with 
 * its .buf member set to NULL and its .len member set to 0.
 *
 * @precondition cs must not be NULL.
 * @precondition len must not be 0.
 */
zstr 
new_z_from_cs_and_len(const char* cs, size_t len);

/**
 * Free the memory used by the zstr.
 *
 * Alternately you could write free(z.buf).
 *
 * @precondition z.buf must not be NULL.: z.buf != NULL
 *
 * (This precondition is here only to help you find memory mismanagement in 
 * your code.)
 */
void
free_z(zstr z);

/**
 * Read from a stream (until EOF) into a zstr.  This does nothing with the fp 
 * argument except call fread(), feof(), and ferror() on it, therefore it reads 
 * from whereever fp is currently set to the end of fp.  It does not fclose() 
 * fp after it is done.
 *
 * This invokes realloc() multiple times as needed along the way.
 *
 * @precondition fp must not be NULL.
 */
zstr z_from_stream(FILE* fp);

/**
 * Write the contents of cz to a stream.  This just calls 
 * fwrite(cz.buf, sizeof(zbyte), cz.len, fp).
 *
 * @precondition fp must not be NULL.
 */
void cz_to_stream(czstr cz, FILE* fp);

/**
 * A simple internal consistency check.  First, it verifies that cz.len is 0 
 * only if cz.buf is NULL or if cz.buf contains a zero-length string (that is a 
 * buffer of size 1 containing the null-terminating character).  Second, it 
 * verifies that strlen(cz.buf) is always <= cz.len (which is true because since 
 * we always append a null-terminating char).  If either of these aren't true 
 * then it raises an error with runtime_assert().  Else, it returns 1.
 */
int cz_check(czstr cz);

/*** macro definitions ***/

typedef union { zstr z; czstr c; } z_union_zstr_czstr;

#ifdef NDEBUG
#define zstrlen(z) ((z).len)
#define Z_EQ(z1, z2) (((z1).len==(z2).len)&&(!memcmp((z1).buf, (z2).buf, (z1).len)))
#define CS_AS_Z(cs) ((zstr){ strlen(cs), cs };)
#define CS_AS_CZ(cs) ((czstr){ strlen((cs)), (cs) };)
#define free_z(z) (free((void*)(z).buf))
#define cz_as_cs(cz) ((const char*)(cz.buf))
#define CZ_AS_CS(cz) ((const char*)(cz.buf))
/* Please avert your gaze.  We are now going to trick the compiler into converting a zstr directly into a czstr by dint of casting into a union and then taking its czstr element. */
#define cz(z) ((z_union_zstr_czstr){ a }).c

#endif /* #ifdef NDEBUG */

#endif /* #ifndef _INCL_zstr_h */


/**
 * Copyright (c) 2002, 2003 Bryce "Zooko" Wilcox-O'Hearn
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
