#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern char* parse(char*);
extern char* unparse(char*);
extern char* suma(char*, char*);
extern char aless(char*, char*);
extern char* roznica(char*, char*);
extern char* aadd_into(char*, char*, int);
extern char* asub(char*, char*, int);

extern char* cparse(char*);
extern char* unparse(char*);

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
	if(aless(s1, s2)) {
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
		aadd_into(bcc[1], bcc[0], m2 + 1);
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
			asub(cm, bcc[1], m2 + 1);
		}
		*ptr += '0';
		ptr++;

		while(!less3(cm, bcc[0], m2 + 1)) {
			(*ptr)++;
			asub(cm, bcc[0], m2 + 1);
		}
		*ptr += '0';
		ptr++;
	}
	*ptr = 0;
	for(ptr = ret; *ptr == '0'; ptr++);
	ptr = parse(ptr);
	*(int*)ptr *= mul;
	//free(ret);

	//free(bcc[0]);
	//free(bcc[1]);
	//free(bcc);
	return ptr;
}

char *ciloczyn(char *s1, char *s2) {
	//printf("%s %s\n", unparse(s1), unparse(s2));
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

	if(aless(s1, s2)) {
		t = n1;
		n1 = n2;
		n2 = t;
		tc = s1;
		s1 = s2;
		s2 = tc;
	}
	if(n2 == 1 && *(s2 + 4) == 0) {
		return parse("0");
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
	n1 += 2;
	for(i = 1; i < 10; i++) {
		acc[i] = malloc(n1);
		memcpy(acc[i], acc[i - 1], n1);
		aadd_into(acc[i], s1, n1);
	}
	for(i = 10; i < 18; i++) {
		acc[i] = malloc(n1);
		memcpy(acc[i], acc[i - 1], n1);
		aadd_into(acc[i], acc[9], n1);
	}

	for(i = 0; i < n2; i++) {
		c = s2[i] & 0xf;
		if(c > 0) {
			aadd_into(ptr, acc[c - 1], n1);
		}
		c = (s2[i] >> 4) & 0xf;
		if(c > 0) {
			aadd_into(ptr, acc[c - 1 + 9], n1);
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

	for(i = 0; i < 18; i++) {
		//free(acc[i]);
	}
	//free(acc);
	return ret;
}

