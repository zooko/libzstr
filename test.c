#include "zstr.h"

#include <assert.h>
#include <stdio.h>

int test_czstr_manual()
{
	zstr a = cs_as_z("abcdefgh");
	czstr b = { a.len, a.buf };
	printf("%d:%s\n", b.len, b.buf);
	return 0;
}

int test_czstr_union_cast_manual()
{
	zstr a = cs_as_z("abcdefgh");
	czstr b = (union { zstr z; czstr c; }){a}.c;
	printf("%d:%s\n", b.len, b.buf);
	return 0;
}

int test_czstr()
{
	zstr a = cs_as_z("abcdefgh");
	czstr b = cz(a);
	printf("%d:%s\n", b.len, b.buf);
	/*b.len = 5;*/ /* should be a compile error since b is const. */
	/*b.buf = NULL;*/ /* should be a compile error since b is const. */
	/*b.buf[0] = '\0';*/ /* should be a compile error since b is a czstr. */
	return 0;
}

int test_new_zstr_from_cs()
{
	zstr z;
	size_t l;
	z = new_z_from_cs("testee");
	l = z.len;
	assert (l == z.len);
	return z.len;
}

int test_repr()
{
	czstr z = cs_as_cz("k");
	czstr rz = cz(repr(z));
	assert (!strcmp(rz.buf, "k"));

	z = cs_as_cz("\\k");
	assert (z.len == 2);
	rz = cz(repr(z));
	assert (rz.len == 3);
	#ifndef NDEBUG
	const czstr a = cs_as_cz("\\\\k");
	#endif
	assert (a.len == 3);
	assert (!strcmp(rz.buf, "\\\\k"));
	assert (z_eq(rz, a));

	zstr z2 = new_z(4);
	z2.buf[0] = 1;
	z2.buf[1] = 10;
	z2.buf[2] = 100;
	z2.buf[3] = 252;

	rz = cz(repr(cz(z2)));
	assert (!strcmp(rz.buf, "\\x01\\x0ad\\xfc"));

	z2 = new_z(1);
	z2.buf[0] = '\0';

	rz = cz(repr(cz(z2)));
	assert (!strcmp(rz.buf, "\\x00"));

	return 0;
}

int main(int argv, char**argc)
{
	/*test_czstr();*/
	return test_repr();
}

