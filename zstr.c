/**
 * copyright 2002-2004 Bryce "Zooko" Wilcox-O'Hearn
 * mailto:zooko@zooko.com
 *
 * See the end of this file for the simple, permissive free software, open 
 * source license.
*/
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "moreassert.h"

#include "zstr.h"

/** commonly used functions */

zstr
zcat(zstr z1, const czstr z2)
{
	zbyte* p;
	if (z2.len == 0) {
		return z1;
	}
	if (z1.buf == NULL) {
		assert (z1.len == 0);
		p = (zbyte*)malloc(z2.len+1);
	} else {
		p = (zbyte*)realloc(z1.buf, z1.len+z2.len+1);
	}
#ifdef Z_EXHAUST_EXIT
	CHECKMALLOCEXIT(p);
#else
	if (p == NULL) {
		return (zstr){ 0, NULL };
	}
#endif
	memcpy(p+z1.len, z2.buf, z2.len+1);
	return (zstr){ z1.len+z2.len, p };
}

zstr
zdup(const czstr z1)
{
	zstr res;
	if ((z1.len == 0) || (z1.buf == NULL)) {
		return (zstr){ 0, NULL };
	}
	res.buf = (zbyte*)malloc(z1.len+1);
#ifdef Z_EXHAUST_EXIT
	CHECKMALLOCEXIT(res.buf);
#else
	if (p == NULL) {
		return (zstr){ 0, NULL };
	}
#endif
	memcpy(res.buf, z1.buf, z1.len+1);
	res.len = z1.len;
	return res;
}

#undef zstrlen
size_t zstrlen(const czstr z)
{
	return z.len;
}

int
zeq(const czstr z1, const czstr z2)
{
	return (z1.len==z2.len)&&(!memcmp(z1.buf, z2.buf, z1.len));
}

#undef Z_EQ
int
Z_EQ(const czstr z1, const czstr z2)
{
	return (z1.len==z2.len)&&(!memcmp(z1.buf, z2.buf, z1.len));
}

int
zcmp(const czstr z1, const czstr z2)
{
	int mcr = memcmp(z1.buf, z2.buf, MIN(z1.len, z2.len));
	if (mcr)
		return mcr;
	else
		if (z1.len == z2.len)
			return 0;
		else
			if (z1.len < z2.len)
				return -1;
			else
				return 1;
}

zstr
repr(const czstr z)
{
	const zbyte*const ze = z.buf+z.len;
	const zbyte* zp = z.buf;
	zbyte* resp;
	zbyte* newp;

	zstr result = new_z(z.len*4);
	if (result.buf == NULL)
		return result;

	resp = result.buf;
	while (zp < ze) {
		if (*zp == '\\') {
			*resp++ = '\\';
			*resp++ = '\\';
			zp++;
		} else if  (isgraph(*zp) || (*zp == ' ')) {
			*resp++ = *zp++;
		} else {
			*resp++ = '\\';
			*resp++ = 'x';
			sprintf((char*)resp, "%02x", *zp++);
			resp += 2;
		}
	}
	result.len = resp - result.buf;
	*resp = '\0';
	newp = (zbyte*)realloc(result.buf, result.len+1);
#ifdef Z_EXHAUST_EXIT
	CHECKMALLOCEXIT(newp);
#else
	if (newp == NULL) {
		return (zstr){ 0, NULL };
	}
#endif
	result.buf = newp;
	return result;
}

#undef free_z
void
free_z(zstr z)
{
	assert (z.buf != NULL); /* @precondition z.buf must not be NULL. */
	free(z.buf);
}

zstr
cs_as_z(char*const cs)
{
	assert (cs != NULL); /* @precondition */
	return (zstr){ strlen(cs), (zbyte*)cs };
}

#undef CS_AS_Z
zstr
CS_AS_Z(char*const cs)
{
	assert (cs != NULL); /* @precondition */
	return (zstr){ strlen(cs), (zbyte*)cs };
}

czstr
cs_as_cz(const char*const cs)
{
	assert (cs != NULL); /* @precondition */
	return (czstr){ strlen(cs), (const zbyte*)cs };
}

#undef CS_AS_CZ
czstr
CS_AS_CZ(const char*const cs)
{
	assert (cs != NULL); /* @precondition */
	return (czstr){ strlen(cs), (const zbyte*)cs };
}

#undef cz_as_cs
const char*
cz_as_cs(const czstr cz)
{
	return (const char*)cz.buf;
}

#undef CZ_AS_CS
const char*
CZ_AS_CS(const czstr cz)
{
	return (const char*)cz.buf;
}

#undef cz
czstr
cz(const zstr z)
{
	return (czstr){ z.len, z.buf };
}

zstr 
new_z(const size_t len)
{
	zstr result;
	result.buf = (zbyte*)malloc(len+1);
#ifdef Z_EXHAUST_EXIT
	CHECKMALLOCEXIT(result.buf);
#else
	if (result.buf == NULL) {
		result.len = 0;
		return result;
	}
#endif
	result.len = len;
	result.buf[len] = '\0';
	return result;
}

