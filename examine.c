#include <omp.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define MIN 12
#define MAX 30
#define MAX_BLOCK_SIZE 1000
#define OBJ_SIZE 3
#define POINTS_PER_PROCCESSOR 500000

struct timespec start, end;
int arg1, arg2, arg4, arg5;
    //arg1: max Number of Colizions, arg2: max execution time, arg4: number of Threads, arg5: number of proccessors
char arg3[30];
//arg3: points file
const char invalid_number_of_args[] = "\n**********invalid number of Arguments**********\n";
const char valid_args[] = "\n**Arguments are OK!**\n";
FILE *fp;


bool checkColizion(float *p);

void doJob(int rank, int worldSize);

bool checkColizionLimmit(int cols);

bool checkExecutionTime();

void CulculateAndPrintTime();

long OpenFileAndGetPointCount(char *filename);

int getNextBlock(float *block, int BlockSize);

void CloseFile();

void validateArguments(int argc, char *argv[]);

float *floatdup(float *src, size_t len);




int main(int argc, char **argv) {
    int rank;
    int size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        validateArguments(argc, argv);
    }

    doJob(rank, size);
    if (rank == 0) {
        clock_gettime(CLOCK_MONOTONIC, &end);
        CulculateAndPrintTime();
    }
    MPI_Finalize();


}

void validateArguments(int argc, char *argv[]) {
    if (argc < 6) {
        printf(invalid_number_of_args);
        arg1 = -1;
        arg2 = -1;
        strcpy(arg3, "dataPoints.bin");
        arg4 = -1;
        arg5 = -1;
        return;
    }
    if (argc >= 6) {
        printf(valid_args);
        arg1 = (int) strtol(argv[1], (char **) NULL, 10);
        arg2 = (int) strtol(argv[2], (char **) NULL, 10);
        stpcpy(arg3, argv[3]);
        arg4 = (int) strtol(argv[4], (char **) NULL, 10);
        arg5 = (int) strtol(argv[5], (char **) NULL, 10);
    }

}

