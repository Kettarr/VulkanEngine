#include "VEInclude.h"
#include "FFMPEGListener.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <iostream>




extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}





namespace ve
{
	void FFMPEGListener::onFrameEnded(veEvent event) {


		global_time = global_time + event.dt;
		frame_time = frame_time + event.dt;

		double ft = 0.005;

		if (FFMPEGListener::frame_time >= ft) {



			//get the image size and image itself
			VkExtent2D extent = getWindowPointer()->getExtent();
			uint32_t imageSize = extent.width * extent.height * 4;
			VkImage image = getEnginePointer()->getRenderer()->getSwapChainImage();

			uint8_t* dataImage = new uint8_t[imageSize];

			//copy image from buffer
			vh::vhBufCopySwapChainImageToHost(getEnginePointer()->getRenderer()->getDevice(),
				getEnginePointer()->getRenderer()->getVmaAllocator(),
				getEnginePointer()->getRenderer()->getGraphicsQueue(),
				getEnginePointer()->getRenderer()->getCommandPool(),
				image, VK_FORMAT_R8G8B8A8_UNORM,
				VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				dataImage, extent.width, extent.height, imageSize);

			ffmpegencoder.setHeight(extent.height);
			ffmpegencoder.setWidth(extent.width);


			ffmpegencoder.FFMPEGencode(dataImage);
			ffmpegencoder.cter++;

			delete[] dataImage;
			FFMPEGListener::frame_time = 0;
		}

		FD_SET mySet;
		long rc;
		FD_ZERO(&mySet);
		FD_SET(rec.sock, &mySet);

		rc = select(0, &mySet, NULL, NULL, tv);

		if (rc == SOCKET_ERROR) {
			printf("ERROR: select, error code: %s\n", WSAGetLastError());
			return;
		}


		

		//std::cout << "RC: " << rc << std::endl;

		if (rc > 0)
		{
			std::cout << "receive" << std::endl;
			Inputs report;
	
			report = rec.receiveInputs();
			std::cout << "receiesInputs" << std::endl;


			if (!report.mouse) {
				handleInput(report.keyA, event.dt);
				handleInput(report.keyB, event.dt);
			}
			else {
				handleInputMouse(float(report.x), float(report.y));
			}

			
				

		}

		/*if (FFMPEGListener::global_time >= 10.1) {
			//std::cout << "receive" << std::endl;
			Report report;
			report = rec.receive();

			if (report.recPackets < 0.9) {
				ft = ft + 0.0025;
			}

			std::cout << "Framerate: " << report.framerate << std::endl;
			std::cout << "Percent lost packages: " << 1.0-report.recPackets << std::endl;
			std::cout << "received Bytes: " << report.recBytes << std::endl;





			FFMPEGListener::global_time = 0;
		}*/



	}


	void FFMPEGListener::handleInput(int input, double dt ) {

		glm::vec4 translate = glm::vec4(0.0, 0.0, 0.0, 1.0); //total translation


		VESceneNode* player = getSceneManagerPointer()->getSceneNode("The Player");
		VESceneNode* pParent = player->getParent();




				switch (input) {

				case 97:
					translate = player->getTransform() * glm::vec4(10.0, 0.0, 0.0, 1.0);//left
					break;
				case 100:
					translate = player->getTransform() * glm::vec4(-10.0, 0.0, 0.0, 1.0); //right
					break;
				case 119:
					translate = player->getTransform() * glm::vec4(0.0, 0.0, -10.0, 1.0); //forward
					translate.y = 0.0f;
					break;
				case 115:
					translate = player->getTransform() * glm::vec4(0.0, 0.0, 10.0, 1.0); //back
					translate.y = 0.0f;
					break;

				default:
					break;
				};
			
				if(pParent == nullptr)
				{
					pParent = player;
				}

				///add the new translation vector to the previous one
				float speed = 30.0f * dt;
				glm::vec3 trans = speed * glm::vec3(translate.x, translate.y, translate.z);
				pParent->multiplyTransform(glm::translate(glm::mat4(1.0f), trans));


				

		
	}

	void FFMPEGListener::handleInputMouse(float x, float y) {


			std::cout << "handle"<<x<<" "<<y << std::endl;
			VESubrender_Nuklear* pSubrender = (VESubrender_Nuklear*)getEnginePointer()->getRenderer()->getOverlay();
			if (pSubrender == nullptr) return;

			struct nk_context* ctx = pSubrender->getContext();


			nk_input_begin(ctx);
			// do a mouse move event
			nk_input_motion(ctx, x, y);
			// do left mouse button click
			nk_input_button(ctx, nk_buttons::NK_BUTTON_LEFT, x, y, 1);
			nk_input_end(ctx);


		
	}



}