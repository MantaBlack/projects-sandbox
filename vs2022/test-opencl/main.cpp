#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 210

#include <CL/cl2.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <random>
#include <fstream>
#include <limits>   //std::numeric_limits
#include <cmath>    //std::fabs
#include <locale>   //std::locale, std::numpunct, std::use_facet

using real = cl_float;
using real2 = cl_float2;

struct space_out : std::numpunct<char>
{
    char do_thousands_sep() const
    {
        return ' ';
    }

    std::string do_grouping() const
    {
        return "\03";
    }
};

static cl_device_type g_device_type = CL_DEVICE_TYPE_GPU;
const  size_t g_block_size = 256u;

static cl::Platform     g_platform;
static cl::Context      g_context;
static cl::Device       g_device;
static cl::CommandQueue g_command_queue;
static cl::Buffer       g_input_buffer;
static cl::Buffer       g_output_buffer;
static cl::Program      g_program;
static cl::Kernel       g_kernel;

static cl_uint g_num_particles = 60 * 256 * 256;
static real* g_particles = nullptr;
static real* g_out_particles = nullptr;
static real* g_pin_particles = nullptr;

static std::vector<real2> g_host_particles(g_num_particles);

static cl::Buffer g_particles_buf;
static cl::Buffer g_pinned_output_buf;

static std::string g_kernel_file("kernel_file.cl");
static std::string g_kernel_name("test_kernel");

