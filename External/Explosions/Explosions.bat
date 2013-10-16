@echo off

set ROOT=%~dp0..\..
set SOURCE=%~dp0\images
set DESTINATION=%ROOT%\Assets\Textures\SpriteSheets\

set SHOEBOX="C:\Program Files (x86)\ShoeBox\ShoeBox.exe"

set SHEETNAME_V1=Explosion_v1
set SHEETNAME_V2=Explosion_v2

set INPUT_FILES_V1=%SOURCE%\explosion_v1_01.png,%SOURCE%\explosion_v1_02.png,%SOURCE%\explosion_v1_03.png,%SOURCE%\explosion_v1_04.png,%SOURCE%\explosion_v1_05.png,%SOURCE%\explosion_v1_06.png,%SOURCE%\explosion_v1_07.png,%SOURCE%\explosion_v1_08.png,%SOURCE%\explosion_v1_09.png,%SOURCE%\explosion_v1_10.png,%SOURCE%\explosion_v1_11.png,%SOURCE%\explosion_v1_12.png,%SOURCE%\explosion_v1_13.png,%SOURCE%\explosion_v1_14.png,%SOURCE%\explosion_v1_15.png,%SOURCE%\explosion_v1_16.png,%SOURCE%\explosion_v1_17.png,%SOURCE%\explosion_v1_18.png,%SOURCE%\explosion_v1_19.png,%SOURCE%\explosion_v1_20.png,%SOURCE%\explosion_v1_21.png,%SOURCE%\explosion_v1_22.png,%SOURCE%\explosion_v1_23.png,%SOURCE%\explosion_v1_24.png,%SOURCE%\explosion_v1_25.png,%SOURCE%\explosion_v1_26.png,%SOURCE%\explosion_v1_27.png,%SOURCE%\explosion_v1_28.png,%SOURCE%\explosion_v1_29.png,%SOURCE%\explosion_v1_30.png,%SOURCE%\explosion_v1_31.png,%SOURCE%\explosion_v1_32.png,%SOURCE%\explosion_v1_33.png,%SOURCE%\explosion_v1_34.png,%SOURCE%\explosion_v1_35.png,%SOURCE%\explosion_v1_36.png,%SOURCE%\explosion_v1_37.png,%SOURCE%\explosion_v1_38.png,%SOURCE%\explosion_v1_39.png,%SOURCE%\explosion_v1_40.png,%SOURCE%\explosion_v1_41.png,%SOURCE%\explosion_v1_42.png,%SOURCE%\explosion_v1_43.png,%SOURCE%\explosion_v1_44.png,%SOURCE%\explosion_v1_45.png,%SOURCE%\explosion_v1_46.png,%SOURCE%\explosion_v1_47.png,%SOURCE%\explosion_v1_48.png,%SOURCE%\explosion_v1_49.png,%SOURCE%\explosion_v1_50.png,%SOURCE%\explosion_v1_51.png

