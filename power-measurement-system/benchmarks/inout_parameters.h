#ifndef inout_parameters_h
#define inout_parameters_h

struct input_parameters
{
        double * __restrict aa;
        double * __restrict bb;
        long long length;
        long long num_repetitions;
};
struct output_parameters
{
        double result;
};
#endif

