# Set empty name of the IDL compiler
set(IDL_COMPILER "")

# Find the IDL compiler on the system
find_program(IDL_COMPILER_EXECUTABLE
  NAMES pidl
  PATHS /usr/bin /usr/local/bin
  DOC "The IDL compiler executable"
)

# Check if the IDL compiler was found
if(IDL_COMPILER_EXECUTABLE)
  message(STATUS "IDL compiler found: ${IDL_COMPILER_EXECUTABLE}")
else()
  message(FATAL_ERROR "IDL compiler not found, please set IDL_COMPILER_PATH environment variable to the directory containing the IDL compiler")
endif()

# Set the IDL compiler as a CMake cache variable
set(IDL_COMPILER ${IDL_COMPILER_EXECUTABLE} CACHE FILEPATH "The IDL compiler executable")
