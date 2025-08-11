#include "kernel/types.h"
#include "user/user.h"

// Mark function as noreturn to prevent compiler recursion warning
void primes(int input_fd) __attribute__((noreturn));

void primes(int input_fd)
{
    int prime;
    int n;

    // Read the first number, which is a prime
    if (read(input_fd, &prime, sizeof(prime)) != sizeof(prime)) {
        close(input_fd);
        exit(0); // No more data
    }

    printf("prime %d\n", prime);

    // If read succeeded, there might be more numbers
    int p[2];
    if (pipe(p) < 0) {
        fprintf(2, "primes: pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "primes: fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // Child: continues the pipeline
        close(input_fd);
        close(p[1]); // Close write end
        primes(p[0]); // Recurse with new input
    } else {
        // Parent: filters out multiples of prime and writes to pipe
        close(p[0]); // Close read end

        while (read(input_fd, &n, sizeof(n)) == sizeof(n)) {
            if (n % prime != 0) {
                write(p[1], &n, sizeof(n));
            }
        }

        // Done sending filtered numbers
        close(p[1]); // Close write end to signal EOF
        close(input_fd);

        // Wait for child and all descendants
        wait(0);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    int p[2];

    if (pipe(p) < 0) {
        fprintf(2, "primes: pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "primes: fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // Child runs the sieve
        close(p[1]); // Close write end
        primes(p[0]);
    } else {
        // Parent feeds numbers 2 to 280 into the pipeline
        close(p[0]); // Close read end

        for (int i = 2; i <= 280; i++) {
            write(p[1], &i, sizeof(i));
        }

        close(p[1]); // Done feeding

        // Wait for the entire pipeline to complete
        wait(0);
        exit(0);
    }

    return 0;
}