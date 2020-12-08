// **Configuration**

#define ROUNDS 1

#define D_RDTSC 0

#define D_GETTIME 100

// **Configuration**


#include <stdio.h>

#include <stdlib.h>

#include <stdint.h>

#include <time.h>



double get_cpu_rate() {

    // Ref: https://stackoverflow.com/a/646254/6734174

    FILE *fp;

    char buffer[20];

    double ret = 0;

    const char* cmd = "lscpu | grep -oP \'MHz:\\s+\\K.*\' | head -n 1";

    fp = popen(cmd, "r");

    if (fp == NULL) {

        printf("Failed to run command\n" );

        exit(1);

    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {

        ret = atof(buffer);

    }

    pclose(fp);

    return ret;

}


#if D_RDTSC

/* assembly code to read the TSC */

static inline uint64_t RDTSC() {

    unsigned int hi, lo;

    __asm__ volatile("rdtsc" : "=a" (lo), "=d" (hi));

    return ((uint64_t)hi << 32) | lo;

}

#endif


#if D_GETTIME

double GETTIME() {

    struct timespec ts;

    double sec;

    clock_gettime(CLOCK_REALTIME, &ts);

    sec = ts.tv_nsec;

    sec /= 1e9;

    sec += ts.tv_sec;

    return sec;

}

#endif


int main(int argc, char** argv)

{

    const uint64_t interval = 1000;

    double rand_x, rand_y, origin_dist;

    uint64_t circle_points = 0, square_points = 0;

    uint64_t measure_rdtsc = 0;

    double measure_gettime = 0;

    double arr_pi[ROUNDS] = {};

    srand(time(NULL));


#if D_RDTSC

    uint64_t c1 = RDTSC();

#endif

#if D_GETTIME

    double t1 = GETTIME();

#endif


    for (int r = 0; r < ROUNDS; r++) {

        for (uint64_t i = 0; i < (interval * interval); i++) {

            rand_x = (double)(rand() % (interval + 1)) / interval;

            rand_y = (double)(rand() % (interval + 1)) / interval;

            origin_dist = rand_x * rand_x + rand_y * rand_y;

            if (origin_dist <= 1.0)

                circle_points++;

            square_points++;

        }

        arr_pi[r] = (double)(4 * circle_points) / square_points;

    }


#if D_RDTSC

    uint64_t cycle = RDTSC()-c1;

    measure_rdtsc = cycle;

#endif

#if D_GETTIME

    double time_s = GETTIME()-t1;

    measure_gettime = time_s;

#endif


    double sum_pi = 0;

    for(int i = 0; i < ROUNDS; i++) {

        sum_pi += arr_pi[i];

    }

    double cpu_rate = get_cpu_rate();

    printf(" %.6lf |", (double)sum_pi/ROUNDS);

    printf(" %.6lf |", (double)measure_rdtsc/ROUNDS/(cpu_rate*1000000));

    printf(" %.6lf | ", (double)measure_gettime/ROUNDS);

    return 0;

}
