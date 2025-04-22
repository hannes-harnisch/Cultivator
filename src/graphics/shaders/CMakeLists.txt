file(GLOB_RECURSE vert_sources CONFIGURE_DEPENDS *.vert.hlsl)
file(GLOB_RECURSE frag_sources CONFIGURE_DEPENDS *.frag.hlsl)
set(shader_sources ${vert_sources} ${frag_sources})

set_source_files_properties(${vert_sources} PROPERTIES shader_type "vs")
set_source_files_properties(${frag_sources} PROPERTIES shader_type "ps")
set_source_files_properties(${shader_sources} PROPERTIES shader_model "6_6")

get_filename_component(vk_lib_dir "${Vulkan_LIBRARY}" DIRECTORY)
get_filename_component(vk_sdk_dir "${vk_lib_dir}" DIRECTORY)

foreach (src_file ${shader_sources})
	get_filename_component(src_name ${src_file} NAME_WLE)
	get_source_file_property(shader_type ${src_file} shader_type)
	get_source_file_property(shader_model ${src_file} shader_model)
	set(output_file ${CMAKE_BINARY_DIR}/${src_name}.spv)
	list(APPEND shader_outputs ${output_file})
	add_custom_command(
		OUTPUT ${output_file}
		COMMAND ${vk_sdk_dir}/bin/dxc ${src_file} /Fo ${output_file} -spirv -O3 -T ${shader_type}_${shader_model} 
		MAIN_DEPENDENCY ${src_file}
		COMMENT "Compiling HLSL shader: ${src_file}"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		VERBATIM
	)
endforeach ()

add_custom_target(CultivatorShaders ALL DEPENDS ${shader_outputs})
