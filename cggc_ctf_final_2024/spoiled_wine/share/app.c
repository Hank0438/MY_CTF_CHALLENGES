#include <stdio.h>

// FILE *fp;

void GetString() {
	char tempBuffer[0x40];

	// fp = fopen("NUL", "wb");

	printf("try to input something: ");
	fgets(tempBuffer, 0x48, stdin);
	fprintf(stdout, tempBuffer);
	// printf(tempBuffer);
}

int main() {
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);
	GetString();
	return 0;
}