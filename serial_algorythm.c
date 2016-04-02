#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <bits/time.h>
#include <time.h>

#define MIN 12
#define MAX 30
#define BLOCK_SIZE 1000
#define OBJ_SIZE 3

struct timespec start, end;

const char invalid_number_of_args[] = "\n**********invalid number of Arguments**********\n";
const char valid_args[] = "\n**Arguments are OK!**\n";
int arg1;
int arg2;
FILE *fp;
char arg3[30];


bool checkColizion(float *p);


void doJob();

void validate_args_for_algorythm(int argc, char *argv[]);

void CulculateAndPrintTime();

long OpenFileAndGetPointCount(char *filename);

void getNextBlock(float *block);

void CloseFile();

int main(int argc, char **argv) {
    clock_gettime(CLOCK_MONOTONIC, &start);
    validate_args_for_algorythm(argc, argv);
    doJob();
    clock_gettime(CLOCK_MONOTONIC, &end);
    CulculateAndPrintTime();

}

void validate_args_for_algorythm(int argc, char *argv[]) {
    if (argc < 4) {
        printf(invalid_number_of_args);
        arg1 = -1;
        arg2 = -1;
        stpcpy(arg3, "dataPoints.bin");
    }
    if (argc >= 4) {
        printf(valid_args);
        arg1 = (int) strtol(argv[1], (char **) NULL, 10);
        arg2 = (int) strtol(argv[2], (char **) NULL, 10);
        stpcpy(arg3, argv[3]);
    }

}

void doJob() {
    long pCount = OpenFileAndGetPointCount(arg3);
    int i = 0;
    int count = 0;
    float block[BLOCK_SIZE * OBJ_SIZE];

    while (pCount > 0) {
        getNextBlock(block);
        pCount = pCount - BLOCK_SIZE;
        int to = BLOCK_SIZE * OBJ_SIZE;

        if (pCount < 0) {
            to = (BLOCK_SIZE + pCount) * OBJ_SIZE;
        }

        for (i = 0; i < to; i = i + 3)
            if (checkColizion(&block[i])) {
                count++;
            }

    }
    CloseFile();
    printf("\ncount: %d", count);
}

bool checkColizion(float *p) {
    if (*(p) < MIN || *(p) > MAX) return false;
    if (*(p + 1) < MIN || *(p + 1) > MAX) return false;
    if (*(p + 2) < MIN || *(p + 2) > MAX) return false;
    return true;
}


void CulculateAndPrintTime() {
    const int DAS_NANO_SECONDS_IN_SEC = 1000000000;
    long timeElapsed_s = end.tv_sec - start.tv_sec;
    long timeElapsed_n = end.tv_nsec - start.tv_nsec;

    if (timeElapsed_n < 0) {
        timeElapsed_n = DAS_NANO_SECONDS_IN_SEC + timeElapsed_n;
        timeElapsed_s--;
    }
    printf("\nTime: %ld.%09ld secs \n", timeElapsed_s, timeElapsed_n);
}


long OpenFileAndGetPointCount(char *filename) {
    fp = fopen(filename, "rb");
    long pCount = 0;
    if (fp != NULL) {
        fread(&pCount, sizeof(long), 1, fp);
        printf("%ld\n", pCount);
    }
    return pCount;
}

void getNextBlock(float *block) {
    if (fp != NULL) {
        fread(block, sizeof(float), BLOCK_SIZE * OBJ_SIZE, fp);
    }
}

void CloseFile() {
    if (fp != NULL) {

        fflush(fp);
        fclose(fp);
    }
}
