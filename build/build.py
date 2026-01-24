#!/usr/bin/python -u
# This Python file uses the following encoding: utf-8
#
# TODO - Introducir comprobaciones de variables alineadas a memoria. Si una variable está declarada con el prefijo alignXXX, comprobar que en el fichero .map su dirección está alineada al descrito en el prefijo y fallar en caso de que no lo sea.
#
import sys
import os
import shutil
import subprocess
import threading

MAX_ALLOCS_PER_NODE = 1000000
# VERSION_SDCC = "3.2.1-8124"
# VERSION_SDCC = "3.3.0_win32"
# VERSION_SDCC = "3.5.0_win32"
# VERSION_SDCC = "3.6.0_win32"
VERSION_SDCC = "4.5.0"
VERSION_SDCC_MAJOR = "4"
VERSION_SDCC_MINOR = "5"
VERSION_SDCC_RELEASE = "0"
PATH_SDCC = ""
ASM_EXEC = ""
COMP_EXEC = ""
EXOMIZER_EXE = ""
HEX2BIN_EXE = ""

def showHowToUse():
  print ('')
  print ('Usage:')
  print ('')
  print ('\tbuild.py PATH_TO_PROJECT_ROOT')
  print ('')
  print ('NOTE: The root directory MUST contain a build.settings file (in UTF-8 encoding) containing:')
  print ('')
  print ('\tProjectName  - Executable name.')
  print ('\tIntermediatesDir    - Intermediates directory. The output, object, dsk and temp dir are calculated relative to this directory.')
  print ('\tOutputDir    - Executable output directory.')
  print ('\tObjectDir    - Temporary directory for intermediate files.')
  print ('\tSourcesDir   - Directory with the source files. Only those files with .c extension will be taken in account.')
  print ('\tResourcesDir - Directory with the resource files. All the files that exist at the root level of that directory will be processed by the exomizer compressor.')
  print ('\tDskDir       - Directory where the .dsk file will be generated.')
  print ('\tIncDir       - Directories where additional include & sources will be searched for and compiled if proceed.')
  print ('\tLibDir       - Directories where the libraries will be searched for.')
  print ('\tLibLink      - Directories where the libraries will be searched for.')
  print ('\tDataAddress  - Address where the program data starts. In Hexadecimal.')
  print ('')
  print ('For example:')
  print ('')
  print ("\t# This Python file uses the following encoding: utf-8")
  print ("\tProjectName = 'HiWorld'")
  print ("\tOutputDir = '..\\..\\bin'")
  print ("\tObjectDir = '.\\obj'")
  print ("\tSourcesDir = '.\\src'")
  print ("\tResourcesDir = '.\\src\\data'")
  print ("\tDskDir = '.\\dsk'")
  print ("\tIncDir = ['..\\common\\include', '..\\common\\source']")
  print ("\tLibDir = ['..\\..\\ThirdParty\\cpcrslib\\lib',]")
  print ("\tLibLink = ['cpcrslib.lib']")
  print ("\tDataAddress = '0xA000'")
  print ('')

class Logger(threading.Thread):
  file = None
  readPipe = None
  writePipe = None
  readerPipe = None
  IGNORE_MSGS = ['Warning: z80instructionSize() failed to parse line node ld',]

  def __init__(self, filename):
    threading.Thread.__init__(self)
    self.file = open(filename, 'w+')
    self.readPipe, self.writePipe = os.pipe()
    self.readerPipe = os.fdopen(self.readPipe)
    self.daemon = True
    self.start()

  def run(self):
    while True:
      line = self.readerPipe.readline()
      if len(line) == 0:
        if line[-1] == os.linesep():
          line = line[:-1]
        continue
      bIgnore = False
      for ignoreMsg in self.IGNORE_MSGS:
        if line.startswith(ignoreMsg):
          bIgnore = True
          break

      if bIgnore == False:
        self.write(line)

  def write(self, text):
    print (text)
    self.file.write(text)
    self.file.flush()
    os.fsync(self.file)

  def fileno(self):
    return self.writePipe

