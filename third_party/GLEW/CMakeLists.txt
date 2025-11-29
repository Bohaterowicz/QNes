# GLEW library setup
set(GLEW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(GLEW_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Check if the library exists
if(EXISTS "${GLEW_LIBRARY_DIR}/glew32s.lib")
  # Create imported target for GLEW with absolute path
  add_library(glew32s STATIC IMPORTED GLOBAL)
  set_target_properties(
    glew32s
    PROPERTIES IMPORTED_LOCATION "${GLEW_LIBRARY_DIR}/glew32s.lib"
               INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}"
               IMPORTED_LINK_INTERFACE_LIBRARIES "opengl32;glu32")
else()
  message(
    FATAL_ERROR "GLEW library not found at: ${GLEW_LIBRARY_DIR}/glew32s.lib")
endif()

# Make GLEW available to other targets (set in cache for global availability)
set(GLEW_FOUND
    TRUE
    CACHE BOOL "GLEW library found" FORCE)
set(GLEW_INCLUDE_DIRS
    "${GLEW_INCLUDE_DIR}"
    CACHE PATH "GLEW include directories" FORCE)
set(GLEW_LIBRARIES
    glew32s
    CACHE STRING "GLEW libraries" FORCE)
