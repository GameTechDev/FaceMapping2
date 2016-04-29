Mapping an Intel® RealSense™ SDK Face Scan to a 3D Head Model
======================================================

The face mapping sample uses the Intel RealSense™ 3D Scan module to scan the user's face and then map it onto an existing 3D head model. This is an improvement on a previous code sample sample that used a "stone faced" mapping technique. In this sample the following features have been added:

- Parameterized head. The user can shape the head using a series of sliders. For example, the user can change the width of the head, ears, jaw, and so on.
- Standardized mesh topology. The mapping algorithm can be applied to a head mesh with a standardized topology and can retain the context of each vertex after the mapping is complete. This paves the way for animating the head, improved blending, and post-mapping effects.
- Color and shape post processing. Morph targets and additional color blending can be applied after the mapping stages are complete to customize the final result.
- Hair. There are new hair models created to fit the base head model. A custom algorithm is used to adjust the hair geometry to conform to the user’s chosen head shape.

For detailed information on this sample, please visit:
https://software.intel.com/en-us/articles/mapping-an-intel-realsense-sdk-face-scan-to-a-3d-head-model

The previous version of the code sample is available here:
https://github.com/GameTechDev/FaceMapping

Build Instructions
==================
The facescan2.sln solution should be built with Visual Studio 2012 or greater. There are 2 build configurations types available, the kind that support scanning faces, and the ones that only supports face mapping of a pre-scanned face. The configurations with the "_NO_RS" prefix don't support face scanning and can be built and run without the RealSense™ SDK or RealSense™ camera

Requirements
============
- Windows 8.1 or Windows 10
- Visual Studio 2012 or higher
- Intel® RealSense™ SDK (R5 release) or newer *
- RealSense™ F200 or SR300 Camera*

* These are only required for scanning new faces. The sample includes a single pre-scanned face for experimenting.

The Intel® RealSense™ SDK can be download here:
https://software.intel.com/en-us/intel-realsense-sdk/download


 