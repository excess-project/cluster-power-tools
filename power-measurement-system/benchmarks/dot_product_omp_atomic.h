#ifndef dot_product_omp_atomic_h
#define dot_product_omp_atomic_h
#include "inout_parameters.h"

/* BENCHMARk FUNCTION ADD_ATOMIC
 * Called from each thread
*/
void dot_product_omp_atomic(struct input_parameters* func_input, struct output_parameters* func_output);
#endif
