#include <stdstring.h>

namespace
{
    const char CharConvArr[] = "0123456789ABCDEF";
}

void itoa(unsigned int input, char* output, unsigned int base)
{
	int i = 0;

	while (input > 0)
	{
		output[i] = CharConvArr[input % base];
		input /= base;
		i++;
	}

    if (i == 0)
    {
        output[i] = CharConvArr[0];
        i++;
    }

	output[i] = '\0';
	i--;

	for (int j = 0; j <= i/2; j++)
	{
		char c = output[i - j];
		output[i - j] = output[j];
		output[j] = c;
	}
}

static unsigned int pow_int(unsigned int base, unsigned int exp)
{
    unsigned int res = 1;
    while (exp > 0)
    {
        res *= base;
        exp--;
    }
    return res;
}

void ftoa(float input, char* output, int precision)
{
    char* ptr = output;

    // 1. Handle Sign
    if (input < 0)
    {
        *ptr++ = '-';
        input = -input;
    }

    // 2. Handle Rounding
    // We add 0.5 / 10^precision to round to nearest digit
    unsigned int multiplier = pow_int(10, precision);
    input += (0.5f / multiplier);

    // 3. Process Integer Part
    unsigned int intPart = (unsigned int)input;
    float floatPart = input - intPart;

    // Use a temp buffer because your itoa writes from index 0
    char tempBuf[32]; 
    itoa(intPart, tempBuf, 10);

    // Copy integer part to output
    char* t = tempBuf;
    while (*t) *ptr++ = *t++;

    // 4. Process Fractional Part
    if (precision > 0)
    {
        *ptr++ = '.';

        // Extract fractional digits as an integer
        // e.g., 0.05 * 100 = 5
        unsigned int fracInt = (unsigned int)(floatPart * multiplier);

        itoa(fracInt, tempBuf, 10);

        // 5. Handle Padding (Leading Zeros)
        // If precision is 3 and we have "5", we need "005"
        int len = strlen(tempBuf);
        for (int i = 0; i < precision - len; i++)
        {
            *ptr++ = '0';
        }

        // Copy fractional digits
        t = tempBuf;
        while (*t) *ptr++ = *t++;
    }

    // Null terminate the final string
    *ptr = '\0';
}

int atoi(const char* input)
{
	int output = 0;

	while (*input != '\0')
	{
		output *= 10;
		if (*input > '9' || *input < '0')
			break;

		output += *input - '0';

		input++;
	}

	return output;
}

char* strncpy(char* dest, const char *src, int num)
{
	int i;

	for (i = 0; i < num && src[i] != '\0'; i++)
		dest[i] = src[i];
	for (; i < num; i++)
		dest[i] = '\0';

   return dest;
}

int strncmp(const char *s1, const char *s2, int num)
{
	unsigned char u1, u2;
  	while (num-- > 0)
    {
      	u1 = (unsigned char) *s1++;
     	u2 = (unsigned char) *s2++;
      	if (u1 != u2)
        	return u1 - u2;
      	if (u1 == '\0')
        	return 0;
    }

  	return 0;
}

int strlen(const char* s)
{
	int i = 0;

	while (s[i] != '\0')
		i++;

	return i;
}

void bzero(void* memory, int length)
{
	char* mem = reinterpret_cast<char*>(memory);

	for (int i = 0; i < length; i++)
		mem[i] = 0;
}

void memcpy(const void* src, void* dst, int num)
{
	const char* memsrc = reinterpret_cast<const char*>(src);
	char* memdst = reinterpret_cast<char*>(dst);

	for (int i = 0; i < num; i++)
		memdst[i] = memsrc[i];
}

void memset(void* memory, unsigned char value, int length)
{
	char* mem = reinterpret_cast<char*>(memory);

	for (int i = 0; i < length; i++)
		mem[i] = value;
}
