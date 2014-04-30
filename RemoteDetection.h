#include <iostream>
#include <opencv2/core/core.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <zmq.hpp>

class RemoteDetection {
  public:
    RemoteDetection();                       
    bool                                        detect(cv::Mat image, int modelId);
    void                                        paint(cv::Mat& image);
    void                                        set(std::string server, int port, std::string name);

    std::vector<cv::Rect>                       getRects();
    std::vector<float>                          getScore();
    std::vector<float>                          getScore2();
    std::vector<std::vector<cv::Rect>>          getRectsPart();

  private:

    std::string                                 m_Server;
    std::string                                 m_Name;
    int                                         m_Port;
    std::vector<cv::Rect>                       m_rects;
    std::vector<std::vector<cv::Rect>>          m_rectsParts;
    std::vector<float>                          m_score;
    std::vector<float>                          m_score2;
    bool                                        sendrequest(cv::Mat image);
    float                                       stringToFloat(const std::string& s);
    int                                         stringToInt(const std::string& s);
    
};