#include "RemoteDetection.h"
#include <opencv2/highgui/highgui.hpp>

//Demo
int main( int argc, char** argv )
{
    //params
    std::string server  = "dst-ws-dell-05.etit.tu-chemnitz.de";
    std::string name    = "Electronic card";
    int         port    = 6003;
    int         model   = 26;

    //read image
    cv::Mat image = cv::imread("devcard.png",1);
    
    //object detector
    RemoteDetection remotedetection;
    remotedetection.set(server,port,name);

    //detect object
    bool isDetected = remotedetection.detect(image,model);

    //if detected, paint
    if(isDetected)
    {
        remotedetection.paint(image);

        //get data
        std::vector<cv::Rect> rects  = remotedetection.getRects();
        std::vector<float>    scores = remotedetection.getScore();

    }

    //show image
    cv::imshow("Display",image);
    cv::waitKey(0);

    return 0;
}
