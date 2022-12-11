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