#define CL_TARGET_OPENCL_VERSION 210

#include <CL/opencl.h>
#include <stdio.h>

#define CHECK_OPENCL_ERROR(status, msg) \
    if(check_error(status, msg)) \
    { \
        fprintf_s(stderr, "\nStatus: %d\n", status); \
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

#define PRINT_DEVICE_INFO(info_type, info_name) \
    { \
        print_device_info( info_type, info_name ); \
    }

#define PRINT_MSG(msg) \
    { \
        fprintf_s(stdout, "\n%s\n", msg); \
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

static size_t           g_num_ctx_devices;

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

static void print_device_info(const cl_int info_type, const char* info_name)
{
    cl_int status      = CL_SUCCESS;
    size_t param_value = 0;

    status = clGetDeviceInfo(g_device,
                             (cl_device_info) info_type,
                             sizeof(size_t),
                             &param_value,
                             NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceInfo() failed");

    if (info_type == CL_DEVICE_SVM_CAPABILITIES)
    {
        fprintf_s(stdout,
                  "Device SVM coarse grain : %s\n",
                  (param_value & CL_DEVICE_SVM_COARSE_GRAIN_BUFFER) ? "Yes" : "No" );
    
        fprintf_s(stdout,
                  "Device SVM fine grain buffer : %s\n",
                  (param_value & CL_DEVICE_SVM_FINE_GRAIN_BUFFER) ? "Yes" : "No" );
    
        fprintf_s(stdout,
                  "Device SVM fine grain system : %s\n",
                  (param_value & CL_DEVICE_SVM_FINE_GRAIN_SYSTEM) ? "Yes" : "No" );
    
        fprintf_s(stdout,
                  "Device SVM atomics : %s\n",
                  (param_value & CL_DEVICE_SVM_ATOMICS) ? "Yes" : "No" );
    }
    else
    {
        fprintf_s(stdout, "%s : %d\n", info_name, param_value);
    }
}

static cl_int set_platform(void)
{
    PRINT_MSG("Setting platform . . .");

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
        fprintf_s(stdout, "Platform(s) found : %d.\n", num_platforms);
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
    PRINT_PLATFORM_INFO(CL_PLATFORM_VERSION, "Platform version");

    return status;
}

static cl_int set_context(void)
{
    PRINT_MSG("Setting context . . .");

    cl_int status = CL_SUCCESS;

    cl_context_properties props[3] = 
    {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties) g_platform,
        0
    };

    g_context = clCreateContextFromType(props,
                                        DEVICE_TYPE,
                                        NULL,
                                        NULL,
                                        &status);
    CHECK_OPENCL_ERROR(status, "clCreateContextFromType() failed");

    status = clGetContextInfo(g_context,
                              CL_CONTEXT_NUM_DEVICES,
                              sizeof(g_num_ctx_devices),
                              &g_num_ctx_devices,
                              NULL);
    CHECK_OPENCL_ERROR(status, "clGetContextInfo() failed");

    fprintf_s(stdout, "# Devices in context : %d\n", g_num_ctx_devices);

    return status;
}

static cl_int set_device(void)
{
    PRINT_MSG("Setting device . . .");

    cl_int status = CL_SUCCESS;
    cl_device_id devices[g_num_ctx_devices];

    status = clGetContextInfo(g_context,
                              CL_CONTEXT_DEVICES,
                              sizeof(devices),
                              devices,
                              NULL);
    CHECK_OPENCL_ERROR(status, "clGetContextInfo() failed");

    g_device = devices[0];

    char device_name[256];

#ifdef CL_DEVICE_BOARD_NAME_AMD
    status = clGetDeviceInfo(g_device,
                             CL_DEVICE_BOARD_NAME_AMD,
                             sizeof(device_name),
                             device_name,
                             NULL);
#else
    status = clGetDeviceInfo(g_device,
                             CL_DEVICE_NAME,
                             sizeof(device_name),
                             device_name,
                             NULL);
#endif

    CHECK_OPENCL_ERROR(status, "clGetDeviceInfo() failed");

    fprintf_s(stdout, "Device name : %s\n", device_name);

    PRINT_DEVICE_INFO(CL_DEVICE_MAX_COMPUTE_UNITS, "Max compute units");
    PRINT_DEVICE_INFO(CL_DEVICE_MAX_WORK_GROUP_SIZE, "Max work group");
    PRINT_DEVICE_INFO(CL_DEVICE_SVM_CAPABILITIES, "");

    return status;
}


int main(int argc, char const *argv[])
{
    cl_int status = CL_SUCCESS;

    status = set_platform();
    CHECK_OPENCL_ERROR(status, "set_platform() failed");

    status = set_context();
    CHECK_OPENCL_ERROR(status, "set_context() failed");

    status = set_device();
    CHECK_OPENCL_ERROR(status, "set_device() failed");

    return 0;
}