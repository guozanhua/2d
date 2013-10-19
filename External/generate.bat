@echo off

::
:: This assumes that Python is installed and in PATH
::

set ROOT=%~dp0..
set EXTERNAL=%~dp0
set GENERATE=python %ROOT%\Source\BuildSystem\spritesheet.py

%GENERATE% %EXTERNAL%\EnemyShip
%GENERATE% %EXTERNAL%\EnemyShipV2
%GENERATE% %EXTERNAL%\Explosions\Explosion_v1
%GENERATE% %EXTERNAL%\Explosions\Explosion_v2
%GENERATE% %EXTERNAL%\HeroShip