@echo off

set ROOT=%~dp0..\..
set SHEETNAME=EnemyShip
set SOURCE=%~dp0
set DESTINATION=%ROOT%\Assets\Textures\SpriteSheets\

set SHOEBOX="C:\Program Files (x86)\ShoeBox\ShoeBox.exe"
set INPUT_FILES=%SOURCE%\basePose.tga,%SOURCE%\rollLeft_0001.tga,%SOURCE%\rollLeft_0002.tga,%SOURCE%\rollLeft_0003.tga,%SOURCE%\rollLeft_0004.tga,%SOURCE%\rollLeft_0005.tga,%SOURCE%\rollLeft_0006.tga,%SOURCE%\rollLeft_0007.tga,%SOURCE%\rollLeft_0008.tga,%SOURCE%\rollLeft_0009.tga,%SOURCE%\rollLeft_0010.tga,%SOURCE%\rollLeft_0011.tga,%SOURCE%\rollLeft_0012.tga,%SOURCE%\rollLeft_0013.tga,%SOURCE%\rollLeft_0014.tga,%SOURCE%\rollLeft_0015.tga,%SOURCE%\rollRight_0016.tga,%SOURCE%\rollRight_0017.tga,%SOURCE%\rollRight_0018.tga,%SOURCE%\rollRight_0019.tga,%SOURCE%\rollRight_0020.tga,%SOURCE%\rollRight_0021.tga,%SOURCE%\rollRight_0022.tga,%SOURCE%\rollRight_0023.tga,%SOURCE%\rollRight_0024.tga,%SOURCE%\rollRight_0025.tga,%SOURCE%\rollRight_0026.tga,%SOURCE%\rollRight_0027.tga,%SOURCE%\rollRight_0028.tga,%SOURCE%\rollRight_0029.tga,%SOURCE%\rollRight_0030.tga,%SOURCE%\rollRight_0031.tga
set FORMAT_LOOP="fileFormatLoop=\t<SubTexture name=\"@id\" x=\"@x\" y=\"@y\" width=\"@w\" height=\"@h\" ox=\"@fx\" oy=\"@fy\" original_width=\"@fw\" original_height=\"@fh\"/>\n"
set FORMAT_OUTER="fileFormatOuter=<TextureAtlas imagePath=\"@TexName\" width=\"@W\" height=\"@H\">\n@loop</TextureAtlas>"

%SHOEBOX% "plugin=shoebox.plugin.spriteSheet::PluginCreateSpriteSheet" "files=%INPUT_FILES%" "fileName=%SHEETNAME%.xml" "texPadding=2" "animationNameIds=@name_###.png" "useCssOverHack=false" "animationFrameIdStart=0" %FORMAT_LOOP% "animationMaxFrames=100" "renderDebugLayer=false" "fileName=sheet.xml" %FORMAT_OUTER% "texPowerOfTwo=true" "fileGenerate2xSize=false" "texExtrudeSize=1" "texMaxSize=4096" "scale=1" "texSquare=false" "texCropAlpha=true"

xcopy /y /q %SHEETNAME%.xml %DESTINATION%
xcopy /y /q %SHEETNAME%.png %DESTINATION%
del %SHEETNAME%.xml
del %SHEETNAME%.png