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
	zstr a;
	czstr b;
	union { zstr z; czstr c; } u;
	a = cs_as_z("abcdefgh");
	u.z = a;
	b = u.c;
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
	zstr z2;
	czstr z = cs_as_cz("k");
	zstr rz = repr(z);
	#ifndef NDEBUG
	const czstr a = cs_as_cz("\\\\k");
	#endif

	assert (!strcmp((const char*)rz.buf, "k"));

	z = cs_as_cz("\\k");
	assert (z.len == 2);
	free_z(rz);
	rz = repr(z);
	assert (rz.len == 3);
	assert (a.len == 3);
	assert (!strcmp((const char*)rz.buf, "\\\\k"));
	assert (zeq(cz(rz), a));

	z2 = new_z(4);
	z2.buf[0] = 1;
	z2.buf[1] = 10;
	z2.buf[2] = 100;
	z2.buf[3] = 252;

	free_z(rz);
	rz = repr(cz(z2));
	assert (!strcmp((const char*)rz.buf, "\\x01\\x0ad\\xfc"));

	free_z(z2);
	z2 = new_z(1);
	z2.buf[0] = '\0';

	free_z(rz);
	rz = repr(cz(z2));
	assert (!strcmp((const char*)rz.buf, "\\x00"));

	free_z(z2);
	free_z(rz);
	return 0;
}

/** This test requires manual intervention to provide an appropriate file to read and write. */
void test_stream()
{
	FILE* fp = fopen("/tmp/opsec.p12", "r");
	FILE* fpo;
	zstr z1 = z_from_stream(fp);
	zstr z2;
	fpo = fopen("/tmp/opsec.p12.out", "w");
	cz_to_stream(cz(z1), fpo);
	fflush(fpo);
	fclose(fpo);
	z2 = z_from_stream(fopen("/tmp/opsec.p12.out", "r"));
	assert (zeq(cz(z1), cz(z2)));
}

int main(int argv, char**argc)
{
	test_czstr();
	/*test_stream();*/
	return test_repr();
}


