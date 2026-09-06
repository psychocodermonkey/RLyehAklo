cmake_minimum_required(VERSION 3.21)

project(RLyehAklo VERSION 0.0.0 LANGUAGES C)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

option(RLYEH_AKLO_BUILD_LINUX_KERNEL_MODULE
       "Build the R'Lyeh Aklo Linux kernel module" OFF)
option(RLYEH_AKLO_STRIP_LINUX_KERNEL_MODULE
       "Strip debug information from the Linux kernel module" OFF)

set(RLYEH_AKLO_LINUX_KERNEL_BUILD_DIR "" CACHE PATH
    "Path to the prepared Linux kernel build tree")

add_library(RLyehAklo INTERFACE)
add_library(RLyeh::Aklo ALIAS RLyehAklo)
set_target_properties(RLyehAklo PROPERTIES EXPORT_NAME Aklo)

target_include_directories(RLyehAklo INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_sources(RLyehAklo INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/scuf/scuf.h>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/scuf/envision-v2.h>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/scuf/hid.h>
)

if(RLYEH_AKLO_BUILD_LINUX_KERNEL_MODULE)
  if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "The Linux kernel module can only be built on Linux")
  endif()

  if(NOT RLYEH_AKLO_LINUX_KERNEL_BUILD_DIR)
    execute_process(
      COMMAND uname -r
      RESULT_VARIABLE _kernel_release_result
      OUTPUT_VARIABLE _kernel_release
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT _kernel_release_result EQUAL 0 OR NOT _kernel_release)
      message(FATAL_ERROR "Could not determine the running Linux kernel release")
    endif()

    set(RLYEH_AKLO_LINUX_KERNEL_BUILD_DIR
        "/lib/modules/${_kernel_release}/build" CACHE PATH
        "Path to the prepared Linux kernel build tree" FORCE)
  endif()

  if(NOT EXISTS "${RLYEH_AKLO_LINUX_KERNEL_BUILD_DIR}/Makefile")
    message(FATAL_ERROR
      "No prepared Linux kernel build tree found at "
      "${RLYEH_AKLO_LINUX_KERNEL_BUILD_DIR}"
    )
  endif()

  find_program(RLYEH_AKLO_MAKE_EXECUTABLE NAMES make REQUIRED)

  set(_linux_module_source_dir "${CMAKE_CURRENT_SOURCE_DIR}/linux/hid-scuf")
  set(_linux_module_working_dir "${CMAKE_CURRENT_BINARY_DIR}/linux/hid-scuf")
  set(_linux_module_output_dir "${CMAKE_CURRENT_BINARY_DIR}/bin")
  set(_linux_module_built
      "${_linux_module_working_dir}/rlyeh-scuf.ko")
  set(_linux_module "${_linux_module_output_dir}/rlyeh-scuf.ko")

  set(_linux_module_strip_command)
  if(RLYEH_AKLO_STRIP_LINUX_KERNEL_MODULE)
    find_program(RLYEH_AKLO_STRIP_EXECUTABLE NAMES strip REQUIRED)
    set(_linux_module_strip_command
        COMMAND "${RLYEH_AKLO_STRIP_EXECUTABLE}" --strip-debug "${_linux_module}")
  endif()

  add_custom_command(
    OUTPUT "${_linux_module}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${_linux_module_working_dir}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${_linux_module_output_dir}"
    COMMAND "${RLYEH_AKLO_MAKE_EXECUTABLE}"
            -C "${RLYEH_AKLO_LINUX_KERNEL_BUILD_DIR}"
            "M=${_linux_module_source_dir}"
            "MO=${_linux_module_working_dir}"
            modules
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${_linux_module_built}" "${_linux_module}"
    ${_linux_module_strip_command}
    DEPENDS
      linux/hid-scuf/Kbuild
      linux/hid-scuf/scuf.c
      linux/hid-scuf/scuf-envision-v2.c
      include/scuf/scuf.h
      include/scuf/envision-v2.h
      include/scuf/hid.h
    COMMENT "Building rlyeh-scuf.ko"
    VERBATIM
    USES_TERMINAL
  )

  add_custom_target(rlyeh-scuf ALL DEPENDS "${_linux_module}")
  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
               "${_linux_module_working_dir}")
endif()

set(RLyehAklo_INSTALL_CMAKEDIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/RLyehAklo" CACHE STRING
    "Install location for the R'Lyeh Aklo CMake package")

configure_package_config_file(
  cmake/RLyehAkloConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/RLyehAkloConfig.cmake"
  INSTALL_DESTINATION "${RLyehAklo_INSTALL_CMAKEDIR}"
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/RLyehAkloConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMinorVersion
)

install(TARGETS RLyehAklo
  EXPORT RLyehAkloTargets
  COMPONENT RLyehAklo
)
install(DIRECTORY include/scuf
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  COMPONENT RLyehAklo
)
install(EXPORT RLyehAkloTargets
  FILE RLyehAkloTargets.cmake
  NAMESPACE RLyeh::
  DESTINATION "${RLyehAklo_INSTALL_CMAKEDIR}"
  COMPONENT RLyehAklo
)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/RLyehAkloConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/RLyehAkloConfigVersion.cmake"
  DESTINATION "${RLyehAklo_INSTALL_CMAKEDIR}"
  COMPONENT RLyehAklo
)
