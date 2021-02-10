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
	buildoptions		{ '/Zc:rvalueCast' }
	files				{ 'Code/**.cc', 'Code/**.hh', 'Code/**.hlsl' }
	removefiles			{ 'Code/**.*.cc', 'Code/**.*.hh' }
	objdir				( '.bin_int/' .. output_dir .. '/%{prj.name}' )
	targetdir			( '.bin/'	 .. output_dir .. '/%{prj.name}' )
	debugdir			( '.bin/'	 .. output_dir .. '/%{prj.name}' )

	filter 'system:Windows'
		systemversion	'latest'
		files			{ 'Code/**/Windows.*.*' }

	filter 'configurations:Debug'
		runtime			'Debug'
		symbols			'On'
		defines			'CT_DEBUG'

	filter 'configurations:Release'
		runtime			'Release'
		optimize		'Speed'
		buildoptions	'/Ob3'
		flags			{ 'LinkTimeOptimization' }

	filter 'files:**.hlsl'
		flags			'ExcludeFromBuild'
		shadermodel		'6.4'

	filter 'files:**.frag.hlsl'
		removeflags		'ExcludeFromBuild'
		shadertype		'Pixel'

project 'Cultivator'
	location			'Code'
	includedirs			'Code'
	kind				'WindowedApp'
	entrypoint			'mainCRTStartup'
	defines				'CT_APP_NAME="%{prj.name}"'