if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS Core Support)
endif()
set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/bin)
set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/lib)

 
set(CMAKE_MODULE_PATH /usr/share/llvm-3.8/cmake)
MESSAGE( STATUS "CMAKE_MODULE_PATH: " ${LLVM_CMAKE_DIR} )
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
MESSAGE( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH} )
include(AddLLVM)
include(HandleLLVMOptions)

add_llvm_loadable_module( LLVMlicm
  source/Optimiser.cpp
  
  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
  
 
