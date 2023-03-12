/* firestarter.c
 * David Joiner
 * Usage: Fire [forestSize(20)] [numTrials(5000)] * [numProbabilities(101)]
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define UNBURNT 0
#define SMOLDERING 1
#define BURNING 2
#define BURNT 3
#define NUMBER_THREADS 8
#define VERSION 2

#define true 1
#define false 0
typedef int boolean;

extern void seed_by_time(int);
extern int ** allocate_forest(int);
extern void initialize_forest(int, int **);
extern double get_percent_burned(int, int **);
extern void delete_forest(int, int **);
extern void light_tree(int, int **,int,int);
extern boolean forest_is_burning(int, int **);
extern void forest_burns(int, int **,double);
extern int burn_until_out(int,int **,double,int,int);
extern void print_forest(int, int **);

void printData();

void *printMessage(void *arg) {
    printf("%s\n", (char *)arg);

    return(NULL);
}
int n_trials=5000,
    n_probs=101,
    forest_size=35,
	chunk=1;
double prob_min=0.0,
       prob_max=1.0,
       * prob_spread,
       * percent_burned;
pthread_mutex_t mut;

int currentRow = 0;
void *calculateBurnSecondEdition(void *arg){
    int **forest;
    int i_trial;
    int local_i_prob;
    double local_percent_burned;
    int thread_id = (int) arg;

    // setup problem
    seed_by_time(0);
    forest=allocate_forest(forest_size);

    while (true){
        pthread_mutex_lock(&mut);
        currentRow += chunk;
        local_i_prob = currentRow;
        //printf("Thread %d has num cuclulation for local index propability %d\n",thread_id, local_i_prob);
        pthread_mutex_unlock(&mut);
        if (local_i_prob >= n_probs) {
            printf("Thread %d out of RANGE  %d is INVALID\n",thread_id, local_i_prob);
            printf("Thread %d finished\n",thread_id);
            // clean up
            delete_forest(forest_size,forest);
            return NULL;
        }
        local_percent_burned = 0.0;
        for (i_trial=0; i_trial < n_trials; i_trial++) {
            //burn until fire is gone
            burn_until_out(forest_size,forest,prob_spread[local_i_prob], forest_size/2,forest_size/2);
            local_percent_burned+=get_percent_burned(forest_size,forest);
        }
        pthread_mutex_lock(&mut);
        percent_burned[local_i_prob]=local_percent_burned/n_trials;
        pthread_mutex_unlock(&mut);
    }

}


int main(int argc, char ** argv) {
    // initial conditions and variable definitions
        double time=0.0;
        clock_t begin=clock();
        int i, i_prob;
        double prob_step;
        pthread_t threads[NUMBER_THREADS];
        // check command line arguments

        if (argc > 1) {
            sscanf(argv[1], "%d", &forest_size);
        }
        if (argc > 2) {
            sscanf(argv[2], "%d", &n_trials);
        }
        if (argc > 3) {
            sscanf(argv[3], "%d", &n_probs);
        }
        prob_spread = (double *) malloc(n_probs * sizeof(double));
        percent_burned = (double *) malloc(n_probs * sizeof(double));

        //for a number of trials, calculate average
        //percent burn
        prob_step = (prob_max - prob_min) / (double) (n_probs - 1);
        for (i_prob = 0; i_prob < n_probs; i_prob++) {
            prob_spread[i_prob] = prob_min + (double) i_prob * prob_step;
        }

        printf("Probability of fire spreading, Average percent burned\n");

        for (i = 0; i < NUMBER_THREADS; ++i) {
            pthread_create(&threads[i], NULL, calculateBurnSecondEdition, (void *) i);
        }

        for (i = 0; i < NUMBER_THREADS; ++i) {
            pthread_join(threads[i], NULL);
        }

        for (i_prob = 0; i_prob < n_probs; i_prob++) {
            printf("%lf , %lf\n", prob_spread[i_prob], percent_burned[i_prob]);
        }
        clock_t end=clock();
    // calculate elapsed time by finding difference (end - begin) and
    // dividing the difference by CLOCKS_PER_SEC to convert to seconds
    time += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("--------------%dnd Version-----------------------\n",VERSION);
    printf("-------------- %d Threads -----------------------\n", NUMBER_THREADS);
    printf("-----The elapsed time is %f seconds-------\n", time);
    printf("---------------forest_size=%d-------------------\n",forest_size);
    return 0;
}

#include <time.h>

void seed_by_time(int offset) {
    time_t the_time;
    time(&the_time);
    srand((int)the_time+offset);
}

int burn_until_out(int forest_size,int ** forest, double prob_spread, int start_i, int start_j) {
    int count;

    initialize_forest(forest_size,forest);
    light_tree(forest_size,forest,start_i,start_j);

    // burn until fire is gone
    count = 0;
    while(forest_is_burning(forest_size,forest)) {
        forest_burns(forest_size,forest,prob_spread);
        count++;
    }

    return count;
}

double get_percent_burned(int forest_size,int ** forest) {
    int i,j;
    int total = forest_size*forest_size-1;
    int sum=0;

    // calculate percent burned
    for (i=0;i<forest_size;i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNT) {
                sum++;
            }
        }
    }

    // return percent burned;
    return ((double)(sum-1)/(double)total);
}


int ** allocate_forest(int forest_size) {
    int i;
    int ** forest;

    forest = (int **) malloc (sizeof(int*)*forest_size);
    for (i=0;i<forest_size;i++) {
        forest[i] = (int *) malloc (sizeof(int)*forest_size);
    }

    return forest;
}

void initialize_forest(int forest_size, int ** forest) {
    int i,j;

    for (i=0;i<forest_size;i++) {
        for (j=0;j<forest_size;j++) {
            forest[i][j]=UNBURNT;
        }
    }
}

void delete_forest(int forest_size, int ** forest) {
    int i;

    for (i=0;i<forest_size;i++) {
        free(forest[i]);
    }
    free(forest);
}

void light_tree(int forest_size, int ** forest, int i, int j) {
    forest[i][j]=SMOLDERING;
}

boolean fire_spreads(double prob_spread) {
    if ((double)rand()/(double)RAND_MAX < prob_spread)
        return true;
    else
        return false;
}

void forest_burns(int forest_size, int **forest,double prob_spread) {
    int i,j;
    extern boolean fire_spreads(double);

    //burning trees burn down, smoldering trees ignite
    for (i=0; i<forest_size; i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNING) forest[i][j]=BURNT;
            if (forest[i][j]==SMOLDERING) forest[i][j]=BURNING;
        }
    }

    //unburnt trees catch fire
    for (i=0; i<forest_size; i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNING) {
                if (i!=0) { // North
                    if (fire_spreads(prob_spread)&&forest[i-1][j]==UNBURNT) {
                        forest[i-1][j]=SMOLDERING;
                    }
                }
                if (i!=forest_size-1) { //South
                    if (fire_spreads(prob_spread)&&forest[i+1][j]==UNBURNT) {
                        forest[i+1][j]=SMOLDERING;
                    }
                }
                if (j!=0) { // West
                    if (fire_spreads(prob_spread)&&forest[i][j-1]==UNBURNT) {
                        forest[i][j-1]=SMOLDERING;
                    }
                }
                if (j!=forest_size-1) { // East
                    if (fire_spreads(prob_spread)&&forest[i][j+1]==UNBURNT) {
                        forest[i][j+1]=SMOLDERING;
                    }
                }
            }
        }
    }
}

boolean forest_is_burning(int forest_size, int ** forest) {
    int i,j;

    for (i=0; i<forest_size; i++) {
        for (j=0; j<forest_size; j++) {
            if (forest[i][j]==SMOLDERING||forest[i][j]==BURNING) {
                return true;
            }
        }
    }
    return false;
}

void print_forest(int forest_size,int ** forest) {
    int i,j;

    for (i=0;i<forest_size;i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNT) {
                printf(".");
            } else {
                printf("X");
            }
        }
        printf("\n");
    }
}

