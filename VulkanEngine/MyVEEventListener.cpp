#include "VEInclude.h"
#include "MyVEEventListener.h"
#include <iostream>

namespace ve
{
	void MyVEEventListener::onFrameEnded(veEvent event, bool t)
	{
		std::cout<<event.dt<<std::endl;

		MyVEEventListener::global_time = MyVEEventListener::global_time + event.dt;
		if (MyVEEventListener::global_time >= 0.5) {
			m_makeScreenshot = true;
			MyVEEventListener::global_time = 0;
		}

		if (m_makeScreenshot && m_numScreenshot<20)
		{
			VkExtent2D extent = getWindowPointer()->getExtent();
			uint32_t imageSize = extent.width * extent.height * 4;
			VkImage image = getEnginePointer()->getRenderer()->getSwapChainImage();

			uint8_t* dataImage = new uint8_t[imageSize];

			vh::vhBufCopySwapChainImageToHost(getEnginePointer()->getRenderer()->getDevice(),
				getEnginePointer()->getRenderer()->getVmaAllocator(),
				getEnginePointer()->getRenderer()->getGraphicsQueue(),
				getEnginePointer()->getRenderer()->getCommandPool(),
				image, VK_FORMAT_R8G8B8A8_UNORM,
				VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				dataImage, extent.width, extent.height, imageSize);

			m_numScreenshot++;

			std::string name("media/screenshots/screenshot" + std::to_string(m_numScreenshot - 1) + ".jpg");
			stbi_write_jpg(name.c_str(), extent.width, extent.height, 4, dataImage, 4 * extent.width);
			delete[] dataImage;

			m_makeScreenshot = false;
		}
	}
}