__kernel void test_kernel(__global float2* in_buffer,
                          __global float2* out_buffer,
                          __local float2* local_cache)
{
    size_t gid = get_global_id(0);
    size_t lid = get_local_id(0);

    local_cache[lid] = in_buffer[gid];
    local_cache[lid] = (float2)(local_cache[lid].s1, local_cache[lid].s0);

    barrier(CLK_LOCAL_MEM_FENCE);

    out_buffer[gid] = local_cache[lid];
}