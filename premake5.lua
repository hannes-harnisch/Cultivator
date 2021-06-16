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
	warnings			'Extra'
	files				{ 'Code/**.cc', 'Code/**.hh', 'Code/**.hlsli' }
	removefiles			{ 'Code/**/**.*.*' }
	files				{ 'Code/**.hlsl' }
	objdir				( '.bin_int/' .. output_dir .. '/%{prj.name}' )
	targetdir			( '.bin/'	  .. output_dir .. '/%{prj.name}' )
	debugdir			( '.bin/'	  .. output_dir .. '/%{prj.name}' )
	pchheader			'PCH.hh'
	pchsource			'Code/PCH.cc'

	filter 'files:**.*.hlsl'
		buildmessage	'Compiling shader %{file.relpath}'
		buildcommands	'C:/VulkanSDK/1.2.176.1/bin/dxc %{file.relpath} /Fo %{cfg.targetdir}/%{file.basename}.spv -spirv -O3 -T ^'
		buildoutputs	'%{cfg.targetdir}/%{file.basename}.spv'

	filter 'files:**.vert.hlsl'
		buildcommands	'vs_6_6'

	filter 'files:**.frag.hlsl'
		buildcommands	'ps_6_6'

	filter 'system:Windows'
		systemversion	'latest'
		files			{ 'Code/**/Windows.*.*', 'Code/**/Direct3D.*.*', 'Code/**/Vulkan.*.*', 'Code/**/VulkanWindows.*.*' }
		includedirs		'C:/VulkanSDK/1.2.176.1/Include'
		defines			{ 'CT_SYSTEM_WINDOWS', 'CT_SYSTEM=windows' }

	filter 'configurations:Debug'
		runtime			'Debug'
		symbols			'On'
		defines			'CT_DEBUG'

	filter 'configurations:Release'
		runtime			'Release'
		optimize		'Speed'

project 'Cultivator'
	location			'Code'
	includedirs			'Code'
	entrypoint			'mainCRTStartup'
	defines				'CT_APP_NAME="%{prj.name}"'

	filter 'configurations:Debug'
		kind			'ConsoleApp'

	filter 'configurations:Release'
		kind			'WindowedApp'