__kernel void reduction(__global int2* input_a,
                        __global int2* input_b,
                        __global int2* output)
{
    size_t gid = get_global_id(0);
}
