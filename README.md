osmpbf
======
This is a simple C++ library to parse OpenStreetMap's PBF files. Have a look at the examples to see it in action.


## Building

### Building on Windows
* First clone the repository and then download dependencies by running
```
git submodule init
git submodule update --recursive
```

* Compiling dependencies

Don´t forget to set Code generation->runtime library to Multi-threaded(/Mt) for Release or Multi-threaded Debug(/Mtd) for Debug for every solution.      
Mman can be compiled without problem, just open the .vcxproj file and build as usual.   
Use CMake to compile Protobuf(build just protoc solution;CmakeFiles are located in cmake subdirectory; on error just disable the protobuf_BUILD_TESTS option) and Zlib.

* Compiling proto files

Once you have compiled Protobuf you need to run protoc.exe located in osmpbf\windows\protobuf\cmake\build\Release
```
windows\protobuf\cmake\build\Release\protoc.exe osmpbf\osmblob.proto --cpp_out=.
windows\protobuf\cmake\build\Release\protoc.exe osmpbf\osmformat.proto --cpp_out=.
```

* Compiling osmpbf

If you are using VS 2013, you need to download and install https://www.microsoft.com/en-us/download/details.aspx?id=41151 and change your "Platform Toolset" to "Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013)" to use the new compiler. You can do that by opening your project's "Property Pages" And going to: "Configuration Properties" > "General" and then changing the "Platform Toolset".  
Create New -> Project from existing source and schoose just osmpbf as include directory. Once the solution is created, just manually add file from examples directory.

* Linking libraries

You need to set C/C++ -> Additional include directories to
```
.
osmpbf
osmpbf\include
windows\protobuf\src
windows\zlib\build
```
For Release set your Linker -> Additional Library directories to
```
windows\protobuf\cmake\build\Release
windows\mman\x64\Release
windows\zlib\build\Release
```
For Debug set your Linker -> Additional Library directories to
```
windows\protobuf\cmake\build\Debug
windows\mman\x64\Debug
windows\zlib\build\Debug
```
For Release set Linker -> input to
```
zlib.lib
Ws2_32.lib
libprotobuf.lib
mman.lib
```
For Debug set Linker -> input to
```
zlibd.lib
Ws2_32.lib
libprotobufd.lib
mman.lib
```

* Troubleshooting

If you get any unresolved external symbol errors, its because you didn´t compile all libraries the same way - check Code generation->runtime library and recompile the libraries. Sometimes libprotobuf doesn´t recompile properly when you just hit rebuild solution. The best is to delete the whole Release or Debug folder.

* Enabling OpenMP

To enable OpenMP, just go to C/C++ -> Language -> Open MP Support and choose Yes.