void doJob(int rank, int worldSize) {
    long pCount;
    int nBlocks = 0;
    int BlockSize = worldSize * POINTS_PER_PROCCESSOR;
    int ActualLocalBlockSize = POINTS_PER_PROCCESSOR * OBJ_SIZE;
    int globalCount[worldSize];
    int i = 0, k=0, z=0;
    int localCount = 0;
    int ThreadCount=0;
    float *block = malloc(sizeof(float) * BlockSize * OBJ_SIZE);
    float *localBlock = malloc(sizeof(float) * ActualLocalBlockSize);
    float *BlockRemaing=NULL;
    int remaining=0;

    if (rank == 0) {
        pCount = OpenFileAndGetPointCount(arg3);
        nBlocks = pCount / BlockSize;
        if (pCount % BlockSize != 0) {
            nBlocks++;
        }
        printf("Total points: %ld\n%d Blocks of %d points each\n", pCount, nBlocks, BlockSize);
    }
    MPI_Bcast(&nBlocks, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&pCount, 1, MPI_INT, 0, MPI_COMM_WORLD);


    for (k = 0; k < nBlocks; k++) {

        if (rank == 0) {
            printf("\nBlock %d from %d", k+1,nBlocks);
            getNextBlock(block, BlockSize);
           // printf("\nBlock %d ", k);
        }
        if (k == nBlocks - 1) {
            remaining =(int) (pCount - BlockSize * (nBlocks - 1));
            if (remaining != BlockSize) {
                BlockSize = remaining - (remaining % worldSize);
                remaining = remaining % worldSize;
                //printf("\nremaining: %d", BlockSize + remaining);
                MPI_Bcast(&BlockSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
                ActualLocalBlockSize = (BlockSize / worldSize) * OBJ_SIZE;
                //printf("BlockSize:%d\n",BlockSize);
                if (rank == 0) {
                    //printf("\nremaining: %d\n", BlockSize);
                    float *tempBlock = floatdup(block, (size_t) BlockSize * OBJ_SIZE);
                    //  printf("size of temp %d",sizeof(tempBlock));
                    BlockRemaing = malloc(remaining * OBJ_SIZE * sizeof(float));
                    for (z = 0; z < remaining; z++) {
                        BlockRemaing[z] = block[BlockSize * OBJ_SIZE + z];
                    }
                    free(block);
                    block = &tempBlock[0];
                }

                localBlock = realloc(localBlock, ActualLocalBlockSize * sizeof(float));
                //printf("\n%d DONE\n", rank);
                //printf("rank: %d,Blocksize: %d, block-size: %d\n", rank, BlockSize, sizeof(block));
                MPI_Barrier(MPI_COMM_WORLD);

            }
            else {
                remaining = 0;
            }
        }

        MPI_Scatter(block, ActualLocalBlockSize, MPI_FLOAT, localBlock, ActualLocalBlockSize, MPI_FLOAT, 0,
                    MPI_COMM_WORLD);
    ThreadCount=0;

#pragma omp parallel
        {
          //  printf("rank: %d num Threads: %d\n",rank,omp_get_num_threads());
#pragma omp  for reduction(+:ThreadCount)
            for (i = 0; i < ActualLocalBlockSize; i = i + 3)
                if (checkColizion(&localBlock[i])) {
                    ThreadCount++;
                    // printf("rank: %d %f\n", rank, localBlock[i]);
                }
        }
        localCount=localCount+ThreadCount;
        MPI_Gather(&localCount, 1, MPI_INT, globalCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            int totalCount = 0;
            for (i = 0; i < worldSize; i++) {
                totalCount += globalCount[i];
            }
            if (checkColizionLimmit(totalCount)) {
                //Do something Like exit
            }
            //printf("\nTotalCount: %d\n", totalCount);
            if (checkExecutionTime()) {
                //Do something like exit
            }
        }


    }


    if (rank == 0) {
        CloseFile();

#pragma omp parallel for reduction(+:localCount)
        for (i = 0; i < remaining; i = i + 3)
            if (checkColizion(&BlockRemaing[i])) {
                localCount++;
                // printf("rank: %d %f\n", rank, localBlock[i]);
            }
    }
    MPI_Gather(&localCount, 1, MPI_INT, globalCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        int totalCount = 0;
        for (i = 0; i < worldSize; i++) {
            totalCount += globalCount[i];
        }
        printf("\nTotalCount: %d\n", totalCount);
    }


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

int getNextBlock(float *block, int BlockSize) {
    if (fp != NULL) {
        size_t ret = fread(block, sizeof(float), (size_t)BlockSize * OBJ_SIZE, fp);
        return (int) (ret / 3);
    }
    return 0;
}

void CloseFile() {
    if (fp != NULL) {

        fflush(fp);
        fclose(fp);
    }
}

float *floatdup(float *src, size_t len) {

    float *p = malloc(len * sizeof(float));
    if (p != NULL) {
        memcpy(p, src, len * sizeof(float));
    }
    else {
        printf("cant allocate memmory");
    }
    return p;
}

bool checkColizionLimmit(int cols) {
    if (arg1 == -1) {
        return false;
    }
    return arg1 <= cols;
}

bool checkExecutionTime() {

    if (arg2 == -1) {
        return false;
    }

    struct timespec temp;
    clock_gettime(CLOCK_MONOTONIC, &temp);
    const int DAS_NANO_SECONDS_IN_SEC = 1000000000;
    long timeElapsed_s = temp.tv_sec - start.tv_sec;
    long timeElapsed_n = temp.tv_nsec - start.tv_nsec;

    if (timeElapsed_n < 0) {
        timeElapsed_n = DAS_NANO_SECONDS_IN_SEC + timeElapsed_n;
        timeElapsed_s--;
    }

    long tempElapsed = timeElapsed_s + timeElapsed_n;
    return tempElapsed >= arg2;

    //printf("\nTime: %ld.%09ld secs \n", timeElapsed_s, timeElapsed_n);
}