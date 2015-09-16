project(gl-template)

cmake_minimum_required(VERSION 2.8.12)

if (UNIX)
    set (compiler_flags "-std=c++11")  
endif()

if (MSVC)
    set (compiler_flags "/WX /EHsc")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()  

#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(cprintf)

#--------------------------------------------------------------------
#	dir variables
#--------------------------------------------------------------------
set(incl_dir ${CMAKE_CURRENT_SOURCE_DIR}/incl)
set(src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)

#--------------------------------------------------------------------
#	source files
#--------------------------------------------------------------------
file(GLOB_RECURSE PROJECT_INCL_FILES "${incl_dir}/*.h")
file(GLOB PROJECT_SRC_FILES "${src_dir}/*.cpp" )
file(GLOB_RECURSE ImGui_PROJECT_INCL_FILES "${incl_dir}/../imgui/*.h")
file(GLOB ImGui_PROJECT_SRC_FILES "${src_dir}/../imgui/*.cpp" )

set (glad_file ${CMAKE_CURRENT_SOURCE_DIR}/glfw/deps/glad.c)

add_executable(	${CMAKE_PROJECT_NAME} 
				${PROJECT_INCL_FILES}
        ${ImGui_PROJECT_INCL_FILES} 
				${PROJECT_SRC_FILES} 
        ${ImGui_PROJECT_SRC_FILES}
				${glad_file})

#--------------------------------------------------------------------
#	the GLFW_LIBRARIES cache variable contains all link-time 
#	dependencies of GLFW as it is currently configured.
#--------------------------------------------------------------------
target_link_libraries(	${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES} cprintf++)

set (glfw_dir ${CMAKE_CURRENT_SOURCE_DIR}/glfw)
set (glm_dir ${CMAKE_CURRENT_SOURCE_DIR}/glm)
set (imgui_dir ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
						          INCLUDE_DIRECTORIES 
                      "${incl_dir};${cprintf_incl_dir};${glfw_dir}/include;${glfw_dir}/deps;${glfw_dir}/deps/GL;${glm_dir};${imgui_dir}"
                      COMPILE_FLAGS ${compiler_flags})
