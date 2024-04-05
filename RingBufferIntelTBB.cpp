#include <iostream>
#include <vector>
#include <tbb/concurrent_queue.h>
#include <tbb/parallel_for.h>

template < typename T >
  class RingBuffer 
  {
    public: RingBuffer(size_t capacity): buffer(capacity) {}

    void push(const T & item) 
    {
      buffer.push(item);
    }

    T pop() 
    {
      T item;
      buffer.try_pop(item);
      return item;
    }

    private: tbb::concurrent_bounded_queue < T > buffer;
  };

int main() 
{
  const size_t bufferSize = 10;
  const size_t numThreads = 4;

  RingBuffer < int > ringBuffer(bufferSize);

  tbb::parallel_for(tbb::blocked_range < size_t > (0, numThreads), [ & ](const tbb::blocked_range < size_t > & range) 
  {
    for (size_t i = range.begin(); i != range.end(); ++i) 
    {
      int item = i * 10;
      ringBuffer.push(item);
      std::cout << "Thread " << i << " pushed item: " << item << std::endl;
    }
  });

  tbb::parallel_for(tbb::blocked_range < size_t > (0, numThreads), [ & ](const tbb::blocked_range < size_t > & range) 
  {
    for (size_t i = range.begin(); i != range.end(); ++i) 
    {
      int item = ringBuffer.pop();
      std::cout << "Thread " << i << " popped item: " << item << std::endl;
    }
  });

  return 0;
}
