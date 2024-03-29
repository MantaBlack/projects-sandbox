#define CL_TARGET_OPENCL_VERSION 210

#include <CL/opencl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <profileapi.h>
#include <locale.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"

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

#define CHECK_NULL(val, msg) \
    { \
        if (val == NULL) \
        { \
            fprintf_s(stderr, "\n%s\nLocation : %s:%d\n", msg, __FILE__, __LINE__ ); \
            exit(EXIT_FAILURE); \
        } \
    }


const size_t         BLOCK_SIZE   = 256u;
const size_t         NUM_ELEMENTS = BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE;
const cl_device_type DEVICE_TYPE  = CL_DEVICE_TYPE_GPU;
const char*          KERNEL_FILE  = "reduction.cl";
const char*          KERNEL_NAME  = "do_reduction";
const char*          BUILD_OPTS   = "-Werror -cl-std=CL2.0";

static cl_platform_id   g_platform;
static cl_context       g_context;
static cl_device_id     g_device;
static cl_command_queue g_command_queue;
static cl_program       g_program;
static cl_kernel        g_kernel;

static cl_int2*         g_in_svm_elemets_a = NULL;
static cl_int2*         g_in_svm_elemets_b = NULL;
static cl_int2*         g_out_svm_buffer   = NULL;
static cl_int2*         g_host_output      = NULL;

static size_t           g_num_ctx_devices;


static char* read_file_to_string(char* contents, const char* filename, size_t* length)
{
    *length = 0;
    FILE* f = fopen(filename, "rb");
    free(contents);
    contents = NULL;

    if (f)
    {
        fseek(f, 0, SEEK_END); // go to end of file
        // since file is open in binary mode, this is the number of bytes
        // from the beginning of the file
        *length = ftell(f);
        fseek(f, 0, SEEK_SET); // go to beginning of file

        contents = (char*)  malloc(*length + 1);
        CHECK_NULL(contents, "malloc() failed");

        contents[*length] = '\0';

        fread(contents, 1, *length, f);

        fclose(f);

    }

    return contents;
}

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

static cl_int set_command_queue(void)
/**
 * Having multiple command-queues allows applications to queue multiple
 * independent commands without requiring synchronization. Note that this should
 * work as long as these objects are not being shared. Sharing of objects across
 * multiple command-queues will require the application to perform appropriate
 * synchronization. (opencl-2.1.pdf, 5.1)
 */
{
    PRINT_MSG("Setting command queue . . .");

    cl_int status = CL_SUCCESS;

    cl_queue_properties props[3] = 
    {
        CL_QUEUE_PROPERTIES,
        CL_QUEUE_PROFILING_ENABLE,
        0
    };

    g_command_queue = clCreateCommandQueueWithProperties(g_context,
                                                         g_device,
                                                         props,
                                                         &status);
    CHECK_OPENCL_ERROR(status, "clCreateCommandQueueWithProperties() failed");

    return status;
}

static cl_int set_svm_buffers(void)
{
    PRINT_MSG("Setting SVM buffers . . .");

    cl_int status = CL_SUCCESS;

    const size_t size_bytes = NUM_ELEMENTS * sizeof(cl_int2);
    const cl_uint alignment = sizeof(cl_int2);

    g_in_svm_elemets_a = (cl_int2*) clSVMAlloc(g_context,
                                              CL_MEM_READ_ONLY,
                                              size_bytes,
                                              alignment);

    if (g_in_svm_elemets_a == NULL)
    {
        LOG_ERROR("clSVMAlloc() failed for g_in_svm_elemets_a");
    }

    g_in_svm_elemets_b = (cl_int2*) clSVMAlloc(g_context,
                                              CL_MEM_READ_ONLY,
                                              size_bytes,
                                              alignment);

    if (g_in_svm_elemets_b == NULL)
    {
        LOG_ERROR("clSVMAlloc() failed for g_in_svm_elemets_b");
    }

    g_out_svm_buffer = (cl_int2*) clSVMAlloc(g_context,
                                             CL_MEM_WRITE_ONLY,
                                             size_bytes,
                                             alignment);

    if (g_out_svm_buffer == NULL)
    {
        LOG_ERROR("clSVMAlloc() failed for g_out_svm_buffer");
    }

    fprintf_s(stdout, "SVM buffers allocated : %u Bytes\n", size_bytes);

    return status;
}

