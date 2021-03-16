/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     Flit.h                                                          *
*  @brief    An interface for random number generator provided by D E Knuth  *
*                                                                            *
*  @author   Yizhuo Meng                                                     *
*  @email    myz2ylp@connect.hku.hk                                          *
*  @date     2020.03.16                                                      *
*                                                                            *
*****************************************************************************/

#ifndef NOCSIM_RANDOMUTIL_H
#define NOCSIM_RANDOMUTIL_H

#include <vector>

// interface to Knuth's RANARRAY RNG
void   ran_start(long seed);
long   ran_next( );
void   ranf_start(long seed);
double ranf_next( );

inline void random_seed(long seed) {
    ran_start(seed);
    ranf_start(seed);
}

inline unsigned long next_random_long() {
    return ran_next();
}

// Returns a random integer in the range [0,max]
inline int random_int(int min, int max) {
    return (ran_next() % (max-min+1)+min);
}

// Returns a random floating-point value in the rage [0,1]
inline double next_random_double(  ) {
    return ranf_next( );
}

// Returns a random floating-point value in the rage [0,max]
inline double random_double( double max ) {
    return ( ranf_next( ) * max );
}

#endif //NOCSIM_RANDOMUTIL_H
