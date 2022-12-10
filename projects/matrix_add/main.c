#include <stdio.h>
#include <stdlib.h>

#define CL_TARGET_OPENCL_VERSION 210

// OpenCL includes
#include <CL/cl.h>

#define MAX_PLATFORM_INFO_STRING_LEN 256
#define APP_DEVICE_TYPE              CL_DEVICE_TYPE_GPU
#define MIN_WORK_ITEM_DIM            3                   // https://registry.khronos.org/OpenCL/specs/3.0-unified/html/OpenCL_API.html#clGetDeviceInfo

static cl_platform_id g_platform;
static char           g_platform_name[MAX_PLATFORM_INFO_STRING_LEN];
static char           g_platform_vendor[MAX_PLATFORM_INFO_STRING_LEN];
static char           g_platform_version[MAX_PLATFORM_INFO_STRING_LEN];

static cl_uint        g_num_context_devices;
static cl_device_id   g_device;
static char           g_device_name[MAX_PLATFORM_INFO_STRING_LEN];
static char           g_device_profile[MAX_PLATFORM_INFO_STRING_LEN];
static size_t         g_device_max_compute_units;
static size_t         g_device_max_workitem_dims;
static size_t         g_device_max_workgroup_size;
static size_t         g_device_max_workitem_sizes[MIN_WORK_ITEM_DIM];

static cl_context       g_context;
static cl_command_queue g_command_queue;


static void set_platform()
{
    cl_int status         = CL_SUCCESS;
    cl_uint num_platforms = 0;

    status = clGetPlatformIDs(0, NULL, &num_platforms);

    if (status != CL_SUCCESS || num_platforms <= 0)
    {
        fprintf(stderr, "ERROR: Failed to get available platforms\n");
        exit(EXIT_FAILURE);
    }

    cl_platform_id platforms[num_platforms];

    status = clGetPlatformIDs(num_platforms, platforms, NULL);

    if (status == CL_SUCCESS)
    {
        fprintf(stdout, "Found %u platform(s)\n", num_platforms);

        g_platform = platforms[0];

        (void) clGetPlatformInfo(g_platform,
                                 CL_PLATFORM_NAME,
                                 sizeof g_platform_name,
                                 g_platform_name,
                                 NULL);

        (void) clGetPlatformInfo(g_platform,
                                 CL_PLATFORM_VENDOR,
                                 sizeof g_platform_vendor,
                                 g_platform_vendor,
                                 NULL);

        (void) clGetPlatformInfo(g_platform,
                                 CL_PLATFORM_VERSION,
                                 sizeof g_platform_version,
                                 g_platform_version,
                                 NULL);
    }
}


static void set_context()
{
    cl_int status = CL_SUCCESS;

    cl_context_properties cprops[3] =
    {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)(g_platform),
        0
    };

    g_context = clCreateContextFromType(cprops,
                                        APP_DEVICE_TYPE,
                                        NULL,
                                        NULL,
                                        &status);

    if (status != CL_SUCCESS)
    {
        fprintf(stderr, "\nERROR(%d): Failed to set context\n", status);
        exit(EXIT_FAILURE);
    }
}

static void set_device_from_context()
{
    cl_int status = CL_SUCCESS;

    status = clGetContextInfo(g_context,
                              CL_CONTEXT_NUM_DEVICES,
                              sizeof(g_num_context_devices),
                              &g_num_context_devices,
                              NULL);

    if (status == CL_SUCCESS)
    {
        fprintf(stdout, "Found %u device(s) in context\n", g_num_context_devices);

        if (g_num_context_devices > 0)
        {
            cl_device_id devices[g_num_context_devices];

            status = clGetContextInfo(g_context,
                                      CL_CONTEXT_DEVICES,
                                      sizeof(devices),
                                      devices,
                                      NULL);

            if (status == CL_SUCCESS)
            {
                g_device = devices[0];
            }
        }
    }

    if (status == CL_SUCCESS)
    {
        (void) clGetDeviceInfo(g_device,
                               CL_DEVICE_NAME,
                               sizeof g_device_name,
                               g_device_name,
                               NULL);

        (void) clGetDeviceInfo(g_device,
                               CL_DEVICE_PROFILE,
                               sizeof g_device_profile,
                               g_device_profile,
                               NULL);

        (void) clGetDeviceInfo(g_device,
                               CL_DEVICE_MAX_COMPUTE_UNITS,
                               sizeof g_device_max_compute_units,
                               (void *) &g_device_max_compute_units,
                               NULL);

        (void) clGetDeviceInfo(g_device,
                               CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
                               sizeof g_device_max_workitem_dims,
                               (void *) &g_device_max_workitem_dims,
                               NULL);

        (void) clGetDeviceInfo(g_device,
                               CL_DEVICE_MAX_WORK_GROUP_SIZE,
                               sizeof g_device_max_workgroup_size,
                               (void *) &g_device_max_workgroup_size,
                               NULL);

        (void) clGetDeviceInfo(g_device,
                               CL_DEVICE_MAX_WORK_ITEM_SIZES,
                               sizeof g_device_max_workitem_sizes,
                               (void *) &g_device_max_workitem_sizes,
                               NULL);
    }

    if (status != CL_SUCCESS)
    {
        fprintf(stderr,
                "\nERROR(%d): Failed to set device from context\n",
                status);
        exit(EXIT_FAILURE);
    }
}

static void set_command_queue()
{
    cl_int status = CL_SUCCESS;

    cl_queue_properties qprop[3] =
    {
        CL_QUEUE_PROPERTIES,
        CL_QUEUE_PROFILING_ENABLE,
        0
    };

    g_command_queue = clCreateCommandQueueWithProperties(g_context,
                                                         g_device,
                                                         qprop,
                                                         &status);

    if (status != CL_SUCCESS)
    {
        fprintf(stderr, "\nERROR(%d): Failed to set command queue\n", status);
        exit(EXIT_FAILURE);
    }

}


int main()
{
    set_platform();

    fprintf(stdout, "Platform name:    %s\n", g_platform_name);
    fprintf(stdout, "Platform vendor:  %s\n", g_platform_vendor);
    fprintf(stdout, "Platform version: %s\n", g_platform_version);

    set_context();

    set_device_from_context();

    // set_device();

    fprintf(stdout, "Device name:        %s\n", g_device_name);
    fprintf(stdout, "Device profile:     %s\n", g_device_profile);
    fprintf(stdout, "Max compute units:  %lu\n", g_device_max_compute_units);
    fprintf(stdout, "Max workgroup size: %lu\n", g_device_max_workgroup_size);
    fprintf(stdout, "Max workitem dims:  %lu\n", g_device_max_workitem_dims);
    fprintf(stdout, "Max workitem sizes:  (%lu, %lu, %lu)\n", g_device_max_workitem_sizes[0],
                                                              g_device_max_workitem_sizes[1],
                                                              g_device_max_workitem_sizes[2]);

    set_command_queue();

    return 0;
}