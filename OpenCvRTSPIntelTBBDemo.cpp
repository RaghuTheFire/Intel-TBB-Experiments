#include <iostream>
#include <thread>
#include <tbb/tbb.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void processFrame(const Mat & frame) 
{
  // Process the frame here
  // Example: Display the frame
  imshow("Frame", frame);
  waitKey(1);
}

void readRTSPStream(const string & rtspUrl) 
{
  VideoCapture cap(rtspUrl);
  if (!cap.isOpened()) 
  {
    cerr << "Failed to open RTSP stream" << endl;
    return;
  }

  Mat frame;
  while (cap.read(frame)) 
  {
    processFrame(frame);
  }
}

int main() 
{
  string rtspUrl = "rtsp://example.com/stream"; // Replace with your RTSP stream URL

  tbb::task_scheduler_init init; // Initialize TBB task scheduler

  // Create a TBB parallel_for loop to read the RTSP stream using multiple threads
  tbb::parallel_for(tbb::blocked_range < int > (0, tbb::task_scheduler_init::default_num_threads()),
    [ & ](const tbb::blocked_range < int > & range) {
      for (int i = range.begin(); i < range.end(); ++i) 
      {
        readRTSPStream(rtspUrl);
      }
    });

  return 0;
}
