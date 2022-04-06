/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonTextureScriptInterface.h
-------------------------------------------
*/

#ifndef ION_TEXTURE_SCRIPT_INTERFACE_H
#define ION_TEXTURE_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "graphics/textures/IonTexture.h"
#include "graphics/textures/IonTextureManager.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "types/IonTypes.h"

namespace ion::script::interfaces
{
	namespace texture_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_texture_class();
		ScriptValidator get_texture_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::textures::Texture> create_texture(const script_tree::ObjectNode &object,
			graphics::textures::TextureManager &texture_manager);

		void create_textures(const ScriptTree &tree,
			graphics::textures::TextureManager &texture_manager);
	} //texture_script_interface::detail


	class TextureScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			TextureScriptInterface() = default;


			/*
				Textures
				Creating from script
			*/

			//Create textures from a script (or object file) with the given asset name
			void CreateTextures(std::string_view asset_name,
				graphics::textures::TextureManager &texture_manager);
	};
} //ion::script::interfaces

#endif