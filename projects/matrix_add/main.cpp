#include <iostream>
#include <vector>

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

// OpenCL includes
#include <CL/cl2.hpp>


static cl::Platform g_platform;
static cl::Context  g_context;
static cl::Device   g_device;


static void set_platform()
{
    cl_int status = CL_SUCCESS;
    std::vector<cl::Platform> platforms;

    status = cl::Platform::get(&platforms);

    if (status != CL_SUCCESS)
        throw std::string("Failed to get available platforms");

    g_platform = platforms.front();
}

static void set_device()
{
    cl_int status = CL_SUCCESS;

    status = clGetDeviceIDs(g_platform)
}


int main()
{
    try
    {
        set_platform();

        std::cout << "Platform name: "   << g_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
        std::cout << "Platform vendor: " << g_platform.getInfo<CL_PLATFORM_VENDOR>() << std::endl;
    }
    catch(cl::Error& e)
    {
        std::cerr << "ERROR: " << "Program failed" << std::endl;
    }
    catch(std::string e)
    {
        std::cerr << "ERROR: " << e << std::endl;
    }

    return 0;
}