set INPUT_FILES_V2=%SOURCE%\explosion_v2_01.png,%SOURCE%\explosion_v2_02.png,%SOURCE%\explosion_v2_03.png,%SOURCE%\explosion_v2_04.png,%SOURCE%\explosion_v2_05.png,%SOURCE%\explosion_v2_06.png,%SOURCE%\explosion_v2_07.png,%SOURCE%\explosion_v2_08.png,%SOURCE%\explosion_v2_09.png,%SOURCE%\explosion_v2_10.png,%SOURCE%\explosion_v2_11.png,%SOURCE%\explosion_v2_12.png,%SOURCE%\explosion_v2_13.png,%SOURCE%\explosion_v2_14.png,%SOURCE%\explosion_v2_15.png,%SOURCE%\explosion_v2_16.png,%SOURCE%\explosion_v2_17.png,%SOURCE%\explosion_v2_18.png,%SOURCE%\explosion_v2_19.png,%SOURCE%\explosion_v2_20.png,%SOURCE%\explosion_v2_21.png,%SOURCE%\explosion_v2_22.png,%SOURCE%\explosion_v2_23.png,%SOURCE%\explosion_v2_24.png,%SOURCE%\explosion_v2_25.png,%SOURCE%\explosion_v2_26.png,%SOURCE%\explosion_v2_27.png,%SOURCE%\explosion_v2_28.png,%SOURCE%\explosion_v2_29.png,%SOURCE%\explosion_v2_30.png,%SOURCE%\explosion_v2_31.png,%SOURCE%\explosion_v2_32.png,%SOURCE%\explosion_v2_33.png,%SOURCE%\explosion_v2_34.png,%SOURCE%\explosion_v2_35.png,%SOURCE%\explosion_v2_36.png,%SOURCE%\explosion_v2_37.png,%SOURCE%\explosion_v2_38.png,%SOURCE%\explosion_v2_39.png,%SOURCE%\explosion_v2_40.png,%SOURCE%\explosion_v2_41.png,%SOURCE%\explosion_v2_42.png,%SOURCE%\explosion_v2_43.png,%SOURCE%\explosion_v2_44.png,%SOURCE%\explosion_v2_45.png,%SOURCE%\explosion_v2_46.png,%SOURCE%\explosion_v2_47.png,%SOURCE%\explosion_v2_48.png,%SOURCE%\explosion_v2_49.png,%SOURCE%\explosion_v2_50.png,%SOURCE%\explosion_v2_51.png,%SOURCE%\explosion_v2_52.png,%SOURCE%\explosion_v2_53.png,%SOURCE%\explosion_v2_54.png,%SOURCE%\explosion_v2_55.png,%SOURCE%\explosion_v2_56.png

set FORMAT_LOOP="fileFormatLoop=\t<SubTexture name=\"@id\" x=\"@x\" y=\"@y\" width=\"@w\" height=\"@h\" ox=\"@fx\" oy=\"@fy\" original_width=\"@fw\" original_height=\"@fh\"/>\n"
set FORMAT_OUTER="fileFormatOuter=<TextureAtlas imagePath=\"@TexName\" width=\"@W\" height=\"@H\">\n@loop</TextureAtlas>"

%SHOEBOX% "plugin=shoebox.plugin.spriteSheet::PluginCreateSpriteSheet" "files=%INPUT_FILES_V1%" "fileName=%SHEETNAME_V1%.xml" "texPadding=2" "animationNameIds=@name_###.png" "useCssOverHack=false" "animationFrameIdStart=0" %FORMAT_LOOP% "animationMaxFrames=100" "renderDebugLayer=false" "fileName=sheet.xml" %FORMAT_OUTER% "texPowerOfTwo=true" "fileGenerate2xSize=false" "texExtrudeSize=1" "texMaxSize=4096" "scale=1" "texSquare=false" "texCropAlpha=true"
xcopy /y /q %SOURCE%\%SHEETNAME_V1%.xml %DESTINATION%
xcopy /y /q %SOURCE%\%SHEETNAME_V1%.png %DESTINATION%
del %SOURCE%\%SHEETNAME_V1%.xml
del %SOURCE%\%SHEETNAME_V1%.png

%SHOEBOX% "plugin=shoebox.plugin.spriteSheet::PluginCreateSpriteSheet" "files=%INPUT_FILES_V2%" "fileName=%SHEETNAME_V2%.xml" "texPadding=2" "animationNameIds=@name_###.png" "useCssOverHack=false" "animationFrameIdStart=0" %FORMAT_LOOP% "animationMaxFrames=100" "renderDebugLayer=false" "fileName=sheet.xml" %FORMAT_OUTER% "texPowerOfTwo=true" "fileGenerate2xSize=false" "texExtrudeSize=1" "texMaxSize=4096" "scale=1" "texSquare=false" "texCropAlpha=true"
xcopy /y /q %SOURCE%\%SHEETNAME_V2%.xml %DESTINATION%
xcopy /y /q %SOURCE%\%SHEETNAME_V2%.png %DESTINATION%
del %SOURCE%\%SHEETNAME_V2%.xml
del %SOURCE%\%SHEETNAME_V2%.png
