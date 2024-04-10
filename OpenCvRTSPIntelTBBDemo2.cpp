/****
threads for continuously grabbing and displaying  1000 rtsp video streams from config files using opencv and intel tbb concurrent queues 
****/

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <opencv2/opencv.hpp>
#include <tbb/concurrent_queue.h>

std::vector < std::string > rtsp_urls;
tbb::concurrent_queue < cv::Mat > frame_queue;
std::mutex queue_mutex;
std::condition_variable queue_cv;
bool stop_threads = false;

void grab_frame(const std::string & url) 
{
  cv::VideoCapture cap(url);
  if (!cap.isOpened()) {
    std::cerr << "Failed to open RTSP stream: " << url << std::endl;
    return;
  }

  while (!stop_threads) 
  {
    cv::Mat frame;
    if (cap.read(frame)) 
    {
      frame_queue.push(frame);
      queue_cv.notify_one();
    }
  }
  cap.release();
}

void display_frames() 
{
  while (!stop_threads) 
  {
    cv::Mat frame;
    {
      std::unique_lock < std::mutex > lock(queue_mutex);
      queue_cv.wait(lock, [ & ] 
      {
        return !frame_queue.empty() || stop_threads;
      });
      if (stop_threads) break;
      frame_queue.try_pop(frame);
    }

    cv::imshow("RTSP Streams", frame);
    cv::waitKey(1);
  }
}

int main() 
{
  // Load RTSP URLs from config files
  rtsp_urls = {
    "rtsp://example.com/stream1",
    "rtsp://example.com/stream2",
    /* add more URLs */
  };

  // Start frame grabbing threads
  std::vector < std::thread > grab_threads;
  for (const auto & url: rtsp_urls) 
  {
    grab_threads.emplace_back(grab_frame, url);
  }

  // Start frame display thread
  std::thread display_thread(display_frames);

  // Wait for user input to stop the program
  std::string input;
  std::getline(std::cin, input);
  // Signal threads to stop
  stop_threads = true;
  queue_cv.notify_all();

  // Wait for threads to finish
  for (auto & thread: grab_threads) 
  {
    thread.join();
  }
  display_thread.join();

  return 0;
}
