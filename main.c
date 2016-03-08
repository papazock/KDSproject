#include <omp.h>
#include "mygenerator.c"


#define MIN 12
#define MAX 30

bool checkColizion(Point p);


void doJob();

int main() {
    char *args[3];
    int pCount;
    int i=0;
    int count=0;
    args[1]="points.txt";
    args[2]="100";
    //write_to_file(3,args);
    Point *p=read_from_file(args[1],&pCount);
    printf("%d",pCount);
#pragma omp parallel num_threads(4) shared(p)
    {
#pragma omp for 
        for(i=0;i<pCount;i++){
            printf("Thread: %d read %d\n",omp_get_thread_num(),i);
            if(checkColizion(p[i])){
                count++;

            }
        }
    }
    printf("\n count: %d",count);
    return 0;
}

void doJob() {
    char *args[3];
    int pCount;
    int i=0;
    int count=0;
    args[1]="points.txt";
    args[2]="100";
    write_to_file(3,args);
    Point *p=read_from_file(args[1],&pCount);
    printf("%d",pCount);
    for(i=0;i<pCount;i++){
        if(checkColizion(p[i])){
            count++;
        }
    }
    printf("\ncount: %d",count);
}

bool checkColizion(Point p) {
    if(p.x<MIN||p.x>MAX) return false;
    if(p.y<MIN||p.y>MAX) return false;
    if(p.z<MIN||p.z>MAX) return false;

    return true;
}

