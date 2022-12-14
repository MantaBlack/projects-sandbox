#define CL_TARGET_OPENCL_VERSION 210

#include <CL/opencl.h>
#include <stdio.h>

#define CHECK_OPENCL_ERROR(err, msg) \
    if(check_error(err, msg)) \
    { \
        fprintf_s(stderr, "\nStatus: %d\n", err); \
        fprintf_s(stderr, "Location : %s:%d\n", __FILE__, __LINE__ ); \
        exit(EXIT_FAILURE); \
    }

#define LOG_ERROR(msg) \
    { \
        fprintf_s(stderr, "\n%s\nLocation : %s:%d\n", msg, __FILE__, __LINE__ ); \
        exit(EXIT_FAILURE); \
    }

#define PRINT_PLATFORM_INFO(info_type, info_name) \
    { \
        print_platform_string_info( info_type, info_name ); \
    }


const cl_uint        BLOCK_SIZE   = 256u;
const cl_uint        NUM_ELEMENTS = BLOCK_SIZE * BLOCK_SIZE;
const cl_device_type DEVICE_TYPE  = CL_DEVICE_TYPE_GPU;
const char*          KERNEL_FILE  = "reduction.cl";
const char*          KERNEL_NAME  = "do_reduction";

static cl_platform_id   g_platform;
static cl_context       g_context;
static cl_device_id     g_device;
static cl_command_queue g_command_queue;
static cl_program       g_program;
static cl_kernel        g_kernel;

static cl_mem           in_buffer_a;
static cl_mem           in_buffer_b;
static cl_mem           out_buffer;

static int check_error(cl_int err, const char* msg)
{
    if (err != CL_SUCCESS)
    {
        fprintf_s(stderr, "%s\n", msg);
        return 1;
    }

    return 0;
}

static void print_platform_string_info(const cl_int info_type, const char* info_name)
{
    cl_int status           = CL_SUCCESS;
    size_t param_size_bytes = 0;

    status = clGetPlatformInfo(g_platform,
                               (cl_platform_info) info_type,
                               sizeof(char),
                               NULL,
                               &param_size_bytes);
    
    CHECK_OPENCL_ERROR(status, "clGetPlatformInfo failed");

    if (param_size_bytes <= 0)
    {
        LOG_ERROR("clGetPlatformInfo() failed");
    }

    char* param_buf = NULL;
    param_buf = (char*) malloc(param_size_bytes);

    if (param_buf != NULL)
    {
        status = clGetPlatformInfo(g_platform,
                                   (cl_platform_info) info_type,
                                   param_size_bytes,
                                   param_buf,
                                   NULL);
        CHECK_OPENCL_ERROR(status, "clGetPlatformInfo() failed");

        fprintf_s(stdout, "%s : %s\n", info_name, param_buf);
    }

    free(param_buf);
}

static cl_int set_platform(void)
{
    cl_int status         = CL_SUCCESS;
    cl_uint num_platforms = 0;

    // get a list of available platforms
    status = clGetPlatformIDs(0, NULL, &num_platforms);
    CHECK_OPENCL_ERROR(status, "clGetPlatformIDs() failed");

    if (num_platforms <= 0)
    {
        LOG_ERROR("No platforms found\n");
    }
    else
    {
        fprintf_s(stdout, "Found %d platform(s).\n", num_platforms);
    }

    cl_platform_id platforms[num_platforms];

    // add platforms found
    status = clGetPlatformIDs(num_platforms, platforms, NULL);
    CHECK_OPENCL_ERROR(status, "clGetPlatformIDs() failed");

    // pick the first one
    g_platform = platforms[0];
    PRINT_PLATFORM_INFO(CL_PLATFORM_NAME, "Platform name");
    PRINT_PLATFORM_INFO(CL_PLATFORM_VENDOR, "Platform vendor");
    PRINT_PLATFORM_INFO(CL_PLATFORM_PROFILE, "Platform profile");

    return status;
}


int main(int argc, char const *argv[])
{
    cl_int status = CL_SUCCESS;

    status = set_platform();
    CHECK_OPENCL_ERROR(status, "set_platform() failed");

    return 0;
}