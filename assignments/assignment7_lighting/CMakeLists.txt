#Transformations example

file(
 GLOB_RECURSE FINALPROJECT_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE FINALPROJECT_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this assignment7_lighting's asset folder to bin when it is built
add_custom_target(copyAssetsFP ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${FINALPROJECT_INC} DESTINATION include/FinalProject)
add_executable(FinalProject ${FINALPROJECT_SRC} ${FINALPROJECT_INC} ${ASSIGNMENT7_ASSETS})
target_link_libraries(FinalProject PUBLIC core IMGUI)
target_include_directories(FinalProject PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when assignment7_lighting is built
add_dependencies(FinalProject copyAssetsFP)