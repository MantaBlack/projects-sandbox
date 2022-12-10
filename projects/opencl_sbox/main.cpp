#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

#include <CL/cl2.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <random>

using real  = cl_float;
using real2 = cl_float2;

static cl_device_type g_device_type   = CL_DEVICE_TYPE_GPU;

static cl::Platform     g_platform;
static cl::Context      g_context;
static cl::Device       g_device;
static cl::CommandQueue g_command_queue;
static cl::Buffer       g_input_buffer;
static cl::Buffer       g_output_buffer;

static cl_uint g_num_particles = 60 * 256;
static real*   g_particles;

static cl::Buffer g_particles_buf;
static cl::Buffer g_pinned_output_buf;

static void set_platform(void)
{
    cl_int status = CL_SUCCESS;
    std::vector<cl::Platform> platforms;

    try
    {
        status = cl::Platform::get(&platforms);

        if (platforms.size() > 0)
        {
            std::cout << "Found " << platforms.size() << " platforms" << std::endl;
            g_platform = platforms.front();

            std::cout << "Platform name: "   << g_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
            std::cout << "Platform vendor: " << g_platform.getInfo<CL_PLATFORM_VENDOR>() << std::endl;
        }

    }
    catch(cl::Error& e)
    {
        std::cout << "set_platform() failed (" << status << ") : " << e.what() << std::endl;
    }
}

static void set_context(void)
{
    cl_int status = CL_SUCCESS;

    try
    {
        cl_context_properties cprops[3] = 
        {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties)(g_platform)(),
            0
        };

        g_context = cl::Context(g_device_type, cprops, NULL, NULL, &status);

    }
    catch(cl::Error& e)
    {
        std::cout << "set_context() failed (" << status << ") : " << e.what() << std::endl;
    }
}

static void set_device()
{
    cl_int status = CL_SUCCESS;
    std::vector<cl::Device> devices;

    try
    {
        devices = g_context.getInfo<CL_CONTEXT_DEVICES>();

        size_t num_devices = devices.size();

        if (num_devices <= 0)
        {
            std::cerr << "No device available" << std::endl;
            exit(EXIT_FAILURE);
        }
        else
        {
            std::cout << "Found " << num_devices << " device(s)" << std::endl;
        }

        std::vector<cl::Device>::iterator itr;

        for(itr = devices.begin(); itr != devices.end(); ++itr)
        {
#ifdef CL_DEVICE_BOARD_NAME_AMD
            std::string device_name;

            (*itr).getInfo<std::string>(CL_DEVICE_BOARD_NAME_AMD, &device_name);

            std::cout << "Device name       : " << device_name << std::endl;
#else
            std::cout << "Device name       : " << (*itr).getInfo<CL_DEVICE_NAME>() << std::endl;
#endif

            std::cout << "Device type       : " << (*itr).getInfo<CL_DEVICE_TYPE>() << std::endl;
            std::cout << "Device profile    : " << (*itr).getInfo<CL_DEVICE_PROFILE>() << std::endl;
            std::cout << "Device vendor ID  : " << (*itr).getInfo<CL_DEVICE_VENDOR_ID>() << std::endl;
            std::cout << "Max compute units : " << (*itr).getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
            std::cout << "Max Work group    : " << (*itr).getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;

            std::cout << "Max WI dims       : " << (*itr).getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>() << std::endl;

            std::vector<size_t> wi_dims = (*itr).getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();

            for (size_t i = 0; i < wi_dims.size(); ++i)
            {
                std::cout << "   Dim[" << i << "]: " << wi_dims[i] << std::endl;
            }

            if ((*itr).getInfo<CL_DEVICE_TYPE>() == g_device_type)
            {
                std::cout << "Found matching device type" << std::endl;
                g_device = *itr;
            }
        }
    }
    catch(cl::Error& e)
    {
        std::cout << "set_device() failed (" << status << ") : " << e.what() << std::endl;
    }
}

/**
// static void set_default_device(void)
// {
//     g_device = cl::Device::getDefault();

//     std::cout << "Device name       : " << g_device.getInfo<CL_DEVICE_NAME>() << std::endl;
//     std::cout << "Device profile    : " << g_device.getInfo<CL_DEVICE_PROFILE>() << std::endl;
//     std::cout << "Device vendor ID  : " << g_device.getInfo<CL_DEVICE_VENDOR_ID>() << std::endl;
//     std::cout << "Max compute units : " << g_device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
//     std::cout << "Max Work group    : " << g_device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;

//     std::cout << "Max WI dims       : " << g_device.getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>() << std::endl;

//     std::vector<size_t> wi_dims = g_device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();

//     for (size_t i = 0; i < wi_dims.size(); ++i)
//     {
//         std::cout << "   Dim[" << i << "]: " << wi_dims[i] << std::endl;
//     }
// }
*/

static void set_command_queue(void)
{
    cl_int status = CL_SUCCESS;
    cl_command_queue_properties qprops = 0;

    try
    {
        qprops |= CL_QUEUE_PROFILING_ENABLE;

        g_command_queue = cl::CommandQueue(g_context,
                                           g_device,
                                           qprops,
                                           &status);
    }
    catch(cl::Error& e)
    {
        std::cout << "set_command_queue() failed (" << status << ") : " << e.what() << std::endl;
    }
}


static void set_buffers(void)
{
    cl_int status = CL_SUCCESS;

    try
    {
        const size_t total_bytes = g_num_particles * sizeof(real2);

        // buffer on GPU
        // accessed by GPU kernel at very high bandwidths
        // see https://rocmdocs.amd.com/en/latest/Programming_Guides/Opencl-optimization.html#regular-device-buffers
        g_particles_buf = cl::Buffer(g_context,
                                     CL_MEM_READ_WRITE,
                                     total_bytes,
                                     g_particles);

        // it remains pinned as long as we don't use it as kernel argument
        // see https://rocmdocs.amd.com/en/latest/Programming_Guides/Opencl-optimization.html#pre-pinned-buffers
        // should g_particles be 256-byte aligned??
        g_pinned_output_buf = cl::Buffer(g_context,
                                         CL_MEM_USE_HOST_PTR,
                                         total_bytes,
                                         g_particles);
    }
    catch(cl::Error& e)
    {
        std::cout << "set_buffers() failed (" << status << ") : " << e.what() << std::endl;
    }
}


static void set_data(void)
{
    // memory size in Bytes for particles
    const size_t total_bytes = g_num_particles * sizeof(real2);

    // a buffer to be used as float4* must be 128-bit aligned.
    // see https://rocmdocs.amd.com/en/latest/Programming_Guides/Opencl-optimization.html#using-the-cpu
    // here we use 8-byte alignment because buffer is used as float2*
    g_particles = static_cast<real*>( aligned_alloc(8, total_bytes) );

    std::random_device rd;
    static std::uniform_real_distribution<real> xdis(0, 1);
    static std::uniform_real_distribution<real> ydis(2, 3);
    static std::default_random_engine generator(rd());

    for (cl_uint i = 0; i < g_num_particles * 2; i += 2)
    {
        g_particles[i]   = xdis(generator);
        g_particles[i+1] = ydis(generator);
    }
}


int main(int argc, char * argv[])
{
    set_platform();
    set_context();
    set_device();
    set_command_queue();
    set_data();
    set_buffers();

    for (int i = 0; i < 64 * 2; i += 2)
    {
        std::cout << g_particles[i] << ", " << g_particles[i+1] << std::endl;
    }

    return 0;
}