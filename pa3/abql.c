// gcc -pthread lock.c lock.s -lm -o lock -m32

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define BILLION 1000000000L

// typedef struct {
//     int lockdata;
// } simplelock;

typedef struct {
    // simplelock *lkp;
    int *pass;
} queuelock;

void abql_lock(queuelock *abql, int n) {
    // printf("before s_lock\n");
    // s_lock(&abql->lkp);
    // printf("after s_lock\n");
    // printf("currently_serving: %d\n", tl->currently_serving);
    // printf("myticketlock: %d\n", myticketlock);
    // s_unlock(&abql->lkp);
    // printf("lock: pid: %d, pass: %d\n", n, abql->pass[n]);
    while (!abql->pass[n]);
}

void abql_unlock(queuelock *abql, int n) {
    abql->pass[n] = 0;
    abql->pass[(n+1)%8] = 1;
    // printf("unlock: pid: %d, next: %d\n", n, (n+1)%8);
}

typedef struct {
    int pid, k, m, N, *a;
    queuelock *abql;
    struct timespec *starts, *ends;
} GM;

// void tts_lock_acquire(simplelock *lkp){
//     s_lock(&lkp);
//     return;
// }

// void tts_lock_release(simplelock *lkp){
//     s_lock_unlock(&lkp);
//     return;
// }

void *benchmark(void *varg) {
    // printf("starting benchmark\n");
    GM *arg = varg;
    int pid, p = 0, q = 0, k, m, N, *a;
    queuelock *abql;
    // printf("before time struct\n");
    struct timespec *starts, *ends, time;
    pid = arg->pid;
    // p = arg->p;
    // q = arg->q;
    k = arg->k;
    m = arg->m;
    N = arg->N;
    a = arg->a;
    // start = arg->start;
    // end = arg->end;
    starts = arg->starts;
    ends = arg->ends;
    abql = arg->abql;

    // printf("k: %d, m: %d, N: %d\n", k, m, N);

    // printf("thread %d, before start time\n", pid);

    clock_gettime(CLOCK_MONOTONIC, &starts[pid]);
    // starts[pid] = start;
    
    for(int i=0; i<N; i++) {
        // printf("pid: %d, before tl_lock\n", pid);
        abql_lock(abql, pid);
        // printf("pid: %d, after tl_lock\n", pid);
        for(int j=0; j<k; j++) q++;
        abql_unlock(abql, pid);
        for(int j=0; j<m; j++) p++;
    }
    clock_gettime(CLOCK_MONOTONIC, &ends[pid]);

    // ends[pid] = end;

    // printf("p: %d, q: %d\n", p, q);
    a[pid] = p+q;
    // printf("a[%d]: %d\n", pid, a[pid]);
}

int main(int argc, char **argv) {
    struct timespec *starts, *ends, min, max;
    double time, sec, nsec;
    int pid, k, m, *a, P, *pass;
    int N;
    P = 8;
    // k = 0;
    N = 750000000;
    // p = 0;
    // q = 0;
    // m = 0;

        if(argc != 3) {
        printf("Usage: pnqueens n p\nAborting...\n");
        exit(0);
    }
    k = atoi(argv[1]);
    m = atoi(argv[2]);

    a = (int *) malloc(P*sizeof(int));
    pass = (int *) malloc(P*sizeof(int));
    starts = (struct timespec *) malloc(8*sizeof(struct timespec));
    ends = (struct timespec *) malloc(8*sizeof(struct timespec));

    // printf("a, starts, ends malloced\n");

    // simplelock *lkp; 
    // lkp = (simplelock *) malloc(sizeof(simplelock)); 

    queuelock *abql;
    abql = (queuelock *) malloc(sizeof(queuelock));

    // s_lock_init(&lkp);

    // abql->lkp = lkp;
    abql->pass = pass;
    abql->pass[0] = 1;

    // clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t *threads = malloc(P * sizeof(threads));
    for(int i = 0; i < P; i++) {
        // printf("initializing thread %d\n", i);
        GM *arg = malloc(sizeof(*arg));
        arg->pid = i;
        // arg->p = p;
        // arg->q = q;
        arg->k = k;
        arg->m = m;
        arg->N = N;
        arg->a = a;
        // arg->start = start;
        // arg->end = end;
        arg->starts = starts;
        arg->ends = ends;
        arg->abql = abql;
        // printf("before pthread_create %d\n", i);
        // printf("threads created pid: %d\n", i);
        pthread_create(&threads[i], NULL, benchmark, arg);
    }

    // printf("threads ran\n");

    for(int i = 0; i < P; i++)
        pthread_join(threads[i], NULL);
    // clock_gettime(CLOCK_MONOTONIC, &end);
    free(threads);

    min = starts[0];
    max = ends[0];
    for(int i=1; i<P; i++) {
        if (starts[i].tv_sec < min.tv_sec) {
            min = starts[i];
        }
        else if (starts[i].tv_sec == min.tv_sec) {
            if (starts[i].tv_nsec < min.tv_nsec) {
                min = starts[i];
            }
        }
        if(ends[i].tv_sec > max.tv_sec) {
            max = ends[i];
        }
        else if (ends[i].tv_sec == max.tv_sec) {
            if (ends[i].tv_nsec > max.tv_nsec) {
                max = ends[i];
            }
        }
    }

    sec = max.tv_sec - min.tv_sec;
    nsec = max.tv_nsec - min.tv_nsec;

    // printf("sec: %lf, nsec: %lf\n", sec, nsec);
    time = sec + nsec / BILLION;
    // time = time / BILLION;
    printf("Elapsed time: %lf seconds\n", time);

    return 0;

}
