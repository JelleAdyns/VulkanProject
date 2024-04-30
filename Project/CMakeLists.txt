# Source files

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/shader.frag"
    "${SHADER_SOURCE_DIR}/shader.vert"
    "${SHADER_SOURCE_DIR}/objshader.frag"
    "${SHADER_SOURCE_DIR}/objshader.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)


set(SOURCES
    "main.cpp"
    "vulkanbase/VulkanBase.h" 
    "vulkanbase/VulkanUtil.h"
    "vulkanbase/VulkanUtil.cpp"
    # Add other source files here
    "labwork/Week01.cpp"
    "labwork/Week02.cpp" 
    "labwork/Week03.cpp" 
    "labwork/Week04.cpp" 
    "labwork/Week05.cpp"
    "labwork/Week06.cpp"
    "GP2CommandPool.h"
    "GP2CommandPool.cpp"
    "GP2CommandBuffer.h"
    "GP2CommandBuffer.cpp"
    "Vertex.h"
    "GP2Buffer.h"
    "GP2Buffer.cpp"
    "OBJParser.h"
    "Camera.h"
    "Camera.cpp"
    "GP2DescriptorPool.h"
    "GP2GraphicsPipeline.h" 
    "GP2Shader.h" 
    "GP2Mesh.h"  
    "GP2UniformBufferObject.h" 
    "ContextStructs.h"
    "GP2Texture.h"
    "GP2Texture.cpp")

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES} )
add_dependencies(${PROJECT_NAME} Shaders)
# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)
target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})

add_custom_target(copy_resources ALL)
add_custom_command(
    TARGET copy_resources PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources
)