static bool is_close(real a, real b)
{
    // return std::fabs(a - b) < std::numeric_limits<T>::epsilon();
    return std::fabs(a - b) < std::numeric_limits<real>::epsilon();
}

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

            std::cout << "Platform name: " << g_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
            std::cout << "Platform vendor: " << g_platform.getInfo<CL_PLATFORM_VENDOR>() << std::endl;
        }

    }
    catch (cl::Error& e)
    {
        std::cout << "set_platform() failed (" << status << ") : " << e.what() << std::endl;
        exit(EXIT_FAILURE);
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
    catch (cl::Error& e)
    {
        std::cout << "set_context() failed (" << status << ") : " << e.what() << std::endl;
        exit(EXIT_FAILURE);
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

        for (itr = devices.begin(); itr != devices.end(); ++itr)
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

            cl_device_svm_capabilities svm = (*itr).getInfo<CL_DEVICE_SVM_CAPABILITIES>();
            std::cout << "Device SVM coarse grain : " << ((svm & CL_DEVICE_SVM_COARSE_GRAIN_BUFFER) ? "Yes" : "No") << std::endl;
            std::cout << "Device SVM fine grain buffer : " << ((svm & CL_DEVICE_SVM_FINE_GRAIN_BUFFER) ? "Yes" : "No") << std::endl;
            std::cout << "Device SVM fine grain system : " << ((svm & CL_DEVICE_SVM_FINE_GRAIN_SYSTEM) ? "Yes" : "No") << std::endl;
            std::cout << "Device SVM atomics : " << ((svm & CL_DEVICE_SVM_ATOMICS) ? "Yes" : "No") << std::endl;

            if ((*itr).getInfo<CL_DEVICE_TYPE>() == g_device_type)
            {
                std::cout << "Found matching device type" << std::endl;
                g_device = *itr;
            }

            std::cout << std::endl;
        }
    }
    catch (cl::Error& e)
    {
        std::cout << "set_device() failed (" << status << ") : " << e.what() << std::endl;
        exit(EXIT_FAILURE);
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
    catch (cl::Error& e)
    {
        std::cout << "set_command_queue() failed (" << status << ") : " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}


static void set_buffers(void)
{
    cl_int status = CL_SUCCESS;

    try
    {
        size_t total_bytes = g_num_particles * sizeof(real2);

        // buffer on GPU
        // accessed by GPU kernel at very high bandwidths
        // see https://rocmdocs.amd.com/en/latest/Programming_Guides/Opencl-optimization.html#regular-device-buffers
        // fails without CL_MEM_USE_HOST_PTR on Windows
        g_particles_buf = cl::Buffer(g_context,
            CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
            total_bytes,
            g_particles,
            &status);

        // it remains pinned as long as we don't use it as kernel argument
        // see https://rocmdocs.amd.com/en/latest/Programming_Guides/Opencl-optimization.html#pre-pinned-buffers
        // should g_particles be 256-byte aligned??
        g_pinned_output_buf = cl::Buffer(g_context,
            CL_MEM_USE_HOST_PTR,
            total_bytes,
            g_pin_particles,
            &status);

        // g_output_buffer = cl::Buffer(g_context,
        //                              CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
        //                              total_bytes,
        //                              g_out_particles,
        //                              &status);

        g_output_buffer = cl::Buffer(g_context,
            CL_MEM_WRITE_ONLY,
            total_bytes,
            NULL,
            &status);
    }
    catch (cl::Error& e)
    {
        std::cout << "set_buffers() failed (" << status << ") : " << e.err() << " - " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}


static void set_data(void)
{
    // memory size in Bytes for particles
    const size_t total_bytes = g_num_particles * sizeof(real2);

    // a buffer to be used as float4* must be 128-bit aligned.
    // see https://rocmdocs.amd.com/en/latest/Programming_Guides/Opencl-optimization.html#using-the-cpu
    // here we use 8-byte alignment because buffer is used as float2*
    g_particles = static_cast<real*>(_aligned_malloc(total_bytes, 8));
    g_out_particles = static_cast<real*>(_aligned_malloc(total_bytes, 8));
    g_pin_particles = static_cast<real*>(_aligned_malloc(total_bytes, 8));

    std::random_device rd;
    static std::uniform_real_distribution<real> xdis(0, 1);
    static std::uniform_real_distribution<real> ydis(2, 3);
    static std::default_random_engine generator(rd());

    if (g_particles != nullptr && g_out_particles != nullptr && g_pin_particles != nullptr)
    {
        for (cl_uint i = 0; i < g_num_particles * 2; i += 2)
        {
            g_particles[i] = xdis(generator);
            g_particles[i + 1] = ydis(generator);

            g_out_particles[i] = 0.0f;
            g_out_particles[i + 1] = 0.0f;

            g_pin_particles[i] = 0.0f;
            g_pin_particles[i + 1] = 0.0f;
        }
    }
}


static void readFileToString(const std::string filename, std::string& contents)
{
    std::ifstream stream(filename);

    if (!stream.is_open())
    {
        std::cerr << "Failed to read file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    contents = std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());

    stream.close();
}


static void create_program()
{
    cl_int status = CL_SUCCESS;

    try
    {
        std::string source_str;

        readFileToString(g_kernel_file, source_str);

        std::vector<std::string> kernel_strings{ source_str };

        std::vector<cl::Device> devices(1);
        devices[0] = g_device;

        g_program = cl::Program(g_context, kernel_strings, &status);
        g_program.build(devices, "-cl-std=CL2.0");
    }
    catch (cl::Error& e)
    {
        std::cerr << "create_program() failed (" << status << ") : " << e.err() << e.what() << std::endl;
        std::cerr << "Build status: " << g_program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(g_device) << std::endl;
        std::cerr << "Build options: " << g_program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(g_device) << std::endl;
        std::cerr << "Build log: " << g_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(g_device) << std::endl;
        exit(EXIT_FAILURE);
    }
}


static void create_kernel()
{
    cl_int status = CL_SUCCESS;

    try
    {
        g_kernel = cl::Kernel(g_program, g_kernel_name.data(), &status);

        g_kernel.setArg(0, g_particles_buf);
        g_kernel.setArg(1, g_output_buffer);
        g_kernel.setArg(2, g_block_size * sizeof(real2), NULL);
    }
    catch (cl::Error& e)
    {
        std::cout << "create_kernel() failed (" << status << ") : " << e.err() << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

static void run_kernel()
{
    cl_int status = CL_SUCCESS;
    std::vector<cl::Event> kernel_event(1);
    cl::Event data_event;

    try
    {
        size_t total_bytes = g_num_particles * sizeof(real2);

        g_command_queue.enqueueNDRangeKernel(g_kernel,
            cl::NullRange,
            cl::NDRange(g_num_particles),
            cl::NDRange(g_block_size),
            NULL,
            &kernel_event[0]);

        g_command_queue.enqueueCopyBuffer(g_output_buffer,
            g_pinned_output_buf,
            0,
            0,
            total_bytes,
            &kernel_event,
            &data_event);

    }
    catch (cl::Error& e)
    {
        std::cout << "run_kernel() failed (" << status << ") : " << e.err() << e.what() << std::endl;
        g_command_queue.finish();
        exit(EXIT_FAILURE);
    }

    g_command_queue.finish();

    cl_ulong start;
    cl_ulong finish;

    try
    {
        kernel_event[0].getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
        kernel_event[0].getProfilingInfo(CL_PROFILING_COMMAND_END, &finish);

        std::cout << "Kernel time: " << static_cast<double>((finish - start) * 1.e-9) << std::endl;

        data_event.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
        data_event.getProfilingInfo(CL_PROFILING_COMMAND_END, &finish);

        std::cout << "Data time: " << static_cast<double>((finish - start) * 1.e-9) << std::endl;
    }
    catch (cl::Error& e)
    {
        std::cout << "Failed to get profiling info (" << status << ") : " << e.err() << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}


static void run_host_kernel()
{
    real2* particles = reinterpret_cast<real2*>(g_particles);

    for (auto i = 0u; i < g_num_particles; ++i)
    {
        auto idx = (g_num_particles - 1) - i;

        g_host_particles[i] = particles[idx];
    }
}

static void verify_results()
{
    real2* device_results = reinterpret_cast<real2*>(g_pin_particles);

    for (auto i = 0; i < g_num_particles; ++i)
    {
        if (!is_close(device_results[i].s[0], g_host_particles[i].s[0]) ||
            !is_close(device_results[i].s[1], g_host_particles[i].s[1]))
        {
            std::cerr << "Verification result: FAIL" << std::endl;
            return;
        }
    }

    std::cerr << "Verification result: PASS" << std::endl;
}


int main()
{
    std::cout.imbue(std::locale(std::cout.getloc(), new space_out));

    std::cout << "# particles: " << g_num_particles << std::endl;

#ifndef HOST_ONLY
    set_platform();
    set_context();
    set_device();
    set_command_queue();
#endif

    std::cout << "Setting data" << std::endl;
    set_data();

    run_host_kernel();

#ifndef HOST_ONLY
    std::cout << "Setting buffers" << std::endl;
    set_buffers();
    create_program();
    create_kernel();
    std::cout << "Running kernel" << std::endl;
    run_kernel();
    std::cout << "Done!" << std::endl;

    verify_results();
#endif

    _aligned_free(g_particles);
    _aligned_free(g_out_particles);
    _aligned_free(g_pin_particles);
    g_particles = nullptr;
    g_out_particles = nullptr;
    g_pin_particles = nullptr;

    return 0;
}
