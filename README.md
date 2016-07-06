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
Don´t forget to set Code generation->runtime library to Multi-threaded(/Mt) for Release and Multi-threaded Debug(/Mtd)

Mman can be compiled without no problem, just open the .vcxproj file and build as usual. 

Use CMake to compile Protobuf(CmakeFiles are located in cmake subdirectory) and Zlib.

*Compiling proto files
Once you have compiled Protobuf you need to run protoc.exe located in osmpbf\windows\protobuf\cmake\build\Release
```
windows\protobuf\cmake\build\Release\protoc.exe osmpbf\osmblob.proto --cpp_out=osmpbf
windows\protobuf\cmake\build\Release\protoc.exe osmpbf\osmformat.proto --cpp_out=osmpbf

```

* Compiling osmpbf
If you are using VS 2013, you need to download and install https://www.microsoft.com/en-us/download/details.aspx?id=41151 and change your "Platform Toolset" to "Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013)" to use the new compiler. You can do that by opening your project's "Property Pages" And going to: "Configuration Properties" > "General" and then changing the "Platform Toolset".

* Linking libraries
You need to set C/C++ -> Additional include directories to
```
osmpbf
osmpbf\include
windows\protobuf\src
windows\zlib\build
```
your Linker -> Additional Library directories to
```
windows\protobuf\cmake\build\Release
windows\mman\x64\Release
windows\zlib\build\Release
```


