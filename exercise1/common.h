#include <unistd.h>
#include <assert.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <x86intrin.h>


struct timespec timespec_normalized(time_t sec, long nsec);

struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);

struct timespec timespec_add(struct timespec lhs, struct timespec rhs);

int timespec_cmp(struct timespec lhs, struct timespec rhs);
