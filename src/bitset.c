/*
    Вспомогательные функции для работы со множествами
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bitset.h>

/*
    Полезные макросы
*/
#if __SIZEOF_LONG__ == 4
  #define BITSET_SHIFT 5
  #define BITSET_MASK 0x001F
#elif __SIZEOF_LONG__ == 8
  #define BITSET_SHIFT 6
  #define BITSET_MASK 0x003F
#else
# error Unsupported architecture!
#endif

#define BITSET_ROW(x) (((unsigned long)x)>>BITSET_SHIFT)
//#define BITSET_COL(x) (1UL<<((((unsigned long)x)&BITSET_MASK) - 1))
#define BITSET_COL(x) (1UL<<((((unsigned long)x)&BITSET_MASK)))

#define BITSET_SIZE (1024)
#define BITSET_BYTE_SIZE ((BITSET_SIZE>>3)+ __SIZEOF_LONG__)
#define BITSET_LONG_SIZE ((BITSET_SIZE>>BITSET_SHIFT)+1)

unsigned long *balloc(void)
{
    return memset(malloc(BITSET_BYTE_SIZE), '\0', BITSET_BYTE_SIZE);
}

unsigned long *bits_from_array(unsigned long *a)
{
	int i;
	unsigned long *result = balloc();

	for(i = 0; a[i] != BITSET_LAST; i++)
	{
		bits_append(a[i], result);
	}
	return result;
}

int bits_belong(long element, unsigned long *set)
{
    if (set == 0)
        return 0;

    if (element < 0)
        return 0;

	if (set[BITSET_ROW(element)]&BITSET_COL(element))
        return 1;
	else
        return 0;
}

void bits_append(unsigned long element, unsigned long *set)
{
	set[BITSET_ROW(element)] = set[BITSET_ROW(element)]|BITSET_COL(element);
}

void bits_disjunct(unsigned long *set1, unsigned long *set2, unsigned long *res)
{
	int i;
	for (i = 0; i < BITSET_LONG_SIZE; i++)
        res[i] = set1[i]|(set2[i]);

	//free(set1);
	//free(set2);
//	set_print(res);
}

void bits_conjunct(unsigned long *set1, unsigned long *set2, unsigned long *res)
{
	int i;
	for (i = 0; i < BITSET_LONG_SIZE; i++)
        res[i] = set1[i]&set2[i];

	//free(set1);
	//free(set2);
//	set_print(res);
}

void bits_sub(unsigned long *set1, unsigned long *set2, unsigned long *res)
{
	int i;
	for (i = 0; i < BITSET_LONG_SIZE; i++)
        res[i] = set1[i]&(~set2[i]);

	//free(set1);
	//free(set2);
//	set_print(res);
}

void set_print(unsigned long *res)
{
	int i;
	printf("[ ");
	for(i = 0; i < BITSET_LONG_SIZE; i++)
		if(bits_belong(i, res)) printf("%d,", i);
	printf(" ]\n");
}

unsigned long bits_first(unsigned long *res)
{
    unsigned long i;

    if (res == 0)
        return BITSET_LAST;

    for (i = 0; i < BITSET_LAST; i++)
        if (bits_belong(i, res))
            return i;

    return BITSET_LAST;
}
