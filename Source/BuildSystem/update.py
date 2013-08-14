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
update.py - Get the newest assets from $(VISION_SDK) and process the assets
"""

import sys
import os
import shutil
import subprocess

from optparse import OptionParser

COMMAND_LINE_OPTIONS = (
    (('-v', '--vision',),
     {'action': 'store',
      'dest': 'vision',
      'help': "Vision SDK directory"}),
    (('-p', '--project',),
     {'action': 'store',
      'dest': 'project',
      'help': "Project root directory"}),
    (('-q', '--quiet',),
     {'action': 'store_false',
      'dest': 'verbose',
      'default': True,
      'help': "Don't print out status updates"}))

LIBRARIES = ['fmodex', 'lua100', 'Base', 'BaseUI', 'Vision',
             'CSharpFramework', 'ManagedFramework', 'VisionEnginePlugin',
             'vHavok', 'vFmodEnginePlugin', 'vRemotePlugin']

def run(arguments, verbose=False, current_directory=""):
    if current_directory == "":
        current_directory = os.path.dirname(arguments[0])

    disable_capture = False
    
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

def update_files(source_path, output_path):
    for filename in os.listdir(source_path):
        copy_file = False

        for library in LIBRARIES:
            if filename.startswith(library):
                copy_file = True
                break

        if copy_file:
            source_file = os.path.join(source_path, filename)
            destination_file = os.path.join(output_path, filename)
            if os.path.exists(destination_file) and\
                (os.stat(source_file).st_mtime - os.stat(destination_file).st_mtime) > 1:
                shutil.copy2(source_file, output_path)
                print('Updating %s...' % filename)
            else:
                print('Skipping %s...' % filename)

def main():
    """
    Update the resources that are available and copy over newest assets.
    """
    
    parser = OptionParser('')
    for options in COMMAND_LINE_OPTIONS:
        parser.add_option(*options[0], **options[1])
    (options, _) = parser.parse_args()
    
    project_directory = options.project
    if not project_directory and len(sys.argv) > 1:
        project_directory = sys.argv[len(sys.argv) - 1]

    success = False

    if not 'VISION_SDK' in os.environ:
        print("Missing VISION_SDK environment variable!")

    vision_directory = os.environ['VISION_SDK']

    if not project_directory:
        project_directory = os.path.dirname(os.path.realpath(__file__))
        project_directory = os.path.abspath(os.path.join(project_directory, '../../'))
    else:
        project_directory = os.path.abspath(project_directory)

    arch = 'win32_vs2010_anarchy'

    conf_dev = 'Bin\%s\dev_dll\DX9' % arch
    update_files( os.path.join(vision_directory, conf_dev), os.path.join(project_directory, conf_dev) )
    
    conf_debug = 'Bin\%s\debug_dll\DX9' % arch
    update_files( os.path.join(vision_directory, conf_debug), os.path.join(project_directory, conf_debug) )
    
    print('Updating assets...')
    asset_processor = os.path.join(vision_directory, '%s\AssetProcessor.exe' % conf_dev)
    run([asset_processor, '--removestale=1', '--transform=1', '--all=1', '%s\Assets' % project_directory],
        False,
        project_directory)

    success = True

    return success

if __name__ == "__main__":
    SUCCESS = main()
    sys.exit(0 if SUCCESS else 1)