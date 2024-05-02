#include <iostream>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

const int IMAGE_HEIGHT = 20; // This affects how many tasks OpenCV splits task into

void reportThreads() 
{
  std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Otherwise process reported thread count can be behind
  int numThreads = cv::getNumThreads();
  std::cout << "cv2.getNumThreads: " << numThreads << std::endl;
}

void runTbb() 
{
  cv::Mat image = cv::Mat::zeros(IMAGE_HEIGHT, 1000, CV_8UC1);
  cv::randu(image, cv::Scalar::all(0), cv::Scalar::all(256));
  std::vector < cv::Mat > images;
  images.push_back(image);
  cv::Mat hist;
  cv::calcHist(images, 0, cv::Mat(), hist, 1, & 256, cv::Scalar::all(0), cv::Scalar::all(256));
  std::cout << "TBB used" << std::endl;
  reportThreads();
}

void runTbbThread() 
{
  //cv::setNumThreads(3); // Calling this here will cause a segfault when run_tbb() is then called outside this thread
  reportThreads();
  runTbb();
  runTbb();
}

int main() 
{
  std::cout << "cv2.getNumThreads: " << cv::getNumThreads() << ", Image height: " << IMAGE_HEIGHT << std::endl;
  cv::setNumThreads(3);
  reportThreads();
  runTbb();
  runTbb();
  std::cout << "Entering thread" << std::endl;
  std::thread thread(runTbbThread);
  thread.join();
  std::cout << "Finished thread" << std::endl;
  reportThreads();
  runTbb();
  runTbb();
  return 0;
}
