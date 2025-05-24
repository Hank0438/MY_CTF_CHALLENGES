#include <stdio.h>

void GetString() {
	char tempBuffer[0x10];
	printf("try to input something: ");
	fgets(tempBuffer, 0x100, stdin);
}

int main() {
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);
	GetString();
	return 0;
}