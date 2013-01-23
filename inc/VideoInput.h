#ifndef _VIDEOINPUT_
#define _VIDEOINPUT_

#include <boost/shared_ptr.hpp>

class VideoFrame;

namespace cv {
    class VideoCapture;
    class Mat;
}

//-----------------------------------------------------------------------------

/** Captures video frames.*/
class VideoInput {
public:
    /** Creates video input. */
    VideoInput();
    virtual ~VideoInput(void);

    void init();

    /** Updates a current frame. */
    virtual void update();

    /** Returns currentFrame. */
    virtual boost::shared_ptr<VideoFrame> getFrame();

protected:
    boost::shared_ptr<cv::VideoCapture> capturer;
    boost::shared_ptr<VideoFrame> frame;
    int frameNumber;
};

//-----------------------------------------------------------------------------

#endif