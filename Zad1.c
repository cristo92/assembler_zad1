#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern char* parse(char*);
extern char* unparse(char*);

char* cparse(char* str) {
	int n = 0, mul = 1;
	if(*str == '-') {
		mul = -1;
		str++;
	}
	char *c = str, *ptr, *ret;
	while(*c != 0) {
		n++;
		c++;
	}
	ptr = (char*)malloc((n + 3) / 2 + 4);
	ret = ptr;
	*(int*)ptr = n * mul;
	ptr += 4;
	c = str + n;
	do {
		c--;
		*ptr = *c - '0';
		if (c == str) {
			ptr++;
			break;
		}
		c--;
		*ptr += (*c - '0') << 4;
		ptr++;
	} while(c != str);
	*ptr = 0;
	return ret;
}


char *cunparse(char *str) {
	int n = *(int*)str;
	char *c = str, *ptr, *ret;
	if(n == 0) {
		ptr = malloc(1);
		*ptr = '0';
		return ptr;
	}
	else if(n < 0) {
		n = 0 - n;
		ptr = malloc(n + 2);
		ret = ptr;
		*(ptr) = '-';
		ptr++;
	} else {
		ptr = malloc(n + 1);
		ret = ptr;
	}
	str += 4;
	c = str + (n + 1) / 2;

	if(n & 1) {
		c--;
		*ptr = ((*c) & 0xf) + '0';
		ptr++;
	}
	if (n > 1) {
		do {
			c--;
			*ptr = (((*c) >> 4) & 0xf) + '0';
			ptr++;
			*ptr = ((*c) & 0xf) + '0';
			ptr++;
		}	while(c != str);	
	}
	*ptr = 0;
	return ret;
}

char CF, AF;

void adc(char *src, char x) {
	char l1 = *src & 0xf,
         l2 = x & 0xf,
         h1 = ((*src) >> 4) & 0xf,
         h2 = (x >> 4) & 0xf;
    if(CF) l1++;
    if (l1 + l2 >= 10) h1++;
    CF = 0;
    if (h1 + h2 >= 10) CF = 1;
    *src = ((l1 + l2) % 10) + (((h1 + h2) % 10) << 4);
}

char less(char *a, char *b) {
	int n = *(int*)a, i, n2 = *(int*)b;
	if(n < 0) n *= -1;
	if(n2 < 0) n2 *= -1;
	if(n > n2) return 0;
	a += 4;
	b += 4;
	n = (n + 1) / 2;
	for(i = n - 1; i >= 0; i--) {
		if(a[i] == b[i]) continue;
		if((unsigned char)a[i] > (unsigned char)b[i]) return 0;
		else return 1;
	}
	return 0;
}

void add_into(char *dest, char *src, int n) {
	int i;
	CF = 0;
	for(i = 0; i < n; i++) {
		adc(dest + i, src[i]);
	}
}

void mywrite(char *s, int n) {
	int i = 0;
	for(i = 0; i < n; i ++) printf("0x%x ", s[i]);
	printf("\n");
}

char less3(char *s1, char *s2, int n) {
	int i = 0;
	//to jest źle!
	for(i = n - 1; i >= 0; i--) {
		if(s1[i] == s2[i]) continue;
		if((unsigned char)s1[i] > (unsigned char)s2[i]) return 0;
		else return 1;
	}
	return 0;
}

void sub(char *s1, char *s2, int n) {
	int i = 0;
	char ch;
	for(i = 0; i < n; i++) {
		ch = s2[i];
		adc(&ch, 0);
		if(ch == 0) continue;
		ch = 0x99 - ch;
		adc(&ch, 1);
		adc(s1 + i, ch);
		CF = CF ^ 1;
	}
	while(CF) {
		ch = 0x99;
		adc(s1 + i, ch);
		i++;
		CF = CF ^ 1;
	}
}

char *ciloraz(char *s1, char *s2) {
	int n1 = *(int*)s1, m1;
	int n2 = *(int*)s2, m2;
	int mul = 1, i, t, offs = 0;
	if(n1 < 0) {
		mul *= -1;
		n1 *= -1;
	}
	if(n2 < 0) {
		mul *= -1;
		n2 *= -1;
	}
	if(n1 < n2 || less(s1, s2)) {
		return parse("0");
	}
	m1 = (n1 + 1) / 2;
	m2 = (n2 + 1) / 2;
	s1 += 4;
	s2 += 4;
	char *ret = malloc(n1), *ptr = ret, *cm;
	memset(ptr, 0, n1);

	char *a = malloc(m1 + 1);
	memcpy(a, s1, m1);
	a[m1] = 0;
	char **bcc = malloc(2);
	bcc[0] = malloc(m2 + 1);
	bcc[1] = malloc(m2 + 1);
	memset(bcc[0], 0, m2 + 1);
	memset(bcc[1], 0, m2 + 1);
	memcpy(bcc[0], s2, m2);
	memcpy(bcc[1], s2, m2);
	for(i = 0; i < 9; i++) {
		add_into(bcc[1], bcc[0], m2 + 1);
	}

	if((n1 - n2) & 1) {
		t = (n1 - n2 - 1) / 2;
	}
	else {
		t = (n1 - n2) / 2;
		offs = 1;
	}
	for(cm = a + t; cm >= a; cm--) {
		while(!less3(cm, bcc[1], m2 + 1)) {
			(*ptr)++;
			sub(cm, bcc[1], m2 + 1);
		}
		*ptr += '0';
		ptr++;

		while(!less3(cm, bcc[0], m2 + 1)) {
			(*ptr)++;
			sub(cm, bcc[0], m2 + 1);
		}
		*ptr += '0';
		ptr++;
	}
	*ptr = 0;
	for(ptr = ret; *ptr == '0'; ptr++);
	ptr = cparse(ptr);
	*(int*)ptr *= mul;
	free(ret);

	free(bcc[0]);
	free(bcc[1]);
	free(bcc);
	return ptr;
}

