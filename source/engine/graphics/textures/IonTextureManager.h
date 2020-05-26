/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonTextureManager.h
-------------------------------------------
*/

#ifndef ION_TEXTURE_MANAGER_H
#define ION_TEXTURE_MANAGER_H

#include "IonTexture.h"
#include "resources/files/IonFileResourceManager.h"
#include "resources/files/repositories/IonImageRepository.h"

namespace ion::graphics::textures
{
	namespace texture_manager::detail
	{

	} //texture_manager::detail


	class TextureManger :
		public resources::files::FileResourceManager<Texture, TextureManger, resources::files::repositories::ImageRepository>
	{
		protected:

			/*
				Events
			*/

			bool LoadResource(Texture &texture) noexcept override
			{
				texture;
				return true;
			}

			bool UnloadResource(Texture &texture) noexcept override
			{
				texture;
				return true;
			}

		public:

			//Default constructor
			TextureManger() = default;

			//Deleted copy constructor
			TextureManger(const TextureManger&) = delete;

			//Default move constructor
			TextureManger(TextureManger&&) = default;


			/*
				Operators
			*/

			//Deleted copy assignment
			TextureManger& operator=(const TextureManger&) = delete;

			//Move assignment
			TextureManger& operator=(TextureManger&&) = default;
	};
} //ion::graphics::textures

#endif