#include "VideoInput.h"
#include "VideoFrame.h"
#include <highgui.h>
#include <cv.h>
#include <OgreLogManager.h>
#include "ARException.h"


//-----------------------------------------------------------------------------

VideoInput::VideoInput()
	: frameNumber(0)
{
}

//-----------------------------------------------------------------------------

VideoInput::~VideoInput(void)
{
}

//-----------------------------------------------------------------------------

void VideoInput::init() 
{
    try {
        capturer.reset(new cv::VideoCapture(0));

        int type = CV_8UC3;
	    capturer->set(CV_CAP_PROP_FORMAT, type);
    } catch (...) {
        throw ARException::CAN_NOT_INIT_CAMERA;
    }

    if (!capturer->isOpened()) {
        throw ARException::CAN_NOT_INIT_CAMERA;
    }
}

//-----------------------------------------------------------------------------

void VideoInput::update() 

{

	boost::shared_ptr<cv::Mat> mat(new cv::Mat());
	*capturer >> *mat;
    //boost::shared_ptr<cv::Mat> flipped(new cv::Mat());
    // cv::flip(*mat, *flipped, 1);
	frame.reset(new VideoFrame(mat, frameNumber++));
}

//-----------------------------------------------------------------------------

boost::shared_ptr<VideoFrame> VideoInput::getFrame()

{
	return frame;
}

//-----------------------------------------------------------------------------