class Project:

  Name = ''
  DirPrj = ''
  DirIntermediates = ''
  DirOutput = ''
  DirObject = ''
  DirSources = []
  DirResources = []
  DirDsk = ''
  DirIncludes = []
  DirLibs = []
  LinkLibs = []
  _loadAddress = ''
  _startAddress = ''
  DataAddress = ''
  SettingsFile = ''
  LastBuildFile = ''

  _dskFilename = ''
  _binFilename = ''
  _symFilename = ''
  _lastBuild = '1'
  _nextBuild = ''

  _map_current = {}
  _map_past = {}

  _logger = None

  def __init__(self, prjDir):
    self.DirPrj = prjDir
    self.SettingsFile = os.path.join(self.DirPrj, 'build.settings')
    self.LastBuildFile = os.path.join(self.DirPrj, 'last_build')
    self._logger = Logger(os.path.join(SYSTEM_BUILD_DIR, 'build.log'))

    assert self.existDir(self.DirPrj), '[ERROR] Specified project does not exist. Project Dir: ' + self.DirPrj

    assert self.existFile(self.SettingsFile), '[ERROR] Settings file does not exist for project. Project Dir: ' + self.DirPrj

    if not self.existFile(self.LastBuildFile):
      with open(self.LastBuildFile, "w") as fLastBuildFile:
        fLastBuildFile.write(str(self._lastBuild))
        self._nextBuild = str(self._lastBuild)
    else:
      with open(self.LastBuildFile, 'r') as fLastBuildFile:
        self._lastBuild = fLastBuildFile.readline()
        self._nextBuild = str(int(self._lastBuild) + 1)

      with open(self.LastBuildFile, 'w') as fLastBuildFile:
        fLastBuildFile.write(str(self._nextBuild))

    self._setDirs()

  def _setDirs(self, workdir='current'):
    filePy = os.path.join(self.DirPrj, os.path.splitext(os.path.basename(self.SettingsFile))[0] + 'Settings.py')

    shutil.copy(self.SettingsFile, filePy)
    from buildSettings import IntermediatesDir, ProjectName, OutputDir, ObjectDir, TempDir, SourcesDir, ResourcesDir, DskDir, IncDir, LibDir, LibDir, LibLink, DataAddress
    os.remove(filePy)

    self.DirIntermediates = self.getRealPath(os.path.join(IntermediatesDir, workdir))
    self.Name = ProjectName
    self.DirTemp = self.getRealPath(os.path.join(self.DirIntermediates, TempDir))
    self.DirOutput = self.getRealPath(os.path.join(self.DirIntermediates, OutputDir))
    self.DirObject = self.getRealPath(os.path.join(self.DirIntermediates, ObjectDir))
    self.DirSources = self.getRealPath(ensureList(SourcesDir))
    self.DirResources = self.getRealPath(ensureList(ResourcesDir))
    self.DirDsk = self.getRealPath(os.path.join(self.DirIntermediates, DskDir))
    self.DirIncludes = self.getRealPath(ensureList(IncDir))
    self.DirLibs = self.getRealPath(ensureList(LibDir))
    self.LinkLibs = ensureList(LibLink)
    self.DataAddress = DataAddress
    self._binFilename = os.path.join(self.DirOutput, self.Name) + '.bin'
    self._dskFilename = os.path.join(self.DirDsk, self.Name) + '.dsk'
    self._symFilename = os.path.join(self.DirObject, self.Name) + '.noi'

    # Export the vars for availability in subprocesses.
    os.environ['CPC_BUILD_DIR_PROJECT'] = os.path.abspath(self.DirPrj)
    os.environ['CPC_BUILD_PROJECT_NAME'] = self.Name
    os.environ['CPC_BUILD_DIR_INTERMEDIATES'] = self.DirIntermediates
    os.environ['CPC_BUILD_DIR_TEMP'] = self.DirTemp
    os.environ['CPC_BUILD_DIR_OUTPUT'] = self.DirOutput
    os.environ['CPC_BUILD_DIR_OBJECT'] = self.DirObject
    os.environ['CPC_BUILD_DIR_SOURCES'] = ';'.join(self.DirSources)
    os.environ['CPC_BUILD_DIR_RESOURCES'] = ';'.join(self.DirResources)
    os.environ['CPC_BUILD_DIR_DSK'] = self.DirDsk
    os.environ['CPC_BUILD_DIR_INCLUDES'] = ';'.join(self.DirIncludes)
    os.environ['CPC_BUILD_DIR_LIBS'] = ';'.join(self.DirLibs)
    os.environ['CPC_BUILD_FILENAME_BIN'] = self._binFilename
    os.environ['CPC_BUILD_FILENAME_DSK'] = self._dskFilename
    os.environ['CPC_BUILD_FILENAME_SYM'] = self._symFilename
    os.environ['CPC_BUILD_LAST'] = self._lastBuild
    os.environ['CPC_BUILD_NEXT'] = self._nextBuild

    # Print config
    print ("\nConfiguration:")

    cmd = COMP_EXEC + ' --version'
    (outputCmd, outputErrCmd) = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='utf-8').communicate()
    print ("\tSDCC version:\n\n%s\n" % outputCmd)

    cmd = ASM_EXEC
    (outputCmd, outputErrCmd) = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='utf-8').communicate()
    print ("\tAssembler version:\n%s\n" % '\n'.join(outputErrCmd.split('\n')[0:3]))

    cmd = EXOMIZER_EXE + ' -v'
    (outputCmd, outputErrCmd) = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='utf-8').communicate()
    print ("\tExomizer version:\n%s\n" % outputCmd)

    #print "\tSettings file:\t[%s]" % self.SettingsFile
    #print "\tLast build:\t[%s]" % os.environ['CPC_BUILD_LAST']
    #print "\tNext build:\t[%s]" % os.environ['CPC_BUILD_NEXT']
    #print ""
    #print "\tProject name:\t[%s]" % os.environ['CPC_BUILD_PROJECT_NAME']
    #print "\tIntermediates:\t[%s]" % os.environ['CPC_BUILD_DIR_INTERMEDIATES']
    #print "\tTemporary dir:\t[%s]" % os.environ['CPC_BUILD_DIR_TEMP']
    #print "\tOutput dir:\t[%s]" % os.environ['CPC_BUILD_DIR_OUTPUT']
    #print "\tObject dir:\t[%s]" % os.environ['CPC_BUILD_DIR_OBJECT']
    #print "\tSources dir:\t[%s]" % os.environ['CPC_BUILD_DIR_SOURCES']
    #print "\tResources dir:\t[%s]" % os.environ['CPC_BUILD_DIR_RESOURCES']
    #print "\t.dsk dir:\t[%s]" % os.environ['CPC_BUILD_DIR_DSK']
    #print "\tIncludes dir:\t[%s]" % os.environ['CPC_BUILD_DIR_INCLUDES']
    #print "\tLibraries dir:\t[%s]" % os.environ['CPC_BUILD_DIR_LIBS']
    #print "\tOutput binary:\t[%s]" % os.environ['CPC_BUILD_FILENAME_BIN']
    #print "\tDisk file:\t[%s]" % os.environ['CPC_BUILD_FILENAME_DSK']
    #print "\tSymbol file:\t[%s]" % os.environ['CPC_BUILD_FILENAME_SYM']

  def existDir(self, dir):
    if type(dir) is list:
      for eachDir in dir:
        if not self.existDir(eachDir):
          return False
      return True
    else:
      #print 'Checking direxist: ' + dir
      return True if (os.path.exists(dir) and not os.path.isfile(dir)) else False

  def existFile(self, file):
    return True if (os.path.exists(file) and os.path.isfile(file)) else False

  def ensureDirExist(self, dir):
    if not self.existDir(dir):
      if not os.path.isfile(dir):
        os.makedirs(dir)
      else:
        print ('File "%s" exist but it is not a directory. Aborting...')
        sys.exit(1)

  def getRealPath(self, pathRelToProject):
    if type(pathRelToProject) is list:
      relPaths = []
      for path in pathRelToProject:
        relPaths.append(os.path.realpath(os.path.normpath(os.path.join(self.DirPrj, path))))
      return relPaths
    else:
      return os.path.realpath(os.path.normpath(os.path.join(self.DirPrj, pathRelToProject)))

  def _printSettings(self):
    print ('PROJECT:\t' + self.Name)
    print ('%15s\t%s' % ("OUTDIR:", self.DirOutput))
    print ('%15s\t%s' % ("OBJDIR:", self.DirObject))
    print ('%15s\t%s' % ("TMPDIR:", self.DirTemp))
    print ('%15s\t%s\n' % ("DSKDIR:", self.DirDsk))

    print ('%15s\t%s' % ("BINFILE:", self._binFilename))
    print ('%15s\t%s' % ("DSKFILE:", self._dskFilename))
    print ('%15s\t%s\n' % ("SYMFILE:", self._symFilename))

    print ('%15s\t%s' % ("SRCDIR(s):", self.DirSources))
    print ('%15s\t%s' % ("RESDIR(s):", self.DirResources))
    print ('%15s\t%s' % ("INCDIR(s):", self.DirIncludes))
    print ('%15s\t%s' % ("LIBDIR(s):", self.DirLibs))
    print ('%15s\t%s' % ("LINKLIB(s):", self.LinkLibs))

    print ('%15s\t%s' % ("DATA ADDR(crt0 start):", self.DataAddress))

  def _prepare(self):
    self.ensureDirExist(self.DirObject)
    self.ensureDirExist(self.DirOutput)
    self.ensureDirExist(self.DirDsk)
    self.ensureDirExist(self.DirTemp)

  def _checkSettings(self):
    isProjectOk = True

    self._printSettings()
    print ('\n\n= CHECKING PROJECT SETTINGS =')
    isProjectOk = self.existDir(self.DirOutput)    if isProjectOk  else False
    isProjectOk = self.existDir(self.DirSources)   if isProjectOk  else False
    isProjectOk = self.existDir(self.DirResources)   if isProjectOk  else False
    isProjectOk = self.existDir(self.DirIncludes)  if isProjectOk  else False
    isProjectOk = self.existDir(self.DirLibs)      if isProjectOk  else False
    isProjectOk = self.existDir(self.DirDsk)       if isProjectOk  else False
    isProjectOk = self.existDir(self.DirObject)    if isProjectOk  else False

    msg = 'Directories missing:\n'
    # TODO - Add specific missing dirs.
    if not self.existDir(self.DirOutput):    msg += '\t' + self.DirOutput        + '\n'
    if not self.existDir(self.DirSources):   msg += '\t' + str(self.DirSources)  + '\n'
    if not self.existDir(self.DirResources): msg += '\t' + str(self.DirResources)  + '\n'
    if not self.existDir(self.DirIncludes):  msg += '\t' + str(self.DirIncludes) + '\n'
    if not self.existDir(self.DirLibs):      msg += '\t' + str(self.DirLibs)     + '\n'
    if not self.existDir(self.DirDsk):       msg += '\t' + self.DirDsk           + '\n'
    if not self.existDir(self.DirObject):    msg += '\t' + self.DirObject        + '\n'

    if not isProjectOk:
      print ('ERROR!')
      print (msg)

    return isProjectOk

  def build(self):

    self._clean()
    self._prepare()
    if not self._checkSettings():
      print ('Project is not correctly configured. Aborting...')
      sys.exit(1)

    self._assemble()
    self._compile() # plus assemble
    self._link()
    self._generateBinary()

    self._fillLinkMaps()

    self._generateData()
    self._generateDsk()


    # We use always "current" as intermediates dir so it always generates the intermediates files
    # with the same exact content and here we keep the intermediate builds for keeping a history
    # of the builds.
    os.rename(self.DirIntermediates, os.path.join(os.path.dirname(self.DirIntermediates), self._nextBuild))
    self._setDirs(self._nextBuild)
    self._generateStatistics()

    self._generateReport()
    self._launchDsk()

  def _clean(self):
    try:
      if self.existFile(self._dskFilename):
        os.remove(self._dskFilename)

      if self.existDir(self.DirOutput):
        shutil.rmtree(self.DirOutput)

      if self.existDir(self.DirObject):
        shutil.rmtree(self.DirObject)

      if self.existDir(self.DirTemp):
        shutil.rmtree(self.DirTemp)

    except Exception as e:
      print ('There were errors during cleaning. Reason:\n\t%s' % e)
    finally:
      #Sanity check
      if self.existFile(self._dskFilename) or self.existFile(self._binFilename) or self.existDir(self.DirObject):
        print ('There were errors while cleaning. Not all files have been cleaned up...')
        sys.exit(1)

  def _assemble(self):

    bMissingFiles = False;
    for srcDir in self.DirSources:
      filelist = os.listdir(srcDir)
      command = ASM_EXEC + ' -o ' + os.path.join(self.DirObject, '%s') + ' ' + os.path.join(srcDir, '%s')

      print ('\n= ASSEMBLING FILES =')
      print (' - Files:')
      for file in filelist:
        if file.endswith('.s'):
          print ('\t%s' % file)

          outFilename = os.path.splitext(file)[0] + '.rel'
          commandLine = command % (outFilename, file)

          print ('Assemble command: ' + commandLine)
          subprocess.Popen(commandLine, shell=True, stdout=self._logger, stderr=self._logger, encoding='utf-8').wait()
          # Check everything went fine
          outFilenamePath = os.path.join(self.DirObject, outFilename)
          if not self.existFile(outFilenamePath):
            bMissingFiles = True
          else: # Search for _BOOT0 in order to determine where _CODE section address must start.
            with open(outFilenamePath, 'r') as fp:
              for each_line in fp:
                if '_BOOT0' in each_line:
                  _tmp_split = each_line.split('size')[1].split()
                  self._startAddress = "0x%X" % (int(_tmp_split[0], 16) + int(_tmp_split[-1], 16))
                  self._loadAddress = "%X" % int(_tmp_split[-1], 16)

                  print ("\t\tSTART ADDRESS: [%s] => [_BOOT0 addr |%s| + _BOOT0 size |%s|]" % (self._startAddress, _tmp_split[-1], _tmp_split[0]))
                  print ("\t\tLOAD ADDRESS: [%s]\n" % self._loadAddress)
                  break

    if bMissingFiles:
      raise Exception('\nOutput file was not generated. Assembling error!!!')

  def _compile(self):
    COMMAND = ""

    assert self._startAddress, \
      "[ERROR] _startAddress should have already been set here.\n" \
      "Please review the build system since it must be automatically determined after assembling " \
      "the crt0 bootstrap."

    if "3.2.1" in VERSION_SDCC:
      COMMAND = '%(compiler_exec)s -c -mz80 --disable-warning 85 --oldralloc --max-allocs-per-node %(max_allocs_per_node)s --code-loc %(start_addr_code)s --data-loc %(start_addr_data)s --no-std-crt0 --callee-saves-bc -o %(out_file)s %(src_file)s '

    elif int(VERSION_SDCC_MAJOR) >= 4 or (int(VERSION_SDCC_MAJOR) == 3 and int(VERSION_SDCC_MINOR) >= 6):
      COMMAND = '%(compiler_exec)s -c -mz80 --Werror --disable-warning 59 --opt-code-speed --max-allocs-per-node %(max_allocs_per_node)s --code-loc %(start_addr_code)s --data-loc %(start_addr_data)s --no-std-crt0 --allow-unsafe-read -o %(out_file)s %(src_file)s '

    else:
      COMMAND = '%(compiler_exec)s -c -mz80 --disable-warning 85 --fno-omit-frame-pointer --opt-code-speed --max-allocs-per-node %(max_allocs_per_node)s --code-loc %(start_addr_code)s --data-loc %(start_addr_data)s --no-std-crt0 --allow-unsafe-read -o %(out_file)s %(src_file)s '

    PARAMS = {
      'compiler_exec':    COMP_EXEC,
      'start_addr_code':  self._startAddress,
      'start_addr_data':  self.DataAddress,
      'max_allocs_per_node': MAX_ALLOCS_PER_NODE,
      'out_file':         '',
      'src_file':         '',
    }

    for incDir in self.DirIncludes:
      if self.existDir(incDir):
        COMMAND += '-I ' + incDir + ' '

    print ('\n= COMPILING =')
    print (' - Files:')
    for srcDir in self.DirSources:
      fileListSrc = os.listdir(srcDir)
      for file in fileListSrc:
        if file.endswith('.c'):
          PARAMS['out_file'] = os.path.join(self.DirObject, file.split('.')[0] + '.rel')
          PARAMS['src_file'] = os.path.join(srcDir, file)
          finalCommand = COMMAND % PARAMS

          print ('\t%s' % file)

          print ('CompileCommand: ' + finalCommand)

          (outputSdcc, outputErrSdcc) = subprocess.Popen(finalCommand, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='utf-8').communicate()

          assert not 'error: ' in (outputSdcc or outputErrSdcc), '\n\tERROR: Compilation unsuccesful! Check output!\n\nSTDOUT:\n%s\n\nSTDERR:\n%s\n\n' % (outputSdcc, outputErrSdcc)
          assert self.existFile(PARAMS['out_file']), '\n\tERROR: Compilation unsuccesful! Check output!\n\nSTDOUT:\n%s\n\nSTDERR:\n%s\n\n' % (outputSdcc, outputErrSdcc)

  def _link(self):
    if "3.2.1" in VERSION_SDCC:
      COMMAND = '%(compiler_exec)s -mz80 --disable-warning 85 --oldralloc --max-allocs-per-node %(max_allocs_per_node)s --code-loc %(start_addr_code)s --data-loc %(start_addr_data)s --no-std-crt0 --callee-saves-bc -o %(out_file)s '

    elif int(VERSION_SDCC_MAJOR) >= 4 or (int(VERSION_SDCC_MAJOR) == 3 and int(VERSION_SDCC_MINOR) >= 6):
      COMMAND = '%(compiler_exec)s -mz80 --disable-warning 85 --opt-code-speed --code-loc %(start_addr_code)s --data-loc %(start_addr_data)s --no-std-crt0 --allow-unsafe-read -o %(out_file)s '

    else:
      COMMAND = '%(compiler_exec)s -mz80 --disable-warning 85 --opt-code-speed --fno-omit-frame-pointer --max-allocs-per-node %(max_allocs_per_node)s --code-loc %(start_addr_code)s --data-loc %(start_addr_data)s --no-std-crt0 --allow-unsafe-read -o %(out_file)s '

    PARAMS = {
      'compiler_exec':    COMP_EXEC,
      'start_addr_code':  self._startAddress,
      'start_addr_data':  self.DataAddress,
      'max_allocs_per_node': MAX_ALLOCS_PER_NODE,
      'out_file':         os.path.join(self.DirObject, self.Name) + '.ihx',
    }
    finalCommand = COMMAND % PARAMS
    fileCRT = '' #This must be always the first file to link against. It contains the initialization code and some other dark things.
    libDirs = ''
    libsLink = ''

    for libDir in self.DirLibs:
      if self.existDir(libDir):
        libDirs += ' -L ' + libDir + ' '

    print ('\n= LINKING =')
    print (' - Libraries:')
    for libLink in self.LinkLibs:
      print ('\t%s' % libLink)
      libsLink += ' -l ' + libLink + ' '

    print (' - Object files:')
    for objDir in ensureList(self.DirObject):
      fileListObj = os.listdir(objDir)
      fileCommands = ''
      for file in fileListObj:
        if file.endswith('.rel'):
          print ('\t%s' % file)
          if file.lower().startswith('crt'):
            fileCRT = os.path.join(self.DirObject, file)
          else:
            fileCommands += ' ' + os.path.join(self.DirObject, file)

      finalCommand += fileCRT + fileCommands + libDirs + libsLink

      print ('Link command: ' + finalCommand)
      subprocess.Popen(finalCommand, shell=True, stdout=self._logger, stderr=self._logger, encoding='utf-8').wait()

      if not self.existFile(PARAMS['out_file']):
        raise Exception('\nOutput file was not generated. Compilation error!!!')

  def _generateBinary(self):

    bMissingFiles = False
    command = HEX2BIN_EXE + ' '

    filelistSrc = os.listdir(self.DirObject)

    print ('\n= BINARY FILE GENERATION =')
    print (' - Files:')
    for file in filelistSrc:
      if file.endswith('.ihx'):
        outFile = self._binFilename #os.path.join(self.DirOutput, self.Name) + '.bin'
        srcOutFile = os.path.join(self.DirObject, self.Name) + '.bin'
        myCommand = command + os.path.join(self.DirObject, file)
        print ('\tInput: %s' % file)
        print ('\tOutput: %s' % srcOutFile)

        subprocess.Popen(myCommand, shell=True, stdout=self._logger, stderr=self._logger, encoding='utf-8').wait()
        shutil.copy(srcOutFile, outFile)
        os.remove(srcOutFile)

        if not self.existFile(outFile):
          bMissingFiles = True

    if bMissingFiles:
      raise Exception('\nOutput file was not generated. Hex2bin error!!!')

  def _compressData(self, inFile, outFile):
    CMD_EXOMIZER = EXOMIZER_EXE + ' raw -b -c %s -o %s'
    command = CMD_EXOMIZER % (inFile, outFile)

    #print 'GenerateDataCommand: ' + command
    (outputExo, outputErrExo) = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='utf-8').communicate()

    # Parse the output for compressed/uncompressed size
    outputExo += outputErrExo

    TAG_UNCOMPRESSED_LENGTH = 'Length of indata: '
    TAG_COMPRESSED_LENGTH = 'Length of crunched data: '
    FILE_UNCOMPRESSED_LENGTH = 0
    FILE_COMPRESSED_LENGTH = 0
    for outputLine in outputExo.split('\n'):
      if TAG_UNCOMPRESSED_LENGTH in outputLine:
        FILE_UNCOMPRESSED_LENGTH = outputLine.split(TAG_UNCOMPRESSED_LENGTH)[1].split(' ')[0] # + ' - 1'
        print ('\t\tUncompressed: %s bytes' % FILE_UNCOMPRESSED_LENGTH)

      if TAG_COMPRESSED_LENGTH in outputLine:
        FILE_COMPRESSED_LENGTH = outputLine.split(TAG_COMPRESSED_LENGTH)[1].split(' ')[0] # + ' - 1'
        print ('\t\tCompressed: %s bytes' % FILE_COMPRESSED_LENGTH)

    assert FILE_UNCOMPRESSED_LENGTH > 0 and FILE_COMPRESSED_LENGTH > 0, 'Error when compressing the data file: [%s]' % file

    return (FILE_COMPRESSED_LENGTH, FILE_UNCOMPRESSED_LENGTH)

  def _writeDefines(self, inFile, outFile, FILE_COMPRESSED_LENGTH, FILE_UNCOMPRESSED_LENGTH):
    file_basename = os.path.basename(inFile)
    file_ext = ''
    file_noext = '        '

    if len(inFile.split('.')) > 1:  # it contains extension
      file_ext = file_basename.split('.')[1].upper()
      for dummy in range(len(file_ext), 3):
        file_ext += ' '

    file_noext = file_basename.split('.')[0].upper()
    for dummy in range(len(file_noext), 8):
      file_noext += ' '

    defineFileSuffix = file_noext.strip() + '_' + file_ext.strip()
    outputDefineFilenameZipBufferCheck = outputDefineFilenameUncomp = '#if FILENAME_' + defineFileSuffix + '_SIZE_UNCOMPRESSED > ZIP_BUFFER_SIZE\n# error The zipped file is too big for the current memory buffer!!!\n#endif'
    outputDefineFilename = '#define FILENAME_' + defineFileSuffix + ' "' + file_noext + file_ext + '"'
    outputDefineFilenameUncomp = '#define FILENAME_' + defineFileSuffix + '_SIZE_UNCOMPRESSED ' + FILE_UNCOMPRESSED_LENGTH
    outputDefineFilenameComp = '#define FILENAME_' + defineFileSuffix + '_SIZE_COMPRESSED ' + FILE_COMPRESSED_LENGTH

    outFile.write(outputDefineFilename + '\n')
    outFile.write(outputDefineFilenameUncomp + '\n')
    outFile.write(outputDefineFilenameComp + '\n')
    outFile.write('\n')
    if inFile.endswith('.zip'):
      outFile.write(outputDefineFilenameZipBufferCheck + '\n')

  def _getFilenamePadded(self, inFile):
    file_basename = os.path.basename(inFile)
    file_ext = ''
    file_noext = '        '

    if len(inFile.split('.')) > 1:  # it contains extension
      file_ext = file_basename.split('.')[1].upper()
      for dummy in range(len(file_ext), 3):
        file_ext += ' '

    file_noext = file_basename.split('.')[0].upper()
    for dummy in range(len(file_noext), 8):
      file_noext += ' '

    return (file_noext, file_ext)

  def _writeDefinesZip(self, inFile, outFile, inFiles):

    (inFileNoExt, inFileExt) = self._getFilenamePadded(inFile)
    defineFileSuffix = inFileNoExt.strip() + '_' + inFileExt.strip()

    offset = 0
    indexTableOffsets = 'const U16 g_%s_Offsets[%s] = {\n\t' % (defineFileSuffix, len(inFiles))
    indexTableSizeComp = 'const U16 g_%s_Size_Comp[%s] = {\n\t' % (defineFileSuffix, len(inFiles))
    indexTableSizeUncomp = 'const U16 g_%s_Size_Uncomp[%s] = {\n\t' % (defineFileSuffix, len(inFiles))
    for zippedFile in inFiles:
      (inFileNoExt, inFileExt) = self._getFilenamePadded(zippedFile['outFile'])
      defineZippedFileSuffix = defineFileSuffix + '_' + inFileNoExt.strip() + '_' + inFileExt.strip()

      outputDefineFilenameOffset = '#define FILENAME_' + defineZippedFileSuffix + '_OFFSET %s' % offset
      outputDefineFilenameUncomp = '#define FILENAME_' + defineZippedFileSuffix + '_SIZE_UNCOMPRESSED ' + zippedFile['sizeUncomp']
      outputDefineFilenameComp = '#define FILENAME_' + defineZippedFileSuffix + '_SIZE_COMPRESSED ' + zippedFile['sizeComp']

      outFile.write(outputDefineFilenameOffset + '\n')
      outFile.write(outputDefineFilenameUncomp + '\n')
      outFile.write(outputDefineFilenameComp + '\n')
      outFile.write('\n')

      indexTableOffsets += '0x%04x, ' % offset
      indexTableSizeComp += '0x%04x, ' % int(zippedFile['sizeComp'])
      indexTableSizeUncomp += '0x%04x, ' % int(zippedFile['sizeUncomp'])

      offset += int(zippedFile['sizeComp'])

    indexTableOffsets += '\n};'
    indexTableSizeComp += '\n};'
    indexTableSizeUncomp += '\n};'

    outFile.write(indexTableOffsets + '\n')
    outFile.write(indexTableSizeComp + '\n')
    outFile.write(indexTableSizeUncomp + '\n')

  def _generateData(self):

    FILE_DATA_DEFINES_OUT = os.path.join(self.DirSources[0], 'file_data_defines.inc')
    FILE_DATA_DEFINES = open(FILE_DATA_DEFINES_OUT, 'w')
    FILE_DATA_DEFINES_HEADER = "// Filename defines and its compressed/uncompressed size. -1 is added at the end for referencing the last byte when used as an offset.\n// Also here is defined the maximum buffer for the zipped files.\n#define ZIP_BUFFER_SIZE 1280"
    FILE_DATA_DEFINES.write(FILE_DATA_DEFINES_HEADER + '\n')

    try:
      print ('\n= PROCESSING RESOURCE FILES =')
      print (' - Files:')

      for resDir in self.DirResources:
        filelist = os.listdir(resDir)
        for file in filelist:
          print ('\t%s' % file)
          if file.lower().startswith('.git'):
            continue

          # If is a file compress it if it has the .exo extension and add the required defines
          # for using it in the code
          if os.path.isfile(os.path.join(resDir, file)):
            inFile = os.path.join(resDir, file)

            if file.endswith(".exo"):
              outFile = os.path.join(self.DirOutput, os.path.basename(file))
              (FILE_COMPRESSED_LENGTH, FILE_UNCOMPRESSED_LENGTH) = self._compressData(inFile, outFile)
              self._writeDefines(file, FILE_DATA_DEFINES, FILE_COMPRESSED_LENGTH, FILE_UNCOMPRESSED_LENGTH)
            else:
              shutil.copyfile(inFile, os.path.join(self.DirOutput, file))

          # if it is a directory compress all of the files inside, generates defines with offset,
          # compressed size, etc... and concatenate all of them in one unique archive. Then
          # compress it as a normal file.
          elif os.path.isdir(os.path.join(resDir, file)):
            print ('Directory found: %s' % os.path.join(resDir, file))
            print ('Compressing files in %s' % os.path.join(resDir, file))

            zipFiles = os.listdir(os.path.join(resDir, file))
            tempFilesPath = list()
            for fileToZip in zipFiles:
              if os.path.isfile(os.path.join(resDir, file, fileToZip)):
                inFile = os.path.join(resDir, file, fileToZip)
                outFile = os.path.join(self.DirTemp, os.path.basename(fileToZip))

                print (' - Input: %s\n - Output: %s' % (inFile, outFile))
                (FILE_COMPRESSED_LENGTH, FILE_UNCOMPRESSED_LENGTH) = self._compressData(inFile, outFile)

                tempFilesPath.append({'outFile': outFile, 'sizeComp': FILE_COMPRESSED_LENGTH, 'sizeUncomp': FILE_UNCOMPRESSED_LENGTH})

            # Concatenate files
            zipFile = os.path.join(self.DirTemp, file+'.zip')
            fout = open(zipFile, 'wb')
            for n in tempFilesPath:
              fin = open(n['outFile'], 'rb')
              shutil.copyfileobj(fin, fout, 65536)
              fin.close()
            fout.close()

            (FILE_COMPRESSED_LENGTH, FILE_UNCOMPRESSED_LENGTH) = self._compressData(zipFile, os.path.join(self.DirOutput, os.path.basename(zipFile)))
            self._writeDefines(os.path.basename(zipFile), FILE_DATA_DEFINES, FILE_COMPRESSED_LENGTH, FILE_UNCOMPRESSED_LENGTH)
            self._writeDefinesZip(os.path.basename(zipFile), FILE_DATA_DEFINES, tempFilesPath)

            #shutil.copy(zipFile, os.path.join(self.DirOutput, os.path.basename(zipFile)))


      print ('')
    finally:
      FILE_DATA_DEFINES.close()

    try:
      import processData
    except ImportError as e:
      print ('No additional data steps defined...')
      global processData
      processData = None

    try:
      #from processData import generateImageFolderStructure
      if processData:
        processData.generateImageFolderStructure(int(self._map_current['e__DATA']))
    except ImportError as e:
      print ('There are no additional data steps...')
      sys.exit(0)

  def _generateDsk(self):

    assert self._loadAddress, \
      "[ERROR] _loadAddress should have been already set here.\n" \
      "Please review the build system since it must be automatically determined after assembling " \
      "the crt0 bootstrap."

    print ('\n= GENERATING DSK =')
    print (' - Files:')

    filelist = os.listdir(self.DirOutput)
    if os.name == 'nt':
      command = SYSTEM_BUILD_DIR + '\\cpcdiskxp\\CPCDiskXP.exe -File %s -AddAmsdosHeader %s %s %s'

      bAddToDsk = '-AddToNewDsk '
      for file in filelist:
        commandDsk = command % (os.path.join(self.DirOutput, file), self._loadAddress, bAddToDsk, self._dskFilename)
        #print 'GenerateDSKCommand: ' + commandDsk
        print ('\t%s' % file)

        subprocess.Popen(commandDsk, shell=True, stdout=self._logger, stderr=self._logger, encoding='utf-8').wait()
        if bAddToDsk == '-AddToNewDsk ':
          bAddToDsk = '-AddToExistingDsk '
    else:
      command = SYSTEM_BUILD_DIR + os.path.sep + 'iDSK %s -n' % self._dskFilename
      subprocess.Popen(command, shell=True, stdout=self._logger, stderr=self._logger, encoding='utf-8').wait()

      command = SYSTEM_BUILD_DIR + os.path.sep + 'iDSK %s -i %s -e %s -c %s'
      for file in filelist:
        commandDsk = command % (self._dskFilename, os.path.join(self.DirOutput, file), self._loadAddress, self._loadAddress)
        subprocess.Popen(commandDsk, shell=True, stdout=self._logger, stderr=self._logger, encoding='utf-8').wait()

      print ('\n\n\n\tContent of .dsk')
      command = SYSTEM_BUILD_DIR + os.path.sep + 'iDSK %s -l' % self._dskFilename
      subprocess.Popen(command, shell=True, stdout=self._logger, stderr=self._logger, encoding='utf-8').wait()

    if not self.existFile(self._dskFilename):
      raise Exception('\nOutput file was not generated. Generating DSK error!!!')

  def _parseMapFileFillArea(self, fp, TAG_AREA, target):
    """
    Fills the supplied dictionary with the entries of the map target area.
    """
    fp.seek(0)
    area_found = False
    last_entry = []
    for line in fp:
      if line.startswith(TAG_AREA):
        area_found = True

      if area_found:
        split_line = line.split()
        try:
          if len(split_line) > 1:
            pos_in_mem = int(split_line[0], 16)
            func_name = split_line[1]
            current_entry = [func_name, 0]
            target[pos_in_mem] = current_entry
            if last_entry:
              target[last_entry[0]][1] = pos_in_mem - last_entry[0]
            last_entry = [pos_in_mem, current_entry[0], current_entry[1]]
        except (ValueError, IndexError):
          if line.startswith('_'):  # We have found another area?
            if not line.startswith(TAG_AREA):
              break
        finally:
          pass

  def _parseMapFile(self, map_file_path):
    """
    Parses the given map file looking for the interesting values.
    """
    match_keys = {
      'l__BOOT0': '',
      'l__CODE': '',
      'l__DATA': '',
      '__bootstrap': '',
      's__CODE': '',
      's__DATA': '',
      'e__CODE': '',
      'e__DATA': ''
    }

    print ('Parsing map file => [%s]' % map_file_path)
    with open(map_file_path, 'r') as fp:
      key_count = 0
      # Extract the initial info.
      for line in fp:
        for each_key in match_keys:
          if each_key in line:
            match_keys[each_key] = str(int(line.split()[0], 16))
            key_count += 1
        if key_count == len(match_keys):
          break

      # Locate the _CODE area and fills the function start and length
      KEY_SECTION = 'INFO_FUNCTIONS'
      match_keys[KEY_SECTION] = {}
      self._parseMapFileFillArea(fp, '_CODE', match_keys[KEY_SECTION])

      # We have to manually calculate the length of the last element since the function
      # _parseMapFileFillArea is not able to determine it.
      if len(match_keys[KEY_SECTION].items()) == 0:
        new_entry = ["NO CODE IN CODE AREA", 0]
        match_keys[KEY_SECTION][int(match_keys['s__CODE'])] = new_entry

      _sorted_dict = sorted(match_keys[KEY_SECTION].items())
      assert sorted(match_keys[KEY_SECTION].items())[-1][-1][-1] == 0, \
        "[ERROR] Last element must be set to 0 since its length can't be determined.\n" \
        "Element => [%s]" % _sorted_dict[-1][-1]
      match_keys['e__CODE'] = str(int(match_keys['__bootstrap']) + int(match_keys['l__BOOT0']) + int(match_keys['l__CODE']))
      _sorted_dict[-1][-1][-1] = int(match_keys['e__CODE']) - _sorted_dict[-1][0]

      # Locate the _DATA area and fills the data start and length
      KEY_SECTION = 'INFO_DATA'
      match_keys[KEY_SECTION] = {}
      self._parseMapFileFillArea(fp, '_DATA', match_keys[KEY_SECTION])

      # We have to manually calculate the length of the last element since the function
      # _parseMapFileFillArea is not able to determine it.
      if len(match_keys[KEY_SECTION].items()) == 0:
        new_entry = ["NO DATA IN DATA AREA", 0]
        match_keys[KEY_SECTION][int(match_keys['s__DATA'])] = new_entry

      _sorted_dict = sorted(match_keys[KEY_SECTION].items())
      assert sorted(match_keys[KEY_SECTION].items())[-1][-1][-1] == 0, \
        "[ERROR] Last element must be set to 0 since its length can't be determined." \
        "Element => [%s]" % _sorted_dict[-1][-1]
      match_keys['e__DATA'] = str(int(match_keys['s__DATA']) + int(match_keys['l__DATA']))
      _sorted_dict[-1][-1][-1] = int(match_keys['e__DATA']) - _sorted_dict[-1][0]

    return match_keys

  def _fillLinkMaps(self):
    """
      We fill the link maps with useful information we will use for further checks and statistics
      across different builds.
    """
    self._map_current = self._parseMapFile(os.path.join(self.DirObject, self.Name + '.map'))

    if self._lastBuild != self._nextBuild:
      from buildSettings import IntermediatesDir, ObjectDir

      # Try to find the last build. It is possible that previous builds have failed without
      # generating good intermediate files or that the directories have been deleted in the
      # meanwhile so we start searching backwards until we find one or we just skip the
      # statistic comparison later.
      past_map_file_path = self.getRealPath(os.path.join(IntermediatesDir, self._lastBuild, ObjectDir, self.Name + '.map'))
      while not self.existFile(past_map_file_path) and int(self._lastBuild) > 0:
        self._lastBuild = str(int(self._lastBuild) - 1)
        past_map_file_path = self.getRealPath(os.path.join(IntermediatesDir, self._lastBuild, ObjectDir, self.Name + '.map'))

      if int(self._lastBuild) > 0:
        self._map_past = self._parseMapFile(past_map_file_path)

  def _generateReport(self):
    """
    Generates a report showing the differences regarding the previous build if it is available.
    """
    assert self._map_current, "[ERROR] Map with the current link information must exist!"

    # TODO - Total code length is not correctly calculated. Size in disk differs from the one here outputted... it could be due to an incomplete .map file
    # Write the report.
    REPORT_FILE_PATH = os.path.join(self.DirObject, 'report.txt')
    with open(REPORT_FILE_PATH, 'w') as fp:
      fp.write('=======================================\n')
      fp.write('DETAILED COMPILATION REPORT:\n')
      fp.write('=======================================\n')

      # Write the function information and statistic if proceed.
      # TODO - Unoptimal when comparing but we have the function sorted by their memory start
      # addresses.
      fp.write('\nFUNCTIONS REPORT:\n')
      fp.write('\n[{:^30}]    [{:^5}] [{:^6}]\n'.format('NAME', 'LEN', 'DIFF'))
      fp.write('----------------------------------------------------\n')
      output = "[%30s] => [%5i] [%6s]\n"
      KEY_SECTION = 'INFO_FUNCTIONS'
      for each_func_addr, each_func_vals in sorted(self._map_current[KEY_SECTION].items()):
        _name = each_func_vals[0]
        _len = each_func_vals[1]
        _len_diff = 0

        if self._map_past:
          _len_past = 0
          for _, each_past_vals in self._map_past[KEY_SECTION].items():
            _name_past = each_past_vals[0]
            if _name_past == _name:
              _len_past = each_past_vals[1]
              _len_diff = _len - _len_past
              break

        fp.write(output % (_name, _len, ("%+i" % _len_diff) if _len_diff != 0 else ''))

      # Write the data information and statistic if proceed.
      # TODO - Unoptimal when comparing but we have the function sorted by their memory start
      # addresses.
      fp.write('\nDATA REPORT:\n')
      fp.write('\n[{:^30}]    [{:^5}] [{:^6}]\n'.format('VAR. NAME', 'LEN', 'DIFF'))
      fp.write('----------------------------------------------------\n')
      KEY_SECTION = 'INFO_DATA'
      for each_func_addr, each_func_vals in sorted(self._map_current[KEY_SECTION].items()):
        _name = each_func_vals[0]
        _len = each_func_vals[1]
        _len_diff = 0

        if self._map_past:
          _len_past = 0
          for _, each_past_vals in self._map_past[KEY_SECTION].items():
            _name_past = each_past_vals[0]
            if _name_past == _name:
              _len_past = each_past_vals[1]
              _len_diff = _len - _len_past
              break

        fp.write(output % (_name, _len, ("%+i" % _len_diff) if _len_diff != 0 else ''))
      
      fp.write('\nGENERAL INFO:\n')
      fp.write('\n[{:^20}]    [{:^5}] [{:^6}]\n'.format('FIELD', 'POS', 'DIFF'))
      fp.write('----------------------------------------------------\n')

      # Write code start and length and statistic if proceed.
      output = "[%20s] => [%5i] [%6s]\n"
      addr_boot = int(self._map_current['__bootstrap'])
      addr_code = int(self._map_current['s__CODE'])

      len_boot = int(self._map_current['l__BOOT0'])
      len_code = int(self._map_current['l__CODE'])
      len_code_total = len_code + len_boot

      len_code_diff = 0
      if self._map_past:
        len_boot_past = int(self._map_past['l__BOOT0'])
        len_code_past = int(self._map_past['l__CODE'])
        len_code_past_total = len_code_past + len_boot_past
        len_code_diff = len_code_total - len_code_past_total

      fp.write(output % ('CODE START ADDRESS', addr_boot, ''))
      fp.write(output % ('CODE END ADDRESS', int(self._map_current['e__CODE']), ''))
      assert len_code + addr_code == addr_boot + len_boot + len_code, \
        "[ERROR] Linked code location is not immediately after the __bootstrap section.\n"\
        "Please review the build system since it must be automatically determined after " \
        "assembling the crt0 bootstrap."
      fp.write(output % \
      ('CODE LENGTH', len_code_total, ("%+i" % (len_code_diff)) if len_code_diff != 0 else ''))

      # Write data start and length and statistic if proceed.
      addr_data = int(self._map_current['s__DATA'])
      len_data = int(self._map_current['l__DATA'])

      len_data_diff = 0
      if self._map_past:
        len_data_past = int(self._map_past['l__DATA'])
        len_data_diff = len_data - len_data_past

      fp.write(output % ('DATA START ADDRESS', addr_data, ''))
      fp.write(output % ('DATA END ADDRESS', int(self._map_current['e__DATA']), ''))
      fp.write(output % ('DATA LENGTH', len_data, ("%+i" % len_data_diff) if len_data_diff != 0 else ''))

    with open(REPORT_FILE_PATH, 'r') as fp:
      print ("\n%s\n" % fp.read())

  def _generateStatistics(self):
    """
    Generates statistics with all the available builds.
    """
    # Builds a dict with all the builds_ids containing the path to the .map files required by the statistic module.
    import pygal

    print ('[INFO] Generating full statistics')
    build_info = {}
    dir_intermediates_base = os.path.dirname(self.DirIntermediates)
    filelist = os.listdir(dir_intermediates_base)
    for build_id in range(1, int(self._nextBuild)):
      full_filename = os.path.join(dir_intermediates_base, str(build_id))
      if os.path.isdir(full_filename):
        path_to_asm = os.path.join(full_filename, 'obj', 'main.asm')
        sdcc_ver = ''
        if os.path.isfile(path_to_asm):
          with open(path_to_asm, 'r') as fp:
            for each_line in fp.readlines():
              if 'Version' in each_line:
                sdcc_ver = each_line.split('Version ')[1]
                break

        path_to_map = os.path.join(full_filename, 'obj', '%s.map' % self.Name)
        if os.path.isfile(path_to_map):
          # We found the map. Now extract the information out of it.
          # key_to_add = '%s SDCC %s' % (build_id, sdcc_ver)
          key_to_add = build_id
          build_info[key_to_add] = self._parseMapFile(path_to_map)
          build_info[key_to_add]['sdcc_ver'] = sdcc_ver

    build_info = sorted(build_info.items())

    # chart = pygal.StackedBar(dynamic_print_values=True, width=1920, height = 1080)

    color_unusable = 'rgba(255, 0, 0, 1)'
    color_code = 'rgba(0, 0, 192, 1)'
    color_code_free = 'rgba(0, 0, 192, .5)'
    color_data = 'rgba(0, 192, 0, 1)'
    color_data_free = 'rgba(0, 192, 0, .5)'

    chart = pygal.StackedBar(
      dynamic_print_values=True,
      style=pygal.style.DefaultStyle(
        colors=(color_unusable, color_code, color_code_free, color_data, color_data_free)
      )
    )
    chart.title = 'Build history statistics'
    chart.x_labels = map(str, [id for id, _ in build_info])
    chart.x_label_rotation = 90
    chart.x_title = 'Build numbers'
    chart.y_title = 'Memory map'
    chart.range = [0, 32768]

    bar_mem_start_len = []
    bar_code_len = []
    bar_data_start_len = []
    bar_data_len = []
    bar_heap_len = []

    for each_build_id, each_build_stats in build_info:
      print ('Adding: [%s]' % each_build_id)
      info_label = 'SDCC %s' % each_build_stats['sdcc_ver']
      bar_mem_start_len.append(int(each_build_stats['s__CODE']))
      bar_code_len.append({ 'value': int(each_build_stats['l__CODE']), 'label': info_label })
      bar_data_start_len.append(int(each_build_stats['s__DATA']) - int(each_build_stats['e__CODE']))
      bar_data_len.append(int(each_build_stats['l__DATA']))
      bar_heap_len.append(32768 - int(each_build_stats['e__DATA']))

    chart.add({'title': 'AREA UNUSABLE', 'color': color_unusable }, bar_mem_start_len)
    chart.add({'title': 'AREA CODE', 'color': color_code, 'print_values_position': 'top' }, bar_code_len)
    chart.add({'title': 'AREA FREE CODE', 'color': color_code_free }, bar_data_start_len)
    chart.add({'title': 'AREA DATA', 'color': color_data }, bar_data_len)
    chart.add({'title': 'AREA HEAP', 'color': color_data_free }, bar_heap_len)

    # chart.render_in_browser(os.path.join(os.path.dirname(self.DirIntermediates), 'report.html'))
    chart.render_to_file(os.path.join(os.path.dirname(self.DirIntermediates), 'report.svg'))

  def _launchDsk(self):
    #command = SYSTEM_BUILD_DIR + '\\cpce\\cpce95.exe /a+ ' + self._dskFilename
    if os.name == 'nt':
      command = SYSTEM_BUILD_DIR + '\\winape\\winape.exe ' + self._dskFilename + ' /A ' + '/SYM:' + self._symFilename
    else:
      command = "\"/Applications/Retro Virtual Machine 2.app/Contents/MacOS/Retro Virtual Machine 2\" -b=cpc6128 -i " + self._dskFilename + " -c='run\"" + self.Name + "\\n'"

    print ('Running image:\n\tcmd: [%s]' % command)
    subprocess.Popen(command, shell=True, stdout=self._logger, stderr=self._logger, encoding='utf-8').wait()

