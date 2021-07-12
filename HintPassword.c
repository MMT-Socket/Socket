#include <stdio.h>
#include <conio.h>
#pragma warning(disable : 4996)
int main() {
    char p[100];
    int i = 0;
    printf("Input : ");
    while ((p[i] = getch()) != 18 && i++ < 15) {
        if (i > 1) printf("\b\b");
        printf("* %d", i);
    }
    p[i] = '\0';
    printf("\nPass : %s", p);
    return 0;
}