char *ciloczyn(char *s1, char *s2) {
	int n1 = *(int*)s1;
	int n2 = *(int*)s2, t, i;
	int mul = 1;
	char *tc, c;
	if(n1 < 0) {
		mul *= -1;
		n1 *= -1;
	}
	if(n2 < 0) {
		mul *= -1;
		n2 *= -1;
	}

	if(n1 < n2 || less(s1, s2)) {
		t = n1;
		n1 = n2;
		n2 = t;
		tc = s1;
		s1 = s2;
		s2 = tc;
	}
	s1 += 4;
	s2 += 4;
	char *ret = malloc(n1 + n2 + 5);
	memset(ret, 0, n1 + n2 + 5);
	*(int*)ret = n1 + n2;
	char *ptr = ret + 4;
	n1 = (n1 + 1) / 2;
	n2 = (n2 + 1) / 2;

	// acc = 0cstring0
	// zero na początku jest na potrzeby wspierania operacji *10
	// zero na końcu jest na wypadek przepełnienia na ostatnim
	// bicie
	char **acc = malloc(18 * 4);
	acc[0] = malloc(n1 + 2);
	memcpy(acc[0], s1, n1);
	acc[0][n1] = 0;
	acc[0][n1 + 1] = 0;
	for(i = 1; i < 10; i++) {
		acc[i] = malloc(n1 + 2);
		memcpy(acc[i], acc[i - 1], n1 + 2);
		add_into(acc[i], s1, n1 + 1);
	}
	for(i = 10; i < 18; i++) {
		acc[i] = malloc(n1 + 2);
		memcpy(acc[i], acc[i - 1], n1 + 2);
		add_into(acc[i], acc[9], n1 + 2);
	}

	for(i = 0; i < n2; i++) {
		c = s2[i] & 0xf;
		if(c > 0) {
			add_into(ptr, acc[c - 1], n1 + 1);
		}
		c = (s2[i] >> 4) & 0xf;
		if(c > 0) {
			add_into(ptr, acc[c - 1 + 9], n1 + 2);
		}
		ptr++;
	}

	n1 = *(int*)ret;
	n2 = (n1 + 1) / 2;
	ptr = ret + 4 + n2 - 1;
	if((*ptr >> 4) != 0) n1 = n2 * 2;
	else if(*ptr != 0) n1 = n2 * 2 - 1;
	else n1 = (n2 - 1) * 2;
	*(int*)ret = n1 * mul;

	for(i = 0; i < 9; i++) {
		free(acc[i]);
	}
	free(acc);
	return ret;
}

char *_croznica(char *s1, char *s2) {
	char *tc;
	int mul = 1;
	if(*(int*)s1 < *(int*)s2 || less(s1, s2)) {
		tc = s1;
		s1 = s2;
		s2 = tc;
		mul = -1;
	}
	int n1 = *(int*)s1, m1, i;
	int n2 = *(int*)s2, m2;
	s1 += 4;
	s2 += 4;

	m1 = (n1 + 1) / 2;
	m2 = (n2 + 1) / 2;
	char *ret = malloc(m1 + 4 + 1);
	char *ptr = ret + 4;
	memcpy(ptr, s1, m1);

	char t;
	for(i = 0; i < m2; i++) {
		t = *(s2 + i);
		adc(&t, 0);
		if(t == 0) continue;
		t = 0x99 - t;
		adc(&t, 1);
		adc(ptr + i, t);
		CF = CF ^ 1;
	}
	for(i = m2; i < m1; i++) {
		if(CF == 0) break;
		CF = 0;
		t = 0x99;
		adc(ptr + i, t);
		CF = CF ^ 1;
	}
	for(i = m1 - 1; i >= 0; i--) {
		if(*(ptr + i) != 0) break;
	}
	if(i < 0) n1 = 0;
	else if(*(ptr + i) > 0x9) n1 = (i + 1) * 2;
	else n1 = i * 2 + 1;
	*(ptr + m1) = 0;
	*(int*)ret = n1 * mul;
	return ret;
}

