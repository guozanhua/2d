#! /usr/bin/python
#
# Confidential Information of Telekinesys Research Limited (t/a Havok). Not for
# disclosure or distribution without Havok's prior written consent. This
# software contains code, techniques and know-how which is confidential and
# proprietary to Havok. Product and Trade Secret source code contains trade
# secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research
# Limited t/a Havok. All Rights Reserved. Use of this software is subject to
# the terms of an end user license agreement.
#

"""
spritesheet.py - Takes a folder or series of images and generates a spritesheet using
                 ShoeBox. It therefore requires ShoeBox to be installed.
"""

import sys
import os
import shutil
import subprocess
import time

from optparse import OptionParser

COMMAND_LINE_OPTIONS = (
    (('-o', '--output',),
     {'action': 'store',
      'dest': 'output',
      'help': "Output directory for spritesheets"}),
    (('-s', '--shoebox',),
     {'action': 'store',
      'dest': 'shoebox',
      'help': "Directory where the ShoeBox executable lives"}),
    (('-q', '--quiet',),
     {'action': 'store_false',
      'dest': 'verbose',
      'default': True,
      'help': "Don't print out status updates"}))

def run(arguments, verbose=False, current_directory=""):
    if current_directory == "":
        current_directory = os.path.dirname(arguments[0])

    disable_capture = True
    
    if disable_capture:
        child = subprocess.Popen(arguments, cwd=current_directory)
    else:
        child = subprocess.Popen(arguments, shell=True, stdout=subprocess.PIPE, cwd=current_directory)

    output = ""

    if disable_capture:
        child.communicate()
    else:
        while True:
            try:
                output_character = child.stdout.read(1)

                # handle the Python 3.0 case where it's returned as a series of bytes
                if isinstance(output_character, bytes):
                    output_character = output_character.decode("utf-8")

                if output_character == '' and child.poll() != None:
                    break

                if verbose:
                    sys.stdout.write(output_character)
                    sys.stdout.flush()

                output += output_character
            except:
                # just catch everything and break out of the loop
                break

    return output

def process(shoebox, project_directory, spritesheet_output_directory, files): 
    if len(files) == 0:
        return

    directory = os.path.dirname(files[0])
    sheetname = os.path.basename(directory)
    intput_files = ','.join(files)

    print('Processing %s...' % sheetname)

    format_loop='"fileFormatLoop=\t<SubTexture name=\\"@id\\" x=\\"@x\\" y=\\"@y\\" width=\\"@w\\" height=\\"@h\\" ox=\\"@fx\\" oy=\\"@fy\\" original_width=\\"@fw\\" original_height=\\"@fh\\"/>\n" '
    format_outer='"fileFormatOuter=<TextureAtlas imagePath=\\"@TexName\\" width=\\"@W\\" height=\\"@H\\">\\n@loop</TextureAtlas>" '

    command = '"%s" "plugin=shoebox.plugin.spriteSheet::PluginCreateSpriteSheet" ' % shoebox
    command += '"files=%s" ' % intput_files
    command += '"fileName=%s.xml" ' % sheetname
    command += '"texPadding=2" "animationNameIds=@name_###.png" "useCssOverHack=false" "animationFrameIdStart=0" '
    command += format_loop
    command += '"animationMaxFrames=100" "renderDebugLayer=false" "fileName=sheet.xml" '
    command += format_outer
    command += '"texPowerOfTwo=true" "fileGenerate2xSize=false" "texExtrudeSize=1" "texMaxSize=4096" "scale=1" "texSquare=false" "texCropAlpha=true"'
    
    run(command, True, directory)
    
    output_files = ['%s\\%s.png' % (directory, sheetname), '%s\\%s.xml' % (directory, sheetname)]

    print("Output directory: %s" % spritesheet_output_directory)

    for output_file in output_files:
        if os.path.exists(output_file):
            try:
                shutil.copy2(output_file, spritesheet_output_directory)
                os.remove(output_file)
                print("Updated: %s" % os.path.basename(output_file))
            except:
                print("Failed to update: %s" % os.path.basename(output_file))

    return

def convert(options):
    success = True

    project_directory = os.path.dirname(os.path.realpath(__file__))
    project_directory = os.path.abspath(os.path.join(project_directory, '../../'))
    
    spritesheet_output_directory = '%s\\Assets\\Textures\\SpriteSheets\\' % project_directory

    shoebox = '%s\ShoeBox\ShoeBox.exe' % os.environ["ProgramFiles"]
    if not os.path.exists(shoebox):
        shoebox = '%s\ShoeBox\ShoeBox.exe' % os.environ["ProgramFiles(x86)"]
 
    if os.path.exists(shoebox):
        input_files = []
        directories = []

        for arg in sys.argv[1:]:
            if os.path.isdir(arg):
                directories.append(arg)
            elif os.path.exists(arg):
                input_files.append(arg)

        def process_files(files):
            process(shoebox, project_directory, spritesheet_output_directory, files)

        process_files(input_files)

        for directory in directories:
            files = os.listdir(directory)
            finals = []
            for file in files:
                finals.append('%s\\%s' % (directory, file))
            process_files(finals)

    time.sleep(5)

    return success

def main():    
    success = False
    options = tuple()

    try:
        parser = OptionParser('')
        for options in COMMAND_LINE_OPTIONS:
            parser.add_option(*options[0], **options[1])
        (options, _) = parser.parse_args()
    except:
        print("Parser error")
        
    success = convert(options)

    return success

if __name__ == "__main__":
    SUCCESS = main()
    sys.exit(0 if SUCCESS else 1)