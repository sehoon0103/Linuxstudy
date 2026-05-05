#include <stdio.h>
#include <stdlib.h>

int factorial(int n) {
    int ret = 1;
    if (n > 1) {
        ret *= n;
        ret *= factorial(n - 1);
    }
    return ret;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("%s <number>\n", argv[0]);
        return -1;
    }

    int i = atoi(argv[1]);
    int result = factorial(i);
    printf("factorial of %d = %d\n", i, result);

    return 0;
}
