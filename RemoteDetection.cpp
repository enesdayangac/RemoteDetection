#include "RemoteDetection.h"

RemoteDetection::RemoteDetection() 
{
}

void RemoteDetection::set(std::string server, int port, std::string name)
{
    m_Server = server;
    m_Port   = port;
    m_Name   = name;
}

bool RemoteDetection::detect(cv::Mat image, int modelId) 
{
    int width = image.size().width;
    int height = image.size().height;
    int channels = image.channels();

    image.at<uchar>(0,0) = static_cast<uchar>(192); //pre-header
    image.at<uchar>(0,1) = static_cast<uchar>(width >> 8);
    image.at<uchar>(0,2) = static_cast<uchar>((&width)[0]);
    image.at<uchar>(0,3) = static_cast<uchar>(height >> 8);
    image.at<uchar>(0,4) = static_cast<uchar>((&height)[0]);
    image.at<uchar>(0,5) = static_cast<uchar>(channels);
    image.at<uchar>(0,6) = static_cast<uchar>(modelId);

    return this->sendrequest(image);
}

std::vector<cv::Rect> RemoteDetection::getRects()
{
    return m_rects;
}

std::vector<float> RemoteDetection::getScore()
{
    return m_score;
}

std::vector<float> RemoteDetection::getScore2()
{
    return m_score2;
}

std::vector<std::vector<cv::Rect>> RemoteDetection::getRectsPart()
{
    return m_rectsParts;
}

bool RemoteDetection::sendrequest(cv::Mat image) 
{
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);

    std::string ipPort= "tcp://"+ m_Server + ":" + std::to_string(m_Port);

    socket.connect (ipPort.c_str());

    int img_size = image.size().height * image.size().width * image.channels();
    zmq::message_t request(img_size);
    memcpy (request.data(),(void *) image.data, img_size);
    socket.send (request);

    // Get the reply.
    zmq::message_t reply;
    try
    {
        socket.recv (&reply);
    }
    catch(zmq::error_t &e)
    {
        std::cout<<"socket.recv exception"<<std::endl;
        return false;
    }

    int replySize = reply.size();
    std::string data((char*)reply.data(),(char*)reply.data() + replySize);
    std::string word;
    std::vector<std::string>worldvector;
    std::stringstream sss(data);
    int wordcount = 0;
    while (sss >> word)
    {
        worldvector.push_back(word);
    }

    int caseHeaderReply = stringToInt(worldvector[0]);

    switch(caseHeaderReply)
    {
        case '2':
        {
            int height1 = stringToInt(worldvector[1]);
            int width1 = stringToInt(worldvector[2]);
            int sizeFloat1 = height1 * width1;

            int height2 = stringToInt(worldvector[3]);
            int width2 = stringToInt(worldvector[4]);
            int sizeFloat2 = height2 * width2;

            cv::Mat bbox1 = cv::Mat::zeros(height1,width1,CV_32FC1);
            for(int i = 0; i < sizeFloat1; i++)
            {
                bbox1.at<float>(i) = stringToFloat(worldvector[5 + i]);
            }

            cv::Mat bbox2 = cv::Mat::zeros(height2,width2,CV_32FC1);
            for(int i = 0; i < sizeFloat2; i++)
            {
                bbox2.at<float>(i) = stringToFloat(worldvector[5 + sizeFloat1 + i]);
            }

            m_rects.clear();
            m_score.clear();
            m_score2.clear();
            m_rectsParts.clear();

            for(int i= 0; i < bbox1.rows; i++)
            {
                int height = bbox1.at<float>(i,3) - bbox1.at<float>(i,1);
                int width = bbox1.at<float>(i,2) - bbox1.at<float>(i,0);
                
                cv::Rect rec(bbox1.at<float>(i,0),bbox1.at<float>(i,1),width,height);
                
                //push back the values
                m_rects.push_back(rec);
            }

            std::vector<std::vector<cv::Rect>>       rectsPartsTemp;

            for(int i = 0; i < bbox2.rows; i++)
            {
                //consider one row
                cv::Mat bbox2NthRow = bbox2.row(i);

                int elementCount = (width2 -2);

                std::vector<cv::Rect> rowRectsTemp;

                for(int j = 0; j < elementCount; j = j + 4)
                {
                    int height = bbox2NthRow.at<float>(0,j+3) - bbox2NthRow.at<float>(0,j+1);
                    int width = bbox2NthRow.at<float>(0,j+2) - bbox2NthRow.at<float>(0,j);

                    cv::Rect rec(bbox2NthRow.at<float>(0,j),bbox2NthRow.at<float>(0,j+1),width,height);
                    rowRectsTemp.push_back(rec);
                }

                rectsPartsTemp.push_back(rowRectsTemp);

                m_score.push_back(bbox2NthRow.at<float>(0,elementCount + 1));
                m_score2.push_back(bbox2NthRow.at<float>(0,elementCount));

            }

            m_rectsParts = rectsPartsTemp;
            return true;

        break;
        }
        case '3':
        {
            std::cout<<"The code is still running... Wait!"<<std::endl;
            return false;
        break;
        }
        case '4':
        {
            std::cout<<"Object is NOT detected"<<std::endl;
            return false;
        break;
        }
        case '5':
        {
            std::cout<<"Failure"<<std::endl;
            return false;
            break;
        }
        default:
        {
            std::cout<<"Case NOT found!"<<std::endl;
            return false;
        break;
        }
    }
}

float RemoteDetection::stringToFloat(const std::string& s)
{
    std::istringstream i(s);
    float x;
    if(!(i >>x))
        return 0.;
    return x;
} 

int RemoteDetection::stringToInt(const std::string& s)
{
    std::istringstream i(s);
    int x;
    if(!(i >>x))
        return 0;
    return x;
}

void RemoteDetection::paint(cv::Mat& image)
{
    //objectname
    for(int i= 0; i < m_rects.size(); i++)
    {
        cv::putText(image, m_Name, m_rects[i].tl() , cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0,127,255),2,8,false);
    }

    //rect
    for(int i= 0; i < m_rects.size(); i++)
    {
        cv::rectangle(image, m_rects[i], cv::Scalar(255,0,0),2,8,0);
    }

    //part number
    if (!m_rectsParts.empty())
    {
        std::vector<cv::Rect> objectparts = m_rectsParts[0];

        for (int j= 0; j < objectparts.size(); ++j)
        {
            cv::rectangle(image,objectparts[j], cv::Scalar(0,255,0),2,8,0);
            cv::putText(image, std::to_string(j), objectparts[j].br() , cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(127,0,255),2,8,false);
        }
    }
}
