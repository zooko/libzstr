#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

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
		p = (zbyte*)malloc(z2.len);
	} else {
		p = (zbyte*)realloc(z1.buf, z1.len+z2.len);
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
	if ((z2.len == 0) || (z2.buf == NULL)) {
		return { 0, NULL };
	}
	res.buf = (zbyte*)malloc(z1.len);
#ifdef Z_EXHAUST_EXIT
	CHECKMALLOCEXIT(res.buf);
#else
	if (p == NULL) {
		return (zstr){ 0, NULL };
	}
#endif
	memcpy(res.buf, z1.buf, z1.len);
	return res;
}

#undef zstrlen
size_t zstrlen(const czstr z)
{
	return z.len;
}

int
z_eq(const czstr z1, const czstr z2)
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
z_cmp(const czstr z1, const czstr z2)
{
	int mcr = memcmp(z1.buf, z2.buf, _MIN_UNSAFE(z1.len, z2.len));
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
			sprintf(resp, "%02x", *zp++);
			resp += 2;
		}
	}
	result.len = resp - result.buf;
	*resp = '\0';
	newp = (zbyte*)realloc(result.buf, result.len+1);
	assert (newp != NULL); /* I don't see how this realloc could have failed! */
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
	return (zstr){ strlen(cs), cs };
}

#undef CS_AS_Z
zstr
CS_AS_Z(char*const cs)
{
	assert (cs != NULL); /* @precondition */
	return (zstr){ strlen(cs), cs };
}

czstr
cs_as_cz(const char*const cs)
{
	assert (cs != NULL); /* @precondition */
	return (czstr){ strlen(cs), cs };
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
	if (result.buf == NULL)
		result.len = 0;
		return result;
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

