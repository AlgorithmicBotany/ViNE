# ViNE
 
## How to run

Clone this repository or download a release version and extract the ZIP archive.

Run a test with a simple unsegmented icosahedron mesh: `./ViNE.exe untrackedmodels/icosahedron.ply`

To run ViNE with already segmentated meshes:
* Copy the mesh, e.g., the PLY file, to the `untrackedmodels` folder.
* Copy the segmentation, saved as a CLR file, to the `saved` folder.
* Run ViNE with saved segmentation: `./ViNE.exe saved/{NAME_OF_YOUR_FILE}.clr`

Known issues:
* If the controllers are not found, try opening this URL while running ViNE.exe: http://localhost:27062/dashboard/controllerbinding.html
This should take you to the controller bindings menu. If there is a controller binding named “VINE Vive Bindings”, activate that binding.

* The model in its initial position maybe very large and far out of view. Try shrinking the model with the controller grip buttons.
It may also be behind you.

* ViNE will crash if the VR headset or controllers are not on and active when it is first launched.
Ensure the headset and controllers are not in standby mode.

* If the model and segmentation do not load, ensure the files are placed in the correct folders.
Alternatively, check the first line of the CLR file to ensure it specifies the location of the 3D mesh file (e.g., the PLY file).

## How to compile on Windows using Microsoft Visual Studio

1. Install the OpenVR SDK: https://github.com/ValveSoftware/openvr
	* The openvr_api.lib/.dll files are included with the source code but if you want a newer version, the simplest way is to install openvr via vcpkg: `vcpkg.exe install openvr:x64-windows`. See https://vcpkg.io/en/getting-started.html for more information on vcpkg.

2. Build the included Rendering Engine Library
	* Open `./ViNE/RenderingEngineLibrary/RenderingEngine.sln`, select Release x64, then Build Solution.
	* Note: the OpenVR and FreeType library files are included in the `lib` folder of the Rendering Engine Library.

3. Build ViNE
	* Open `./ViNE/ViNE/ViNE.sln`, select Release x64, then Build Solution.
	* If everything was successful, the ViNE software package can be found in `./ViNE/ViNE/BUILD_PACKAGE/x64/Release`.

# IsoPoly

Included with ViNE is a program to create polygon meshes from 3D image stacks. Please see the README.md file in the `IsoPoly` folder for instructions.

# Citation

ViNE and IsoPoly were developed by Jeremy Adam Hart as part of his M.Sc. thesis at the University of Calgary: http://algorithmicbotany.org/papers/hart.th2020.html

Please use the following citation:
```
@mastersthesis{Hart2020ivm,
	Author = {Jeremy Adam Hart},
	School = {University of Calgary},
	Title = {Interactive Visualization and Modeling of Plant Structures in Virtual Reality},
	Year = {2022}}
```
