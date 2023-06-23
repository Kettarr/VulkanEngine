#include "VEEventListener.h"
#include <cstdint>
#include <string>
#ifndef MYVEEVENTLISTENER_H
#define MYVEEVENTLISTENER_H

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
	class MyVEEventListener : public VEEventListener
	{
	protected:
		
		bool m_makeScreenshot = false; ///<Should I make a screeshot after frame is done?
		bool m_makeScreenshotDepth = false; ///<Should I make a screeshot after frame is done?
		uint32_t m_numScreenshot = 0; ///<Screenshot ID
		double global_time = 0; //global Time

		virtual void onFrameEnded(veEvent event, bool t);


	public:
		///Constructor
		MyVEEventListener(std::string name)
			: VEEventListener(name) {};

		///Destructor
		virtual ~MyVEEventListener() {};
	};
} // namespace ve

#endif