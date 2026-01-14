#  PYTHON_USE_STATIC_LIB    - Set to ON to force the use of the static
#                             library.  Default is OFF.


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was PythonConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

####################################################################################

set(BUILD_LIBPYTHON_SHARED ON)
if(BUILD_LIBPYTHON_SHARED)
  set(PYTHON_BUILD_SHARED 1)
  set(PYTHON_BUILD_STATIC 0)
else()
  set(PYTHON_BUILD_SHARED 0)
  set(PYTHON_BUILD_STATIC 1)
endif()

set_and_check(PYTHON_CONFIG_DIR "${PACKAGE_PREFIX_DIR}/share/python3.13")
set_and_check(PYTHON_INCLUDE_DIR "${PACKAGE_PREFIX_DIR}/include/python3.13")
set_and_check(PYTHON_TARGETS "${PACKAGE_PREFIX_DIR}/share/python3.13/PythonTargets.cmake")



if(NOT PYTHON_TARGETS_IMPORTED)
  set(PYTHON_TARGETS_IMPORTED 1)
  include(${PYTHON_TARGETS})
endif()

if(TARGET libpython-shared)
  set(PYTHON_LIBRARY_SHARED libpython-shared)
endif()
if(TARGET libpython-static)
  set(PYTHON_LIBRARY_STATIC libpython-static)
endif()

if(NOT Python_USE_STATIC_LIB)
  if(PYTHON_LIBRARY_SHARED)
    set(PYTHON_LIBRARIES ${PYTHON_LIBRARY_SHARED})
  elseif(PYTHON_LIBRARY_STATIC)
    set(PYTHON_LIBRARIES ${PYTHON_LIBRARY_STATIC})
  endif()
else()
  if(PYTHON_LIBRARY_STATIC)
    set(PYTHON_LIBRARIES ${PYTHON_LIBRARY_STATIC})
  else()
    set(PYTHON_LIBRARIES ${PYTHON_LIBRARY_SHARED})
  endif()
endif()

if(NOT Python_FIND_QUIETLY)
  message(STATUS "PYTHON_LIBRARIES set to ${PYTHON_LIBRARIES}")
endif()

set(PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIR})
