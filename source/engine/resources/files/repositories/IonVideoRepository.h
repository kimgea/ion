/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources/files/repositories
File:	IonVideoRepository.h
-------------------------------------------
*/

#ifndef _ION_VIDEO_REPOSITORY_
#define _ION_VIDEO_REPOSITORY_

#include <string_view>
#include "IonFileRepository.h"

namespace ion::resources::files::repositories
{
	using namespace std::string_view_literals;

	namespace video_repository::detail
	{
		constexpr auto file_extensions = ".3gp/.amc/.avi/.dv/.f4v/.flc/.flv/.m2v/.m4v/.mkv/.mp4/.mpg/.mpeg/.mov/.rm/.rmvb/.tg2/.wmv"sv;
	} //video_repository::detail


	struct VideoRepository : FileRepository
	{
		//Default construct a video repository
		//Naming convention is set to file path
		VideoRepository() noexcept;

		//Construct a video repository with the given naming convention
		VideoRepository(file_repository::NamingConvention naming_convention) noexcept;
	};
} //ion::resources::files::repositories

#endif