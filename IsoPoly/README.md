# How to compile on Windows using Microsoft Visual Studio

1. Build openVDB: https://github.com/AcademySoftwareFoundation/openvdb
	* There are many dependencies. The simplest way is to install them via vcpkg: https://vcpkg.io/en/getting-started.html
	* If you are running vcpkg for the first time, it will take a while to install all the necessary packages.
	* Build the x64/Release version of openVDB.
	
2. Build IsoPoly: 
	* Open the IsoPoly solution file
	* Set project settings to point to openVDB. Alternatively, you can:
		* Copy the `openvdb/openvdb/openvdb` folder to the IsoPoly folder: `IsoPoly/openvdb` 
		* Copy the `openvdb/build/openvdb/openvdb/openvdb/version.h` to the IsoPoly folder: `IsoPoly/openvdb/openvdb`
		* Copy the `openvdb/build/openvdb/openvdb/Release` folder to `IsoPoly/openvdb/build/openvdb/Release`
	* Build x64/Release of IsoPoly
	
# How to run on Windows

Ensure the following dll files are accesible by `IsoPoly.exe`: `blosc.dll`, `lz4.dll`, `openvdb.dll`, `tbb.dll`, `zlib1.dll`, `zstd.dll`.
The simplest way is to copy them from the openVDB build, i.e., copy the dll's from `openvdb/build/openvdb/openvdb/Release` to the folder with `IsoPoly.exe`.

You will also need to provide a 3D image stack in RAW format and its properties: width, height, depth, and the bit depth.

Usage: `IsoPoly.exe <raw file> <width> <height> <depth> <isovalue> <bits (8/16)> <downsample ratio> <x sectors> <y sectors> <z sectors> <swap endianness (0/1)>`

- `<raw file>` is the name of the 3D image stack in RAW format
- `<width>/<height>/<depth>` are the x/y/z dimensions of the image stack
- `<isovalue>` is an 8/16-bit integer, specifying the iso threshold for generating a mesh
- `<bits>` is the bit depth: 8-bit or 16-bit depth
- `<downsample ratio>` averages cells in a NxNxN region into 1 cell
- `<x/y/z sectors>` separates the mesh into a grid of dimension X by Y by Z, generating a different PLY for each. Setting all to 1 will generate just one mesh
- `<swap endianness>` swaps the order of bytes of a word in the image stack

Example execution:
	`IsoPoly.exe .\input\CT-Head-LSB.raw 256 256 113 1232 16 2 1 1 1`
