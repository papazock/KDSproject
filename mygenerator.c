//
// Created by chris on 4/3/2016.
//

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct{
    float x;
    float y;
    float z;
}Point;

char *filename;
long coordinate_index;

bool validate_args(int argc, char **argv);

void write_to_file(int argc, char **argv);

Point *read_from_file(char *filename,int *count);


Point *read_from_file(char *filename, int *pCount) {
    FILE *fp = fopen(filename, "r");
    int i = 0;
    float x,y,z;
    Point *points;
    if (fp != NULL) {

        fscanf (fp, "%d", &i);
        points=(Point *)malloc(sizeof(Point)*i);
        *pCount=i;
        i=0;
        while (!feof (fp))
        {
            fscanf (fp, "%f\t%f\t%f\n", &x,&y,&z);
            points[i].x=x;
            points[i].y=y;
            points[i].z=z;
            i++;
        }
        if(fp!=NULL){

            fflush(fp);
            fclose (fp);
        }

        return points;
    } else {
        perror("Error");
    }
    return NULL;

}

void write_to_file(int argc, char **argv) {
    filename=(char *)malloc(sizeof(char)*25);
    if (validate_args(argc, argv)) {
        strcpy(filename, argv[1]);
        coordinate_index = strtol(argv[2], (char **) NULL, 10);
    }
    int utime;
    long int ltime;
    int i;

    ltime = time(NULL);
    utime = (unsigned int) ltime / 2;
    srand(utime);

    FILE *fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, "%ld\n", coordinate_index);
        for (i = 0; i < coordinate_index; i++) {
            float x = (float) 34 * rand() / (RAND_MAX - 1);
            float y = (float) 34 * rand() / (RAND_MAX - 1);
            float z = (float) 34 * rand() / (RAND_MAX - 1);
            fprintf(fp, "%f\t%f\t%f\n", x, y, z);
        }
        fflush(fp);
        fclose(fp);
    } else {
        perror("Error");
    }
}

bool validate_args(int argc, char **argv) {
    if (argc != 3) {
        printf("\nProgram needs 2 arguments. arg1: #data_Points, arg2: output_file_name\nargs set to default\n");
        strcpy(filename, "dataPoints.txt");
        coordinate_index = 10000;
        return false;
    } else {
        bool temp = true;
        if (strtol(argv[2], (char **) NULL, 10) <= 0) {
            printf("\n #data_Points must be >0, set to default: 10000\n");
            coordinate_index = 10000;
            temp = false;
            //strcpy(filename, argv[2]);
        }
        if (strcmp(argv[1], " ")==0) {
            printf("\n file name must not be empty, set to default: dataPoints.txt\n");
            //strcpy(filename , "dataPoints.txt");
            if (!temp) {
                coordinate_index = 10000;
            }
            temp = false;
        }
        return temp;
    }
}