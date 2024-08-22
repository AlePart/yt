#include <stdio.h>
#include <stdarg.h>
#include <string.h>

int sum(int count, ...) {
	va_list args;
	va_start(args, count);

	int sum = 0;
	for (int i = 0; i < count; i++) {
		sum += va_arg(args, int);
	}

	va_end(args);
	return sum;
}

void print_numbers(const char* format, ...) {
    va_list args;
    va_start(args, format);

    const char* p = format;
    int i = 0;
    while (*p != '\0') {
        if (*p == 'd') {
            int num = va_arg(args, int);
            printf("%d ", num);
        }
        else if (*p == 'f') {
            double num = va_arg(args, double);
            printf("%f ", num);
        }
        else if (*p == 'c') {
            char ch = (char)va_arg(args, int); // va_arg promuove i char a int
            printf("%c ", ch);
        }
        p++;
        printf("%d\n", ++i);
    }

    va_end(args);
    printf("\n");
}


typedef struct {
    void (*func)(int); 
} Functor;

typedef void (*FunctorFunc)(int);

void print_square(int n) {
    printf("Square of %d is %d\n", n, n * n);
}

void print_cube(int n) {
	printf("Cube of %d is %d\n", n, n * n * n);
}

void print_quadruple(int n) {
	printf("Quadruple of %d is %d\n", n, n * n * n * n);
}


void use_functor(Functor f, int value) {
    f.func(value);
}



int main() {



    printf("Sum: %d\n", sum(3, 1, 2, 3));
    print_numbers("dfc\0", 42, 3.14, 'A');

  
    Functor f;
    f.func = print_square; 

    use_functor(f, 5); 

    f.func = print_cube;
    use_functor(f, 5);

    f.func = print_quadruple;
    use_functor(f, 5);

    FunctorFunc f2 = print_square;

    f2(2);


    return 0;
}