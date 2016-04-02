#include "mygenerator.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

FILE *fp;


int main(int argc, char **argv) {
    validate_args_for_generator(argc, argv);
    printf("\n*****Start creating file: %s*****\n", filename);
    write_to_file(coordinate_index, filename);
    printf("\n*****File %s successfully created!*****\n", filename);
    OpenFileAndGetPointCount(filename);
    return 0;
}





void write_to_file(long k, char *f) {
    int utime;
    long int ltime;
    int i;
    int hasToWrite = 0;

    ltime = time(NULL);
    utime = (unsigned int) ltime / 2;
    srand(utime);
    int tenPercent = 0;
    float block[BLOCK_SIZE * OBJ_SIZE];


    FILE *fp = fopen(f, "wb");
    if (fp != NULL) {
        fwrite(&k, sizeof(long), 1, fp);
        for (i = 0; i < k * OBJ_SIZE; i++) {
            if (i % (BLOCK_SIZE * OBJ_SIZE) == 0 && i != 0) {
                fwrite(block, sizeof(float), BLOCK_SIZE * OBJ_SIZE, fp);
                printf("\t**%.2f/100 created**\n", tenPercent / (1.0 * k / BLOCK_SIZE) * 100);
                tenPercent++;
                memset(block, 0, sizeof(block));
                hasToWrite = 0;
            }

            block[i % (BLOCK_SIZE * OBJ_SIZE)] = (float) 34 * rand() / (RAND_MAX - 1);
            if (i != k * OBJ_SIZE - 1) {
                hasToWrite = 1;
            }
            //printf("%f\n",block[i%(BLOCK_SIZE*OBJ_SIZE)]);


        }
        if (hasToWrite == 1) {
            fwrite(block, sizeof(float), BLOCK_SIZE * OBJ_SIZE, fp);
        }
        printf("\t**100.0/100 created**\n");
        fflush(fp);
        fclose(fp);
    } else {
        perror("Error");
    }
}

void validate_args_for_generator(int argc, char **argv) {
    if (argc != 3) {
        printf("\nProgram needs 2 arguments. arg1: #data_Points, arg2: output_file_name\nargs set to defaults\n");
        strcpy(filename, "dataPoints.bin");
        coordinate_index = 1000000;
        return;
    } else {
        coordinate_index = strtol(argv[1], (char **) NULL, 10);
        strcpy(filename, argv[2]);
    }
    return;
}

long OpenFileAndGetPointCount(char* filename){
    FILE *fp = fopen(filename, "rb");
    long pCount;
    if (fp != NULL) {
        fread(&pCount, sizeof(long), 1, fp);
        float block[BLOCK_SIZE * OBJ_SIZE];
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
