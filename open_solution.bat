@echo off

set VISION_SDK=C:\Havok\2013_2_Branch

set SOLVER_SOLUTION=%~dp0\Workspace\2D_Toolset_Win32_VS2010_DX9_All.sln
set DEVENV="C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe"

echo Opening Solution...
start %DEVENV% %SOLVER_SOLUTION%