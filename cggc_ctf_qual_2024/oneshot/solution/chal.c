#include <stdio.h>
#include <unistd.h>

void main() {
    char *buf;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    printf("%p\n", printf);
    printf("> ");
    scanf("%p%*c", &buf);
    fgets(buf, 0xe0, stdin);
    puts("bye");
}