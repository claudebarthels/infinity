/**
 * Utils - Debug
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef UTILS_DEBUG_H_
#define UTILS_DEBUG_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef INFINITY_DEBUG_ON
	#define INFINITY_DEBUG(X, ...) {fprintf(stdout, X, ##__VA_ARGS__); fflush(stdout);}
#else
	#define INFINITY_DEBUG(X, ...) {}
#endif

#ifdef INFINITY_ASSERT_ON
	#define INFINITY_ASSERT(B, X, ...) {if(!(B)) {fprintf(stdout, X, ##__VA_ARGS__); fflush(stdout); exit(-1);}}
#else
	#define INFINITY_ASSERT(B, X, ...) {}
#endif

#endif /* UTILS_DEBUG_H_ */
