#include <stdio.h>
#include <stdlib.h>
#include "int_to_alpha.h"

char* int_to_alpha(int num) {
	int digits = 1, div = 1;
	int isNeg = 0;
	char* numStr = NULL;

	while (num / div >= 9 || num / div <= -9) {
		digits++;
		div *= 10;
	}

	if (num < 0)
		isNeg = 1;

	numStr = malloc(digits + isNeg + 1);
	if (numStr == NULL) return NULL;

	numStr[digits + isNeg] = '\0';

	while (digits > 0) {
		--digits;
		if (isNeg)
			numStr[digits + 1] = -(num % 10) + '0';
		else
			numStr[digits] = num % 10 + '0';
		num /= 10;
	}
	if (isNeg)
		numStr[0] = '-';
	return numStr;
}
