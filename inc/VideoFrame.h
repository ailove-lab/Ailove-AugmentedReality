#ifndef _VIDEOFRAME_
#define _VIDEOFRAME_

#include <boost/shared_ptr.hpp>

namespace cv {
    class Mat;
}

//-----------------------------------------------------------------------------

/** Stores one frame from video input */
class VideoFrame {
public:
    /** Initialize frame by matrix and number. */
    VideoFrame(boost::shared_ptr<cv::Mat> frame, int number);
    virtual ~VideoFrame(void);

    /** Returns frame data without transformation */
    const unsigned char* getFrame() const;

    /** Returns frame data transformed to RGBA format (for Ogre compability) */
    const unsigned char* getFrameRGBA();

    /** Returns frame number */
    int getFrameNumber() const {
        return frameNumber;
    }

    int getWidth() const;
    int getHeight() const;

private:

    boost::shared_ptr<cv::Mat> frame;
    boost::shared_ptr<cv::Mat> rgbaFrame;
    int frameNumber;
};

//-----------------------------------------------------------------------------

#endif