def ensureList(value):
  if type(value) is not list:
    return [value]
  else:
    return value

##################################################
##################################################
##################################################
SYSTEM_BUILD_DIR = os.path.realpath(os.path.dirname(sys.argv[0]))

if len(sys.argv) < 2:
  showHowToUse()
  sys.exit(1)

def importsFromProject(relPath):
  path = os.path.abspath(relPath)
  if not path in sys.path:
    sys.path.insert(1, path)


def configurePaths():
  global PATH_SDCC, ASM_EXEC, COMP_EXEC, EXOMIZER_EXE, HEX2BIN_EXE
  EXEC_EXT = '.exe' if os.name == 'nt' else ''
  # PATH_SDCC = os.path.join("c:\\amstrad\\build\\sdcc", VERSION_SDCC, "bin")
  _basedir = os.path.abspath(os.path.dirname(__file__))
  PATH_SDCC = os.path.join(_basedir, "sdcc", sys.platform, VERSION_SDCC, "bin")
  ASM_EXEC = os.path.join(PATH_SDCC, "sdasz80" + EXEC_EXT)
  COMP_EXEC = os.path.join(PATH_SDCC, "sdcc" + EXEC_EXT)
  EXOMIZER_EXE = os.path.abspath(os.path.join(_basedir, '..', 'ThirdParty', 'Exomizer_3.1.1', sys.platform, 'exomizer' + EXEC_EXT))
  HEX2BIN_EXE = os.path.join(_basedir, 'hex2bin', sys.platform, 'bin', 'hex2bin' + ('.exe' if os.name == 'nt' else ''))
  os.environ['CPC_BUILD_EXE_EXOMIZER'] = EXOMIZER_EXE
  os.environ['CPC_BUILD_PATH_SDCC'] = PATH_SDCC
  os.environ['CPC_BUILD_EXE_ASM'] = ASM_EXEC
  os.environ['CPC_BUILD_EXEC_COMP'] = COMP_EXEC
  os.environ['CPC_BUILD_EXE_HEX2BIN'] = HEX2BIN_EXE

