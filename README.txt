Using Text Editor and Command Prompt
=====================================
To compile for windows(commandline instead of Visual Studio) or Linux:
Open Command Prompt and navigate to this directory
mkdir out
cd out
cmake ../
cmake --build . --Release


Using Visual Studio
===================
Right Click on project source folder and Open With Visual Studio.
Or open VS and File->Open->Folder
Select WINDOWS_32 or WINDOWS_64
Then Build->Build All

Note: You must have CMake installed on system. (Version 3.16 or higher required)