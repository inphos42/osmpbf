osmpbf
======
This is a simple C++ library to parse OpenStreetMap's PBF files. Have a look at the examples to see it in action.


## Building

### Building on Windows
* First clone the repository and then download dependencies. The script ask for your permission to download required dependency. You can always say no if you already have it on your PC. Steps bellow are written assuming you downloaded all the dependencies with script
```
cd osmpbf
windows_setup_deps.bat
```

* Compiling dependencies

Don´t forget to set Code generation->runtime library to Multi-threaded(/Mt) for Release or Multi-threaded Debug(/Mtd) for Debug for every solution.      
Mman can be compiled without problem, just open the .vcproj file and build as usual.   
Use CMake to compile Protobuf(build just protoc solution;CmakeFiles are located in cmake subdirectory; on error just disable the protobuf_BUILD_TESTS option) and Zlib.


* Compiling osmpbf
  
Use Cmake to compile osmpbf and check if all properties below are set correctly(Hit Advanced to see all properties). Just for reference, here are example paths if you downloaded all dependencies with windows_setup_deps.bat. You need to specify absolute paths.

```
Protobuf_INCLUDE_DIR = path_to_protobuf/src
PROTOBUF_LIBRARY_RELEASE = path_to_protobuf/cmake/build/Release/libprotobuf.lib
PROTOBUF_PROTOC_EXECUTABLE = path_to_protobuf/cmake/build/Release/protoc.exe
PROTOBUF_SRC_ROOT_FOLDER = path_to_protobuf
ZLIB_INCLUDE_DIR = path_to_zlib
ZLIB_LIBRARY_RELEAE = path_to_zlib/build/Release/zlib.lib
```
Now open the osmpbf-suite.sln.  
You will need to manually add two more includes into C/C++ -> Additional include directories
```
path_to_zlib\build (for zconf.h)
path_to_mman (for mman.h)
```
If you are using VS 2013, you need to download, install https://www.microsoft.com/en-us/download/details.aspx?id=41151 and change your "Platform Toolset" to "Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013)" to use the new compiler. You can do that by opening your project's "Property Pages" And going to: "Configuration Properties" > "General" and then changing the "Platform Toolset".  
Now you you should successfully compile osmpf project.


### Linking libraries

Now that you succesfully compiled static library for osmpbf,you may include it in your project.
You need to set C/C++ -> Additional include directories to
```
ext\osmpbf\osmpbf\include
ext\osmpbf
```
Set your Linker -> Additional Library directories to
```
ext\osmpbf\build\osmpbf\{Debug | Release}
ext\osmpbf\windows\protobuf\cmake\build\{Debug | Release}
ext\osmpbf\windows\zlib\build\{Debug | Release}
ext\osmpbf\windows\mman\x64\{Debug | Release}
```
For Debug set your Linker -> Additional Library directories to
```
osmpbf.lib
libprotobufd.lib
zlibd.lib
mman.lib
Ws2_32.lib
```
For Release set Linker -> input to
```
osmpbf.lib
libprotobuf.lib
zlib.lib
mman.lib
Ws2_32.lib
```


### Memory leak detector
Authors of Protobuf assume the OS clears the memory at the end of program, so memory leak detectors like Visual Leak Detector will detect memory leaks. To fix this do
```
#include <google/protobuf/stubs/common.h>
google::protobuf::ShutdownProtobufLibrary(); //call before program exits
```

### Troubleshooting

If you get any unresolved external symbol errors, its because you didn´t compile all libraries the same way - check Code generation->runtime library for each library and recompile with correct runtime if necessary. Sometimes libprotobuf doesn´t recompile properly when you just hit rebuild solution. The best way is to always call clean solution and build again.  

### Enabling OpenMP

To enable OpenMP, just go to C/C++ -> Language -> Open MP Support and choose Yes.



