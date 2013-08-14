@echo off

::
:: Call this the first time you sync to the source code or any time
:: you update the Anarchy SDK
::

set _ROOT=%~dp0
set _UPDATE=%_ROOT%\Source\BuildSystem

%_UPDATE%\update.py --project=%_ROOT% --assets

pause