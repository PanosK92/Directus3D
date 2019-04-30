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

//= INCLUDES ========================
#include <vector>
#include <string>
#include "../../Core/EngineDefs.h"
#include "../../RHI/RHI_Definition.h"
//===================================

struct FIBITMAP;

namespace Spartan
{
	class Context;

	class SPARTAN_CLASS ImageImporter
	{
	public:
		ImageImporter(Context* context);
		~ImageImporter();

		bool Load(const std::string& file_path, RHI_Texture* texture, bool generate_mipmaps = true);

	private:	
		bool GetBitsFromFibitmap(std::vector<std::byte>* data, FIBITMAP* bitmap, unsigned int width, unsigned int height, unsigned int channels);
		void GenerateMipmaps(FIBITMAP* bitmap, RHI_Texture* texture, unsigned int width, unsigned int height, unsigned int channels);

		unsigned int ComputeChannelCount(FIBITMAP* bitmap);
		unsigned int ComputeBitsPerChannel(FIBITMAP* bitmap) const;
		RHI_Format ComputeTextureFormat(unsigned int bpp, unsigned int channels) const;
		bool IsVisuallyGrayscale(FIBITMAP* bitmap) const;
		FIBITMAP* ApplyBitmapCorrections(FIBITMAP* bitmap);
		FIBITMAP* _FreeImage_ConvertTo32Bits(FIBITMAP* bitmap);
		FIBITMAP* _FreeImage_Rescale(FIBITMAP* bitmap, unsigned int width, unsigned int height);

		Context* m_context;
	};
}