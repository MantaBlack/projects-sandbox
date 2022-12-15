__kernel void do_reduction(__global int2* input_a,
                           __global int2* input_b,
                           __global int2* output,
                           __local  int2* local_cache)
{
    size_t gid = get_global_id(0);
    size_t lid = get_local_id(0);

    local_cache[lid] = input_a[gid] + input_b[gid];

    barrier(CLK_LOCAL_MEM_FENCE);

    output[gid] = local_cache[lid];
}