static cl_int set_data(void)
{
    PRINT_MSG("Setting data . . .");

    cl_int status = CL_SUCCESS;

    const size_t size_bytes = NUM_ELEMENTS * sizeof(cl_int2);

    // map first input buffer for writing. non-blocking map
    status = clEnqueueSVMMap(g_command_queue,
                             CL_FALSE,
                             CL_MAP_WRITE,
                             g_in_svm_elemets_a,
                             size_bytes,
                             0,
                             NULL,
                             NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueSVMMap() failed for g_in_svm_elemets_a");

    // map second input buffer for writing. non-blocking map
    status = clEnqueueSVMMap(g_command_queue,
                             CL_FALSE,
                             CL_MAP_WRITE,
                             g_in_svm_elemets_b,
                             size_bytes,
                             0,
                             NULL,
                             NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueSVMMap() failed for g_in_svm_elemets_b");

    // fill the output buffer with zeros
    cl_int2 pattern = {0};

    status = clEnqueueSVMMemFill(g_command_queue,
                                 g_out_svm_buffer,
                                 &pattern,
                                 sizeof(cl_int2),
                                 size_bytes,
                                 0,
                                 NULL,
                                 NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueSVMMemFill() failed for g_out_svm_buffer");

    // block and wait for all previously queued commands to complete
    status = clFinish(g_command_queue);
    CHECK_OPENCL_ERROR(status, "clFinish() failed");

    // allocate memory for host kernel
    g_host_output = (cl_int2*) malloc(size_bytes);
    CHECK_NULL(g_host_output, "malloc() failed");

    size_t i = 0;
    cl_ulong4 avg = {0};

    for (i = 0; i < NUM_ELEMENTS; ++i)
    {
        g_in_svm_elemets_a[i] = (cl_int2){ {rand(), rand()} };
        g_in_svm_elemets_b[i] = (cl_int2){ {rand(), rand()} };

        g_host_output[i] = (cl_int2){0};

        avg.s[0] += g_in_svm_elemets_a[i].s[0];
        avg.s[1] += g_in_svm_elemets_a[i].s[1];
        avg.s[2] += g_in_svm_elemets_b[i].s[0];
        avg.s[3] += g_in_svm_elemets_b[i].s[1];
    }

    // unmap buffers indicating that updates are completed by host
    status = clEnqueueSVMUnmap(g_command_queue,
                               g_in_svm_elemets_a,
                               0,
                               NULL,
                               NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueSVMUnmap() failed for g_in_svm_elemets_a");

    status = clEnqueueSVMUnmap(g_command_queue,
                               g_in_svm_elemets_b,
                               0,
                               NULL,
                               NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueSVMUnmap() failed for g_in_svm_elemets_b");

    // block and wait for all unmap commands to complete
    status = clFinish(g_command_queue);
    CHECK_OPENCL_ERROR(status, "clFinish() failed");

    fprintf_s(stdout, "Average a.s[0]  : %d\n", avg.s[0] / NUM_ELEMENTS);
    fprintf_s(stdout, "Average a.s[1]  : %d\n", avg.s[1] / NUM_ELEMENTS);
    fprintf_s(stdout, "Average b.s[0]  : %d\n", avg.s[2] / NUM_ELEMENTS);
    fprintf_s(stdout, "Average b.s[1]  : %d\n", avg.s[3] / NUM_ELEMENTS);

    return status;
}

static cl_int set_program(void)
{
    PRINT_MSG("Setting program . . .");

    cl_int status = CL_SUCCESS;

    size_t length        = 0;
    char* program_source = NULL;

    program_source = read_file_to_string(program_source, KERNEL_FILE, &length);
    CHECK_NULL(program_source, "read_file_to_string() failed");

    fprintf_s(stdout, GRN "\n%s\n" RESET, program_source);

    g_program = clCreateProgramWithSource(g_context,
                                          1u,
                                          (const char **)(&program_source),
                                          &length,
                                          &status);
    CHECK_OPENCL_ERROR(status, "clCreateProgramWithSource() failed");

    status = clBuildProgram(g_program,
                            1u,
                            &g_device,
                            BUILD_OPTS,
                            NULL,
                            NULL);

    if (status == CL_BUILD_PROGRAM_FAILURE)
    {
        cl_build_status build_status = CL_BUILD_NONE;

        status = clGetProgramBuildInfo(g_program,
                                       g_device,
                                       CL_PROGRAM_BUILD_STATUS,
                                       sizeof(build_status),
                                       &build_status,
                                       NULL);
        CHECK_OPENCL_ERROR(status, "clGetProgramBuildInfo() failed");

        size_t build_log_size = 0;
        char*  build_log      = NULL;

        status = clGetProgramBuildInfo(g_program,
                                       g_device,
                                       CL_PROGRAM_BUILD_LOG,
                                       sizeof(char),
                                       NULL,
                                       &build_log_size);
        CHECK_OPENCL_ERROR(status, "clGetProgramBuildInfo failed");

        build_log = (char*) malloc(build_log_size);
        CHECK_NULL(build_log, "malloc failed");

        status = clGetProgramBuildInfo(g_program,
                                       g_device,
                                       CL_PROGRAM_BUILD_LOG,
                                       build_log_size,
                                       build_log,
                                       NULL);
        CHECK_OPENCL_ERROR(status, "clGetProgramBuildInfo failed");

        fprintf_s(stdout, RED "Build status : %d\n" RESET, build_status);
        fprintf_s(stdout, RED "Build Log : \n%s\n" RESET, build_log);

        free(build_log);
    }
    else
    {
        CHECK_OPENCL_ERROR(status, "clBuildProgram() failed");
    }

    free(program_source);

    return status;
}

static cl_int set_kernel(void)
{
    PRINT_MSG("Setting kernel . . .");

    cl_int status = CL_SUCCESS;

    g_kernel = clCreateKernel(g_program,
                              KERNEL_NAME,
                              &status);
    CHECK_OPENCL_ERROR(status, "clCreateKernel() failed");

    status = clSetKernelArgSVMPointer(g_kernel,
                                      0,
                                      g_in_svm_elemets_a);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg() failed");

    status = clSetKernelArgSVMPointer(g_kernel,
                                      1,
                                      g_in_svm_elemets_b);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg() failed");

    status = clSetKernelArgSVMPointer(g_kernel,
                                      2,
                                      g_out_svm_buffer);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg() failed");

    status = clSetKernelArg(g_kernel,
                            3,
                            BLOCK_SIZE * sizeof(cl_int2),
                            NULL);
    CHECK_OPENCL_ERROR(status, "clSetKernelArg() failed");

    return status;
}

static cl_int run_kernel(void)
{
    PRINT_MSG("Running kernel . . .");

    cl_int status = CL_SUCCESS;

    // fix warning for variable length array
    enum { WORK_DIM = 1};

    // const cl_uint work_dim = 1;
    const size_t global_work_size[WORK_DIM] = {NUM_ELEMENTS};
    const size_t local_work_size[WORK_DIM]  = {BLOCK_SIZE};
    cl_event run_event;

    status = clEnqueueNDRangeKernel(g_command_queue,
                                    g_kernel,
                                    WORK_DIM,
                                    NULL,
                                    global_work_size,
                                    local_work_size,
                                    0,
                                    NULL,
                                    &run_event);
    CHECK_OPENCL_ERROR(status, "clEnqueueNDRangeKernel() failed");

    // synchronization point
    status = clFinish(g_command_queue);
    CHECK_OPENCL_ERROR(status, "clFinish() failed");

    cl_ulong cmd_start = 0;
    status = clGetEventProfilingInfo(run_event,
                                     CL_PROFILING_COMMAND_START,
                                     sizeof(size_t),
                                     &cmd_start,
                                     NULL);
    CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo() failed");

    cl_ulong cmd_end = 0;
    status = clGetEventProfilingInfo(run_event,
                                     CL_PROFILING_COMMAND_END,
                                     sizeof(size_t),
                                     &cmd_end,
                                     NULL);
    CHECK_OPENCL_ERROR(status, "clGetEventProfilingInfo() failed");

    fprintf_s(stdout, "Kernel time : %5.3g milliseconds\n", (cmd_end - cmd_start) / 1.0e6);

    return status;
}

static cl_int run_host_kernel(void)
{
    PRINT_MSG("Running host kernel . . .");

    cl_int status = CL_SUCCESS;
    const size_t size_bytes = NUM_ELEMENTS * sizeof(cl_int2);

    status = clEnqueueSVMMap(g_command_queue,
                             CL_FALSE,
                             CL_MAP_READ,
                             g_in_svm_elemets_a,
                             size_bytes,
                             0,
                             NULL,
                             NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueSVMMap() failed for g_in_svm_elemets_a");

    status = clEnqueueSVMMap(g_command_queue,
                             CL_FALSE,
                             CL_MAP_READ,
                             g_in_svm_elemets_b,
                             size_bytes,
                             0,
                             NULL,
                             NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueSVMMap() failed for g_in_svm_elemets_b");

    status = clEnqueueSVMMap(g_command_queue,
                             CL_FALSE,
                             CL_MAP_READ,
                             g_out_svm_buffer,
                             size_bytes,
                             0,
                             NULL,
                             NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueSVMMap() failed for g_out_svm_buffer");

    // sync point
    status = clFinish(g_command_queue);
    CHECK_OPENCL_ERROR(status, "clFinish() failed");

    size_t i = 0;
    LARGE_INTEGER start = {0};
    LARGE_INTEGER end   = {0};
    LARGE_INTEGER freq  = {0};
    WINBOOL query_ok    = 1;

    query_ok = QueryPerformanceFrequency(&freq);

    query_ok = QueryPerformanceCounter(&start) && query_ok;

    for (i = 0; i < NUM_ELEMENTS; ++i)
    {
        g_host_output[i].s[0] = g_in_svm_elemets_a[i].s[0] + g_in_svm_elemets_b[i].s[0];
        g_host_output[i].s[1] = g_in_svm_elemets_a[i].s[1] + g_in_svm_elemets_b[i].s[1];
    }

    query_ok = QueryPerformanceCounter(&end) && query_ok;

    if (query_ok)
    {
        LARGE_INTEGER elapsed = {0};
        elapsed.QuadPart = end.QuadPart - start.QuadPart;

        /**
         * We now have the elapsed number of ticks, along with the
         * number of ticks-per-second. We use these values
         * to convert to the number of elapsed microseconds.
         * To guard against loss-of-precision, we convert
         * to microseconds *before* dividing by ticks-per-second.
         * https://learn.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps
         */

        elapsed.QuadPart *= 1.0e3; // in milliseconds
        elapsed.QuadPart /= freq.QuadPart;

        fprintf_s(stdout, "Host kernel time : %5.3g milliseconds\n", (double)elapsed.QuadPart);
    }

    cl_bool failed = CL_FALSE;

    // verify results
    for (i = 0; i < NUM_ELEMENTS; ++i)
    {
        if (g_host_output[i].s[0] != g_out_svm_buffer[i].s[0] ||
            g_host_output[i].s[1] != g_out_svm_buffer[i].s[1])
        {
            failed = CL_TRUE;
            break;
        }
    }
    
    if (failed)
    {
        fprintf_s(stderr, "Verification result : FAIL\n");
    }
    else
    {
        fprintf_s(stderr, "Verification result : PASS\n");
    }

    status = clEnqueueSVMUnmap(g_command_queue,
                               g_in_svm_elemets_a,
                               0,
                               NULL,
                               NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueSVMUnmap() failed for g_in_svm_elemets_a");

    status = clEnqueueSVMUnmap(g_command_queue,
                               g_in_svm_elemets_b,
                               0,
                               NULL,
                               NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueSVMUnmap() failed for g_in_svm_elemets_b");

    status = clEnqueueSVMUnmap(g_command_queue,
                               g_out_svm_buffer,
                               0,
                               NULL,
                               NULL);
    CHECK_OPENCL_ERROR(status, "clEnqueueSVMUnmap() failed for g_out_svm_buffer");

    // sync point
    status = clFinish(g_command_queue);
    CHECK_OPENCL_ERROR(status, "clFinish() failed");

    return status;
}

static void free_svm_buffers(void)
{
    PRINT_MSG("Freeing SVM buffers . . .");

    clSVMFree(g_context, g_in_svm_elemets_a);
    clSVMFree(g_context, g_in_svm_elemets_b);
    clSVMFree(g_context, g_out_svm_buffer);
}


int main(int argc, char const *argv[])
{
    cl_int status = CL_SUCCESS;

    // setlocale(LC_ALL, "nl-NL");

    srand(time(NULL));

    status = set_platform();
    CHECK_OPENCL_ERROR(status, "set_platform() failed");

    status = set_context();
    CHECK_OPENCL_ERROR(status, "set_context() failed");

    status = set_device();
    CHECK_OPENCL_ERROR(status, "set_device() failed");

    status = set_command_queue();
    CHECK_OPENCL_ERROR(status, "set_command_queue() failed");

    status = set_svm_buffers();
    CHECK_OPENCL_ERROR(status, "set_svm_buffers() failed");

    status = set_data();
    CHECK_OPENCL_ERROR(status, "set_data() failed");

    status = set_program();
    CHECK_OPENCL_ERROR(status, "set_program() failed");

    status = set_kernel();
    CHECK_OPENCL_ERROR(status, "set_kernel() failed");

    status = run_kernel();
    CHECK_OPENCL_ERROR(status, "run_kernel() failed");

    status = run_host_kernel();
    CHECK_OPENCL_ERROR(status, "run_host_kernel() failed");

    free_svm_buffers();

    free(g_host_output);

    return 0;
}