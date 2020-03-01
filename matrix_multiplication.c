#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

typedef struct {
    int n;
    bool *a;
    bool *b;
    bool *c;
} matrices_struct;

int freerow = 0;
pthread_mutex_t rowmutex = PTHREAD_MUTEX_INITIALIZER;

void fillMatrix(int n, bool x[n][n]){
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            x[i][j] = rand() & 1;
        }
    }
}

void showMatrix(int n, bool x[n][n]){
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            printf("%d ", x[i][j]);
        }
        printf("\n");
    }
}

void *fillRow(void *args){
    int i;
    matrices_struct *ms = args;
    while (freerow < ms->n){
        pthread_mutex_lock(&rowmutex);
            if(freerow >= ms->n){
                pthread_mutex_unlock(&rowmutex);
                break;
            }
            i = freerow;
	        freerow++;
	    pthread_mutex_unlock(&rowmutex);

        for(int j = 0; j < ms->n; j++){
            bool finish = false;
            for(int m = 0; m < ms->n; m++){
                finish = finish || (*(ms->a + i*ms->n + m) && *(ms->b + m*ms->n + j));
                if(finish)      
                    break;
            }
            *(ms->c + i*ms->n + j) = finish;
        }
    }
}

void multiplyMatrix(int n, int numOfThreads, bool *a, bool *b, bool *c){
    pthread_t threads[numOfThreads];
    matrices_struct *ms = malloc(sizeof(*ms));
    ms->n = n;
    ms->a = a;
    ms->b = b;
    ms->c = c;

    pthread_mutex_lock(&rowmutex);
    for(int i = 0; i < numOfThreads; i++){ 
        if ( pthread_create( &threads[i], NULL, fillRow, ms) ) {
	        printf("błąd przy tworzeniu wątka.");
	        abort();
	    }
        //printf("%ld \n", threads[i]); threads id
    } 
    pthread_mutex_unlock(&rowmutex);
    for(int i = 0; i < numOfThreads; i++){
        if ( pthread_join ( threads[i], NULL ) ) {
		    printf("błąd przy kończeniu wątka.");
		    abort();
	    }
    } 
}

int main(){
    srand(time(NULL));
    int size;
    int numOfThreads;
    scanf("%d", &size);
    scanf("%d", &numOfThreads);

    bool a[size][size];
    bool b[size][size];
    bool c[size][size];

    fillMatrix(size, a);
    fillMatrix(size, b);
    
    showMatrix(size, a);
    printf("\n");

    showMatrix(size, b);
    printf("\n");

    printf("\n");
    printf("\n");
    multiplyMatrix(size, numOfThreads, *a, *b, *c);
    printf("\n");
    printf("\n");

    showMatrix(size, c);
    printf("%d \n", freerow);

}
