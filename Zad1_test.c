#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern char* parse(char*);
extern char* unparse(char*);
extern char* suma(char*, char*);
extern char  aless(char*, char*);
extern char* roznica(char*, char*);
extern char* aadd_into(char*, char*, int);
extern char* asub(char*, char*, int);
extern char* iloczyn(char*, char*);

extern char* cparse(char*);
extern char* unparse(char*);
extern char* csuma(char*, char*);
extern char* croznica(char*, char*);
extern char* ciloczyn(char*, char*);
extern char* ciloraz(char*, char*);

int main() {
	int bl;
	char *a, *b, *c, *d, *e, *f, *g;
	b = parse("9998877665544332210");
	a = parse("0");
	assert(strcmp("0", unparse(iloczyn(a,b))) == 0);

	a = parse("-9998877665544332211");
	printf("%d %d\n", iloczyn(a,b), iloczyn(b,a));

	a = parse("12345");
	b = parse("1265");
	aadd_into(a + 4, b + 4, 1);
	assert(strcmp("12310", unparse(a)) == 0);
	b = parse("1099");
	aadd_into(a + 4, b + 4, 2);
	assert(strcmp("13409", unparse(a)) == 0);
	b = parse("10000");
	aadd_into(a + 4, b + 6, 1);
	assert(strcmp("13410", unparse(a)) == 0);
	aadd_into(a + 4, b + 4, 3);
	assert(strcmp("23410", unparse(a)) == 0);
	asub(a + 4, b + 4, 3);
	assert(strcmp("13410", unparse(a)) == 0);
	asub(a + 4, b + 6, 1);
	assert(strcmp("13409", unparse(a)) == 0);
	b = parse("1099");
	asub(a + 4, b + 4, 2);
	assert(strcmp("12310", unparse(a)) == 0);
	b = parse("1265");
	asub(a + 4, b + 4, 1);
	assert(strcmp("12345", unparse(a)) == 0);

	a = parse("-123");
	b = parse("-91");

	a = parse("1231241");
	b = parse("-9999");
	assert(strcmp("1241240", unparse(roznica(a,b))) == 0);

	a = parse("-472345");
	b = parse("12325551254");
	c = parse("3124000");
	d = parse("999912399");
	e = unparse(ciloczyn(suma(a,b),roznica(c,d)));
	assert(strcmp(e, "-12285495673250776691") == 0);
	assert(strcmp("12325078909", unparse(suma(a,b))) == 0);
	assert(strcmp("12325078909", unparse(suma(a,b))) == 0);
	assert(strcmp("12325078909", unparse(suma(a,b))) == 0);

	a = parse("11199");
	b = parse("1");
	assert(strcmp("11200", unparse(suma(a,b))) == 0);

	a = parse("-123");
	b = parse("-91");
	e = unparse(suma(a,b));
	assert(strcmp("-214", e) == 0);

	a = parse("999");
	b = parse("9999");
	e = unparse(suma(a,b));
	assert(strcmp(e, "10998") == 0);

	a = parse("123456");
	b = parse("11111");
	e = unparse(roznica(a,b));
	assert(strcmp("112345", e) == 0);
	assert(strcmp("-112345", unparse(roznica(b,a))) == 0);

	a = parse("2000000");
	b = parse("1");
	e = unparse(roznica(a,b));
	assert(strcmp("1999999", e) == 0);
	assert(strcmp("-1999999", unparse(roznica(b,a))) == 0);

	a = parse("200");
	b = parse( "23");
	assert(strcmp("177", unparse(roznica(a,b))) == 0);
	assert(strcmp("-177", unparse(roznica(b,a))) == 0);

	a = parse("10000");
	b = parse("9999");
	suma(a,b);
	assert(strcmp("1", unparse(roznica(a,b))) == 0);
	assert(strcmp("-1", unparse(roznica(b,a))) == 0);

	a = parse("1");
	b = parse("1");
	assert(strcmp(unparse(roznica(a,b)), "0") == 0);

	a = parse("0");
	b = parse("0");
	assert(strcmp(unparse(suma(a,b)), "0") == 0);
	assert(strcmp(unparse(roznica(a,b)), "0") == 0);
	assert(strcmp(unparse(ciloczyn(a,b)), "0") == 0);

	a = parse("1000");
	b = parse("111");
	assert(strcmp(unparse(suma(a,b)), "1111") == 0);

	a = parse("9999");
	b = parse("1");
	assert(strcmp(unparse(suma(a,b)), "10000") == 0);

	a = parse("999");
	b = parse("9999");
	e = unparse(suma(a,b));
	assert(strcmp(e, "10998") == 0);

	a = parse("1");
	b = parse("99999999999999");
	assert(strcmp("100000000000000", unparse(suma(a,b))) == 0);

	a = parse("123456");
	b = parse("11111");
	assert(strcmp("112345", unparse(roznica(a,b))) == 0);

	a = parse("23456789");
	b = parse("123456789");
	assert(strcmp("-100000000", unparse(roznica(a,b))) == 0);

	a = parse("543");
	b = parse("1000");
	assert(strcmp("-457", unparse(roznica(a,b))) == 0);

	a = parse("923");
	b = parse("923");
	assert(strcmp("0", unparse(roznica(a,b))) == 0);

	a = parse("-1234");
	assert(strcmp(unparse(a), "-1234") == 0);

	a = parse("-123");
	b = parse("-91");
	assert(strcmp("-32", unparse(roznica(a,b))) == 0);
	assert(strcmp("-214", unparse(suma(a,b))) == 0);
	assert(strcmp("-32", unparse(roznica(a,b))) == 0);
	assert(strcmp("-214", unparse(suma(a,b))) == 0);

	b = parse("91");
	assert(strcmp("-214", unparse(roznica(a,b))) == 0);
	assert(strcmp("-32", unparse(suma(a,b))) == 0);

	a = parse("-65");
	assert(strcmp("-156", unparse(roznica(a,b))) == 0);
	assert(strcmp("26", unparse(suma(a,b))) == 0);

	a = parse("65");
	b = parse("91");
	assert(strcmp("-26", unparse(roznica(a,b))) == 0);

	a = parse("1111");
	b = parse("111");
	e = unparse(ciloczyn(a,b));
	assert(strcmp(e, "123321") == 0);

	a = parse("987654321");
	b = parse("-9876543210");
	e = unparse(ciloczyn(a,b));
	assert(strcmp(e, "-9754610577899710410") == 0);

	a = parse("-987654321");
	b = parse("-98765432100");
	e = unparse(ciloczyn(a,b));
	assert(strcmp(e, "97546105778997104100") == 0);

	a = parse("7");
	b = parse("8");
	e = unparse(ciloczyn(a,b));
	assert(strcmp(e, "56") == 0);

	//(a + b) * (c - d)
	a = parse("-472345");
	b = parse("12325551254");
	c = parse("3124000");
	d = parse("999912399");
	e = suma(a,b);
	f = roznica(c,d);
	g = roznica(parse("472345"), b);
	e = unparse(ciloczyn(suma(a,b),roznica(c,d)));
	assert(strcmp(e, "-12285495673250776691") == 0);
	e = unparse(ciloczyn(ciloczyn(suma(a,b),roznica(c,d)),suma(a,b)));
	assert(strcmp(e, "-151419703608993903262112910119") == 0);
	e = unparse(ciloraz(ciloczyn(suma(a,b),roznica(c,d)),suma(a,b)));
	assert(strcmp(e, "-996788399") == 0);

	a = parse("1234567");
	b = parse("12345");
	assert(strcmp(unparse(ciloraz(a,b)), "100") == 0);
	b = parse("1234");
	assert(strcmp(unparse(ciloraz(a,b)), "1000") == 0);

	a = parse("98765432123456789");
	b = parse("5555");
	assert(strcmp("17779555737796", unparse(ciloraz(a,b))) == 0);
	assert(strcmp("0", unparse(ciloraz(b,a))) == 0);

	printf("SUCCESS\n");
}