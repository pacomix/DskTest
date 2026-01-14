@echo off
setlocal 

goto MAIN

::-----------------------------------------------
:: "%~f2" get abs path of %~2. 
::"%~fs2" get abs path with short names of %~2.
:assemble
  setlocal
  
  @echo Assembling:
  @echo   Input:  %2
  @echo   Output: %1
  
  set _objFile=%~f1
  set _srcFile=%~f2
  
  IF EXIST %_objFile% (del %_objFile%)
  sdasz80.exe -o %_objFile% %_srcFile%
  
  IF NOT EXIST %1 (
    echo There were an error while assembling: %2
    goto :error
  )
  
  endlocal
  goto :eof
::-----------------------------------------------

::-----------------------------------------------
:: "%~f2" get abs path of %~2. 
::"%~fs2" get abs path with short names of %~2.
:link
  setlocal
  
  @echo Assembling:
  @echo   Input:  %2
  @echo   Output: %1
  
  set _outFile=%~f1
  set _inFile0=%~f2
  set _inFile1=%~f3
  set _inFile2=%~f4
  set _inFile3=%~f5
  set _inFile4=%~f6
  set _inFile5=%~f7
  set _inFile6=%~f8
  
  IF EXIST %_outFile% (del %_outFile%)
  sdar.exe rc -o %_outFile% %_inFile0% %_inFile1% %_inFile2% %_inFile3% %_inFile4% %_inFile5% %_inFile6%
  
  IF NOT EXIST %1 (
    echo There were an error while linking: %1
    goto :error
  )
  
  endlocal
  goto :eof
::-----------------------------------------------


:MAIN

call :assemble .\obj\cpcrslib_tiny.rel   .\src\cpcrslib_tiny.s
rem call :assemble .\obj\cpcrslib.rel   .\src\cpcrslib.s
rem call :assemble .\obj\GphStr.rel     .\src\GphStr.s
rem call :assemble .\obj\Sprites.rel    .\src\Sprites.s
rem call :assemble .\obj\Keyboard.rel   .\src\Keyboard.s
rem call :assemble .\obj\GphStrStd.rel  .\src\GphStrStd.s
rem call :assemble .\obj\TileMap.rel    .\src\TileMap.s

call :link .\lib\cpcrslibmodtiny.lib .\obj\cpcrslib_tiny.rel

@echo Done!
goto :end

:error
@echo There were errors during the build!

:end

endlocal