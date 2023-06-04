#!/usr/bin/env python

"""
Script to build a "binary wheel" for the 'pip' Python package manager for
the LAMMPS python module which includes the shared library file. After a
successful build the script attempts to install the wheel into a system
specific site-packages folder or - failing that - into the corresponding
user site-packages folder.  Called from the 'install-python' build target
in the GNU make and CMake based build systems.  Can also be called
independently and used to build the wheel without installing it.
"""

from __future__ import print_function
import sys,os,shutil,glob,subprocess
from argparse import ArgumentParser

parser = ArgumentParser(prog='install.py',
                        description='LAMMPS python package installer script')

parser.add_argument("-p", "--package", required=True,
                    help="path to the LAMMPS Python package")
parser.add_argument("-l", "--lib", required=True,
                    help="path to the compiled LAMMPS shared library")
parser.add_argument("-n", "--noinstall", action="store_true", default=False,
                    help="only build a binary wheel. Don't attempt to install it")
parser.add_argument("-w", "--wheeldir", required=False,
                    help="path to a directory where the created wheel will be stored")

args = parser.parse_args()

# validate arguments and make paths absolute

if args.package:
  if not os.path.exists(args.package):
    print("ERROR: LAMMPS package %s does not exist" % args.package)
    parser.print_help()
    sys.exit(1)
  else:
    args.package = os.path.abspath(args.package)

if args.lib:
  if not os.path.exists(args.lib):
    print("ERROR: LAMMPS shared library %s does not exist" % args.lib)
    parser.print_help()
    sys.exit(1)
  else:
    args.lib = os.path.abspath(args.lib)

if args.wheeldir:
  if not os.path.exists(args.wheeldir):
    print("ERROR: directory %s to store the wheel does not exist" % args.wheeldir)
    parser.print_help()
    sys.exit(1)
  else:
    args.wheeldir = os.path.abspath(args.wheeldir)

# we need to switch to the folder of the python package
olddir = os.path.abspath('.')
os.chdir(os.path.dirname(args.package))

# remove any wheel files left over from previous calls
print("Purging existing wheels...")
for wheel in glob.glob('lammps-*.whl'):
  print("deleting " + wheel)
  os.remove(wheel)

# copy shared object to the current folder so that
# it will show up in the installation at the expected location
os.putenv('LAMMPS_SHARED_LIB',os.path.basename(args.lib))
shutil.copy(args.lib,'lammps')

# create a virtual environment for building the wheel
shutil.rmtree('buildwheel',True)
try:
  txt = subprocess.check_output([sys.executable, '-m', 'venv', 'buildwheel'], stderr=subprocess.STDOUT, shell=False)
  print(txt.decode('UTF-8'))
except subprocess.CalledProcessError as err:
  sys.exit("Failed to create a virtual environment: {0}".format(err.output.decode('UTF-8')))

# now run the commands to build the wheel. those must be in a separate script
# and run in subprocess, since this will use the virtual environment and
# there is no simple way to return from that in python.
os.system(sys.executable + ' makewheel.py')

# remove temporary folders and files
shutil.rmtree('buildwheel',True)
shutil.rmtree('build',True)
shutil.rmtree('lammps.egg-info',True)
os.remove(os.path.join('lammps',os.path.basename(args.lib)))

# stop here if we were asked not to install the wheel we created
if args.noinstall:
  if args.wheeldir:
    for wheel in glob.glob('lammps-*.whl'):
      shutil.copy(wheel, args.wheeldir)
      os.remove(wheel)
  exit(0)

# install the wheel with pip. first try to install in the default environment.
# that will be a virtual environment, if active, or the system folder.
# if in a virtual environment, we must not use the python executable
# that is running this script (configured by cmake), but use "python"
# from the regular system path. The user may have changed to the virtual
# environment *after* running cmake.
# recent versions of pip will automatically drop to use the user folder
# in case the system folder is not writable.

# we use a subprocess so we can catch an exception on failure.
# we need to check whether pip refused to install because of a
# version of the module previously installed with distutils. those
# must be uninstalled manually. We must not ignore this and drop
# back to install into a (forced) user folder.

if "VIRTUAL_ENV" in os.environ:
  print("Installing wheel into virtual environment")
  py_exe = 'python'
else:
  print("Installing wheel into system site-packages folder")
  py_exe = sys.executable

for wheel in glob.glob('lammps-*.whl'):
  try:
    txt = subprocess.check_output([py_exe, '-m', 'pip', 'install', '--force-reinstall', wheel], stderr=subprocess.STDOUT, shell=False)
    print(txt.decode('UTF-8'))
    if args.wheeldir:
      shutil.copy(wheel, args.wheeldir)
    else:
      shutil.copy(wheel, olddir)
    os.remove(wheel)
    continue
  except subprocess.CalledProcessError as err:
    errmsg = err.output.decode('UTF-8')
    if errmsg.find("distutils installed"):
      sys.exit(errmsg + "You need to uninstall the LAMMPS python module manually first.\n")
  try:
    print('Installing wheel into system site-packages folder failed. Trying user folder now')
    txt = subprocess.check_output([sys.executable, '-m', 'pip', 'install', '--user', '--force-reinstall', wheel], stderr=subprocess.STDOUT, shell=False)
    print(txt.decode('UTF-8'))
    if args.wheeldir:
      shutil.copy(wheel, args.wheeldir)
    else:
      shutil.copy(wheel, olddir)
    os.remove(wheel)
  except:
    sys.exit('Failed to install wheel ' + wheel)
