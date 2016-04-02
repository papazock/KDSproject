#ifndef KDSPROJECT_MYGENERATOR_H
#define KDSPROJECT_MYGENERATOR_H

#define BLOCK_SIZE 1000
#define OBJ_SIZE 3
typedef struct {
    float x;
    float y;
    float z;
} Point;


char filename[50];
long coordinate_index;

void validate_args_for_generator(int argc, char **argv);

void write_to_file(long k, char *f);

long OpenFileAndGetPointCount(char* filename);

void getNextBlock(float* block);

void CloseFile();
#endif //KDSPROJECT_MYGENERATOR_H
