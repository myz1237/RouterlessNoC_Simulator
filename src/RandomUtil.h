#ifndef NOCSIM_RANDOMUTIL_H
#define NOCSIM_RANDOMUTIL_H

#include <vector>
#include <assert.h>
// interface to Knuth's RANARRAY RNG
void   ran_start(long seed);
long   ran_next();

inline void random_seed(long seed) {
    ran_start(seed);
}

inline unsigned long random_long() {
    return ran_next();
}

// Returns a random integer in the range [0,max]
inline int random_int(int min, int max) {
    return (ran_next() % (max-min+1)+min);
}


#endif //NOCSIM_RANDOMUTIL_H
