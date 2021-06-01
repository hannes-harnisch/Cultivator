output_dir				= '%{cfg.buildcfg}_%{cfg.architecture}_%{cfg.system}'

workspace 'Cultivator'
	architecture		'x64'
	configurations		{ 'Debug', 'Release' }
	flags				{ 'MultiProcessorCompile' }
	startproject		'Cultivator'
	language			'C++'
	cppdialect			'C++latest'
	conformancemode		'On'
	staticruntime		'On'
	floatingpoint		'Fast'
	rtti				'Off'
	exceptionhandling	'Off'
	warnings			'Extra'
	buildoptions		{ '/Zc:rvalueCast' }
	files				{ 'Code/**.cc', 'Code/**.hh' }
	removefiles			{ 'Code/**/**.*.*' }
	files				{ 'Code/**.hlsl' }
	objdir				( '.bin_int/' .. output_dir .. '/%{prj.name}' )
	targetdir			( '.bin/'	  .. output_dir .. '/%{prj.name}' )
	debugdir			( '.bin/'	  .. output_dir .. '/%{prj.name}' )
	pchheader			'PCH.hh'
	pchsource			'Code/PCH.cc'

	filter 'files:**.hlsl'
		buildmessage	'Compiling shader %{file.relpath}'
		buildcommands	'C:/VulkanSDK/1.2.176.1/bin/dxc -DENABLE_SPIRV_CODEGEN=ON -spirv "%{cfg.targetdir}/%{file.basename}.spv" "%{file.relpath}" ^'
		buildoutputs	'%{cfg.targetdir}/%{file.basename}.spv'

	filter 'files:**.vert.hlsl'	
		buildcommands	'-T vs_6_0'

	filter 'files:**.frag.hlsl'	
		buildcommands	'-T ps_6_0'

	filter 'system:Windows'
		systemversion	'latest'
		files			{ 'Code/**/Windows.*.*', 'Code/**/Direct3D.*.*', 'Code/**/Vulkan.*.*', 'Code/**/VulkanWindows.*.*' }
		includedirs		'C:/VulkanSDK/1.2.176.1/Include'
		libdirs			'C:/VulkanSDK/1.2.176.1/Lib'
		links			'Vulkan-1.lib'
		defines			'CT_SYSTEM_WINDOWS'

	filter 'configurations:Debug'
		runtime			'Debug'
		symbols			'On'
		defines			'CT_DEBUG'

	filter 'configurations:Release'
		runtime			'Release'
		optimize		'Speed'
		buildoptions	'/Ob3'
		flags			{ 'LinkTimeOptimization' }

project 'Cultivator'
	location			'Code'
	includedirs			'Code'
	entrypoint			'mainCRTStartup'
	defines				'CT_APP_NAME="%{prj.name}"'

	filter 'configurations:Debug'
		kind			'ConsoleApp'

	filter 'configurations:Release'
		kind			'WindowedApp'