char *_csuma(char *s1, char *s2) {
	int n1 = *(int*)s1, m1;
	int n2 = *(int*)s2, t, m2;
	s1 += 4;
	s2 += 4;
	char *tc;

	if (n1 < n2) {
		t = n1;
		n1 = n2;
		n2 = t;

		tc = s1;
		s1 = s2;
		s2 = tc;
	}

	char *ret, *ptr;
	ret = malloc((n1 + 2) / 2 + 5);
	ptr = ret + 4;

	m1 = (n1 + 1) / 2;
	m2 = (n2 + 1) / 2;

	memcpy(ptr, s1, m1);

	int i;
	for(i = 0; i < m2; i++) {
		adc(ptr + i, *(s2 + i));
	}
	for(i = m2; i < m1; i++) {
		adc(ptr + i, 0);
	}
	if (CF) {
		adc(ptr + m1, 0);
		n1++;
	}
	*(ptr + n1) = 0;
	*(int*)ret = n1;
	return ret;
}

char *csuma(char *s1, char *s2) {
	int n1 = *(int*)s1;
	int n2 = *(int*)s2;
	char *ret;
	if(n1 >= 0) {
		if(n2 >= 0) return _csuma(s1, s2);
		*(int*)s2 *= -1;
		ret = _croznica(s1, s2);
		*(int*)s2 *= -1;
		return ret;
	}
	*(int*)s1 *= -1;
	if(n2 >= 0) {
		ret = _croznica(s1, s2);
		*(int*)s1 *= -1;
		*(int*)ret *= -1;
		return ret;
	}
	*(int*)s2 *= -1;
	ret = _csuma(s1,s2);
	*(int*)ret *= -1;
	*(int*)s2 *= -1;
	*(int*)s1 *= -1;
	return ret;
}

char *croznica(char *s1, char *s2) {
	int n1 = *(int*)s1;
	int n2 = *(int*)s2;
	char *ret;
	if(n1 >= 0) {
		if(n2 >= 0) return _croznica(s1, s2);
		*(int*)s2 *= -1;
		ret = _csuma(s1, s2);
		*(int*)s2 *= -1;
		return ret;
	}
	if (n2 >= 0) {
		*(int*)s1 *= -1;
		ret = _csuma(s1, s2);
		*(int*)ret *= -1;
		*(int*)s1 *= -1;
		return ret;
	}
	*(int*)s1 *= -1;
	*(int*)s2 *= -1;
	ret = _croznica(s1, s2);
	*(int*)ret *= -1;
	*(int*)s1 *= -1;
	*(int*)s2 *= -1;
	return ret;
}

int main() {
	int bl;
	char *a, *b, *c, *d, *e;
	a = parse("10000");
	b = parse("9999");
	assert(strcmp("1", unparse(croznica(a,b))) == 0);

	a = parse("1");
	b = parse("1");
	assert(strcmp(unparse(croznica(a,b)), "0") == 0);
	a = parse("1000");
	b = parse("111");
	assert(strcmp(unparse(csuma(a,b)), "1111") == 0);

	a = parse("9999");
	b = parse("1");
	assert(strcmp(unparse(csuma(a,b)), "10000") == 0);

	a = parse("999");
	b = parse("9999");
	assert(strcmp(unparse(csuma(a,b)), "10998") == 0);

	a = parse("1");
	b = parse("99999999999999");
	assert(strcmp("100000000000000", unparse(csuma(a,b))) == 0);

	a = parse("123456");
	b = parse("11111");
	assert(strcmp("112345", unparse(croznica(a,b))) == 0);

	a = parse("23456789");
	b = parse("123456789");
	assert(strcmp("-100000000", unparse(croznica(a,b))) == 0);

	a = parse("543");
	b = parse("1000");
	assert(strcmp("-457", unparse(croznica(a,b))) == 0);

	a = parse("923");
	b = parse("923");
	assert(strcmp("0", unparse(croznica(a,b))) == 0);

	a = parse("-1234");
	assert(strcmp(unparse(a), "-1234") == 0);

	a = parse("-123");
	b = parse("-91");
	assert(strcmp("-32", unparse(croznica(a,b))) == 0);
	assert(strcmp("-214", unparse(csuma(a,b))) == 0);

	b = parse("91");
	assert(strcmp("-214", unparse(croznica(a,b))) == 0);
	assert(strcmp("-32", unparse(csuma(a,b))) == 0);

	a = parse("-65");
	assert(strcmp("-156", unparse(croznica(a,b))) == 0);
	assert(strcmp("26", unparse(csuma(a,b))) == 0);

	a = parse("65");
	b = parse("91");
	assert(strcmp("-26", unparse(croznica(a,b))) == 0);

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
	e = unparse(ciloczyn(csuma(a,b),croznica(c,d)));
	assert(strcmp(e, "-12285495673250776691") == 0);
	e = unparse(ciloczyn(ciloczyn(csuma(a,b),croznica(c,d)),csuma(a,b)));
	assert(strcmp(e, "-151419703608993903262112910119") == 0);
	e = unparse(ciloraz(ciloczyn(csuma(a,b),croznica(c,d)),csuma(a,b)));
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
