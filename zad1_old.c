#include <stdlib.h>
#include <string.h>

char CF, AF;

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
		ptr = malloc(2);
		*ptr = '0';
		*(ptr + 1) = 0;
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


char *_croznica(char *s1, char *s2) {
	char *tc;
	int mul = 1;
	if(aless(s1, s2)) {
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
	char *ret = malloc(m1 + 4 + 1);
	char *ptr = ret + 4;
	memcpy(ptr, s1, m1);

	m2 = (n2 + 1) / 2;

	char t;
	for(i = 0; i < m2; i++, ptr++, s2++) {
		t = *(s2);
		adc(&t, 0);
		if(t == 0) continue;
		t = 0x99 - t;
		adc(&t, 1);
		adc(ptr, t);
		CF = CF ^ 1;
	}
	for(i = m2; i < m1; i++, ptr++) {
		if(CF == 0) break;
		CF = 0;
		t = 0x99;
		adc(ptr, t);
		CF = CF ^ 1;
	}
	ptr = ret + 4;
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

	// edi := s1
	// esi := s2
	// eax := n1
	// ebx := n2
	// ecx := t/tc

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
	// eax := ret
	// edi := ptr
	// esi := s1
	// ebx := m1
	ptr = ret + 4;

	m1 = (n1 + 1) / 2;
	// Żywe: n1, m1, m2, ptr, ret, i, s1, s2

	memcpy(ptr, s1, m1);

	m2 = (n2 + 1) / 2;

	int i;
	for(i = 0; i < m2; i++) {
		// ecx := i
		// ebx := m2
		// edi := ptr
		// esi := s2
		// free: eax, edx
		adc(ptr + i, *(s2 + i));
	}
	for(i = m2; i < m1; i++) {
		// ecx := i
		// ebx := m1
		// edi := ptr
		adc(ptr + i, 0);
	}
	if (CF) {
		// ebx := m1
		// edi := ptr
		// edx := n1
		adc(ptr + m1, 0);
		n1++;
	}
	*(ptr + n1) = 0; // błąd!
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
	*(int*)s1 *= -1;
	if (n2 >= 0) {
		ret = _csuma(s1, s2);
		*(int*)ret *= -1;
		*(int*)s1 *= -1;
		return ret;
	}
	*(int*)s2 *= -1;
	ret = _croznica(s1, s2);
	*(int*)s2 *= -1;
	*(int*)ret *= -1;
	*(int*)s1 *= -1;
	return ret;
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

void add_into(char *dest, char *src, int n) {
	int i;
	CF = 0;
	for(i = 0; i < n; i++) {
		adc(dest + i, src[i]);
	}
}