zstr
new_z_from_cs(const char*const cs)
{
	zstr result;
	assert (cs != NULL); /* @precondition */
	result.len = strlen(cs);

	result.buf = (zbyte*)malloc(result.len+1);
#ifdef Z_EXHAUST_EXIT
	CHECKMALLOCEXIT(result.buf);
#else
	if (result.buf == NULL) {
		result.len = 0;
		return result;
	}
#endif
	memcpy(result.buf, cs, result.len+1);
	return result;
}

zstr 
new_z_from_cs_and_len(const char*const cs, const size_t len)
{
	zstr result;
	assert (cs != NULL); /* @precondition */
	assert (len != 0); /* @precondition */

	result.len = len;
	result.buf = (zbyte*)malloc(result.len+1);
#ifdef Z_EXHAUST_EXIT
	CHECKMALLOCEXIT(result.buf);
#else
	if (result.buf == NULL) {
		result.len = 0;
		return result;
	}
#endif

	memcpy(result.buf, cs, result.len);
	result.buf[len] = '\0';

	return result;
}

static const size_t BUFINCREMENT = 16384;

zstr 
z_slurp_stream(FILE* fp)
{
	size_t res, bufsiz, space, len;
	zbyte* buf;

	len = 0;
	assert (fp != NULL); /* @precondition */

	bufsiz = BUFINCREMENT + 1;
	buf = (zbyte*)malloc(bufsiz);
#ifdef Z_EXHAUST_EXIT
	CHECKMALLOCEXIT(buf);
#else
	if (buf == NULL) {
		return (zstr){ 0, NULL };
	}
#endif
	while (!feof(fp)) {
		runtime_assert(!ferror(fp), "file error");
		space = (bufsiz - 1) - len;
		if (space < (BUFINCREMENT/2)) {
			bufsiz += BUFINCREMENT;
			buf = (zbyte*)realloc(buf, bufsiz);
#ifdef Z_EXHAUST_EXIT
			CHECKMALLOCEXIT(buf);
#else
			if (buf == NULL) {
				return (zstr){ 0, NULL };
			}
#endif
			space = (bufsiz - 1) - len;
		}
		res = fread(buf + len, sizeof(zbyte), space, fp);
		len += res;
	}
	assert(len < bufsiz); /* error internal to this function */
	buf = (zbyte*)realloc(buf, len+1);
#ifdef Z_EXHAUST_EXIT
	CHECKMALLOCEXIT(buf);
#else
	if (buf == NULL) {
		return (zstr){ 0, NULL };
	}
#endif
	buf[len] = '\0';
	return (zstr){ len, buf };
}

zstr
z_decode(FILE* fp)
{
	size_t res;
	zbyte* len;
	zstr result;
	assert(fp != NULL); /* @precondition */

	len = (zbyte*)malloc(4);
	CHECKMALLOCEXIT(len);
	res = fread(len, 4, 1, fp);
	runtime_assert(res == 1, "fread() failed to read the length.");
	result.len = (size_t)uint32_decode(len);
	free(len);
	printf("result.len = %d\n", result.len);
    result.buf = (zbyte*)malloc(result.len + 1);
#ifdef Z_EXHAUST_EXIT
    CHECKMALLOCEXIT(result.buf);
#else
    if (result.buf == NULL) {
        result.len = 0;
        return result;
    }
#endif
	if(result.len > 0){
		res = fread(result.buf, result.len, 1, fp);
		runtime_assert(res == 1, "Failed to read all of the data.");
		result.buf[result.len] = '\0';
	}
	return result;
}	

void
z_encode(czstr cz, FILE* fp)
{
	size_t res;
	zbyte* len;
	assert(fp != NULL); /* @precondition */
	
	len = (zbyte*)malloc(4);
	runtime_assert(len != NULL, "Memory exhaustion.");
	uint32_encode(cz.len, len);
	res = fwrite(len, sizeof(zbyte), 4, fp);
	runtime_assert(res = 4, "fwrite() failed to completely write the data.");
	res = fwrite(cz.buf, sizeof(zbyte), cz.len, fp);
	runtime_assert(res = cz.len, "fwrite() failed to completely write the data.");
}
	
void 
cz_to_stream(const czstr cz, FILE* fp)
{
	size_t res;
	assert (fp != NULL); /* @precondition */

	res = fwrite(cz.buf, sizeof(zbyte), cz.len, fp);
	runtime_assert(res == cz.len, "fwrite() failed to completely write the data.");
}

int cz_check(const czstr cz)
{
	runtime_assert((cz.len == 0) == ((cz.buf == NULL) || (cz.buf[0] == '\0')), "If cz.len is 0 then cz.buf is required to be either NULL or a pointer to an array of length 1 containing a null char.");
	runtime_assert((cz.buf == NULL) || (strlen(cz.buf) <= cz.len), "If cz.buf is non-NULL then strlen(cz.buf) is required to be less than or equal to cz.len.");
	return 1;
}

/**
 * Copyright (c) 2002-2004 Bryce "Zooko" Wilcox-O'Hearn
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software to deal in this software without restriction, including
 * without limitation the rights to use, modify, distribute, sublicense, and/or 
 * sell copies of this software, and to permit persons to whom this software is 
 * furnished to do so, provided that the above copyright notice and this 
 * permission notice is included in all copies or substantial portions of this 
 * software. THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED.
 */