def validatePaths():
  assert (os.path.exists(PATH_SDCC) and not os.path.isfile(PATH_SDCC)), '\nSDCC version does not exist [%s]' % PATH_SDCC
  assert (os.path.isfile(ASM_EXEC)), '\nSDCC assembler exec does not exist'
  assert (os.path.isfile(COMP_EXEC)), '\nSDCC compiler exec does not exist'
  assert (os.path.isfile(EXOMIZER_EXE)), '\nExomizer not found under dir => [%s]' % EXOMIZER_EXE
  assert (os.path.isfile(HEX2BIN_EXE)), '\nhex2bin executable not found under => [%s]' % HEX2BIN_EXE

importsFromProject('.')
#importsFromProject(os.path.join(SYSTEM_BUILD_DIR, 'Python27', 'Lib', 'site-packages'))
importsFromProject(os.path.join(SYSTEM_BUILD_DIR, 'Python3', 'Lib', 'site-packages'))

if (len(sys.argv) > 2):
  print ("len of sys.arv: %s" % len(sys.argv))
  if (sys.argv[2]):
    VERSION_SDCC = sys.argv[2]
    VERSION_SDCC_LIST = VERSION_SDCC.split('.')
    VERSION_SDCC_MAJOR = VERSION_SDCC_LIST[0]
    VERSION_SDCC_MINOR = VERSION_SDCC_LIST[1]
    VERSION_SDCC_RELEASE = VERSION_SDCC_LIST[2]

if (len(sys.argv) > 3):
  if (sys.argv[3]):
    MAX_ALLOCS_PER_NODE = sys.argv[3]

configurePaths()
validatePaths()
prj = Project('.')
prj.build()

print ('\n\nAssembling, compilation, binary and dsk generation successful!\n\tGenerated dsk in: %s' % (prj._dskFilename))
