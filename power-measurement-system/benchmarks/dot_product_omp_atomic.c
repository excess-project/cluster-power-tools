#include "dot_product_omp_atomic.h"
/* BENCHMARk FUNCTION ADD_ATOMIC
 * Called from each thread
*/
void dot_product_omp_atomic(struct input_parameters* func_input, struct output_parameters* func_output)
{
   double* __restrict local_a;
   double* __restrict local_b;
   double* dot_product;
   const long long length = func_input->length;
   const long long num_repetitions = func_input->num_repetitions;
   long long ii,jj;
   local_a = func_input->aa;
   local_b = func_input->bb;
   dot_product = &(func_output->result);
   for(jj=0;jj<num_repetitions;jj++)
   {
     for(ii=0;ii<length;ii++)
     {
//       #pragma omp atomic 
       *dot_product+=local_a[ii]*local_b[ii];
     }
   }
}
