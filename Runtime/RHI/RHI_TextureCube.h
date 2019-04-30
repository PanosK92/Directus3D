/*
Copyright(c) 2016-2019 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES ===========
#include "RHI_Texture.h"
//======================

namespace Spartan
{
	class SPARTAN_CLASS RHI_TextureCube : public RHI_Texture
	{
	public:
		// Creates a cubemap. 6 textures containing mip-levels have to be provided
		RHI_TextureCube(Context* context, unsigned int width, unsigned int height, unsigned int channels, RHI_Format format, const std::vector<std::vector<std::vector<std::byte>>>& data) : RHI_Texture(context)
		{
			m_width			= width;
			m_height		= height;
			m_channels		= channels;
			m_format		= format;
			m_has_mipmaps	= true;
			//m_data		= data; // Cubemaps are loaded dynamically but I should still fix this	
			Create(data);
		}

		~RHI_TextureCube();

	private:
		bool Create(const std::vector<std::vector<std::vector<std::byte>>>& data);
	};
}