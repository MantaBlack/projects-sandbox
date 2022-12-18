# General Notes

## OpenCL on Windows

- Install MinGW for Windows and use `pacman` to install opencl headers and ICD

## Install Clang for MSYS2
- `pacman -S mingw-w64-x86_64-clang`

## Install EasyClangComplete

```javascript
 "common_flags" : [
    // some example includes
    "-I/usr/include",
    "-I$project_base_path/src",
    // this is needed to include the correct headers for clang
    "-I/usr/lib/clang/$clang_version/include",
    // For simple projects, you can add a folder where your current file is
    "-I$file_path",
    "-IC:\\msys64\\ucrt64\\include"
```

## OpenGL Tuturials

- Install GLFW: `pacman -S mingw-w64-ucrt-x86_64-glfw`
- Install GLAD: Follow instructions in tutorial
- On BIANCA, I did not copy the KHR directory to include