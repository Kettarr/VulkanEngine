#pragma once
#include "VEInclude.h"
#include "VEEventListener.h"
#include <cstdint>
#include <string>
#include "FFMPEGEncoder.h"

#ifndef FFMPEGLISTENER_H
#define FFMPEGLISTENER_H

#include "UDPReceiver.h"

namespace ve

{
	/**
		*
		* \brief An event listener for GLFW events
		*
		* This is the default event listener for GLFW events. Basically it steers the standard camera around,
		* just as you would expect from a first person shooter.
		*
		*/
	class FFMPEGListener : public VEEventListener
	{
	private: 
		FFMPEGEncoder ffmpegencoder;
		UDPReceiver rec;
		const timeval* tv;
		VEWindow* window = g_pVEWindowSingleton;
	
		
	protected:
		double global_time = 0;
		double frame_time = 0;
		virtual void onFrameEnded(veEvent event);
		virtual void handleInputMouse(float x, float y);
		virtual void handleInput(int input, double dt);


	public:
		///Constructor
		FFMPEGListener(std::string name)
			: VEEventListener(name) {
			ffmpegencoder.setup();
			rec.startWinsock();
			rec.init(50001);
			timeval mtv = { .tv_sec = 0,
		.tv_usec = 10 };
			tv = &mtv;
		
			

		};


		///Destructor
		virtual ~FFMPEGListener() {
			WSACleanup();
		};
	};
} // namespace ve

#endif