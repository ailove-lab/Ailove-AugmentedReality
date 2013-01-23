#include "VideoFrame.h"
#include <cv.h>

//-----------------------------------------------------------------------------

VideoFrame::VideoFrame(boost::shared_ptr<cv::Mat> frame, int number)
    : frameNumber(number), frame(frame)
{
}

//-----------------------------------------------------------------------------

VideoFrame::~VideoFrame(void)
{
}

//-----------------------------------------------------------------------------

const unsigned char* VideoFrame::getFrame() const 

{
    return frame.get() ? frame->data : NULL;
}

//-----------------------------------------------------------------------------
    
const unsigned char* VideoFrame::getFrameRGBA() 

{
    if (rgbaFrame.get()) {
        return rgbaFrame->data;
    }

    if (frame.get()) {
        rgbaFrame.reset(new cv::Mat());
        cv::cvtColor(*frame, *rgbaFrame, CV_RGB2RGBA);
        return rgbaFrame->data;
    }

    return NULL;
}

//-----------------------------------------------------------------------------

int VideoFrame::getWidth() const 
{
    return frame.get() ? frame->cols : -1;
}

//-----------------------------------------------------------------------------
    
int VideoFrame::getHeight() const 
{
    return frame.get() ? frame->rows : -1;
}

//-----------------------------------------------------------------------------