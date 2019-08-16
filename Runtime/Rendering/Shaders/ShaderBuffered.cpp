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

//= INCLUDES ============================
#include "ShaderBuffered.h"
#include "../../RHI/RHI_ConstantBuffer.h"
#include "../../Logging/Log.h"
//=======================================

//= NAMESPACES =====
using namespace std;
//==================

namespace Spartan
{
	ShaderBuffered::ShaderBuffered(const shared_ptr<RHI_Device>& rhi_device) : RHI_Shader(rhi_device)
	{

	}

	bool ShaderBuffered::UpdateBuffer(void* data, uint32_t index) const
	{
		if (!data)
		{
			LOG_ERROR_INVALID_PARAMETER();
			return false;
		}

		if (!m_buffers[index])
		{
			LOG_WARNING("Uninitialized buffer.");
			return false;
		}

		// Get a pointer of the buffer
		auto result = false;
		if (const auto buffer = m_buffers[index]->Map()) // Get buffer pointer
		{
			memcpy(buffer, data, m_buffers[index]->GetSize());	// Copy data
			result = m_buffers[index]->Unmap();					// Unmap buffer
		}

		if (!result)
		{
			LOG_ERROR("Failed to map buffer");
		}
		return result;
	}
}
