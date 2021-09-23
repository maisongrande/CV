#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "challenges.h"

int main(int argc, char **argv)
{
        clock_t start, end, part_start, part_end;

        start = clock();

        for (int i = 0; i < LENGTH(answers); i++) {
                part_start = clock();
                printf("Answer %3d %1s %-15ld", i+1, "=   ", answers[i]());
                part_end = clock();
                printf("cpu_t: %fs.\n",
                       ((double)(part_end-part_start)) / CLOCKS_PER_SEC);
        }
        end = clock();

        printf("%sTOTAL:%31s %fs.\n\n",
               "------------------------------------------------\n",
               "cpu_t:",
               ((double)(end-start)) / CLOCKS_PER_SEC);

        return EXIT_SUCCESS;
}
