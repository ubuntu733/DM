//
// Created by alex on 17-3-22.
//

#ifndef TEST2_THREADSAFE_QUEUE_H
#define TEST2_THREADSAFE_QUEUE_H


//***********************************************
//
// Filename: DMCore/message/threadsafe_queue.h
//
// Author: ShenChengEn - ubuntu733@gmail.com
// Description: ---
// Create: 2017-03-22 10:28:11
// Last Modified: 2017-03-22 10:28:11
//***********************************************
//




#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
template<typename T>
class threadsafe_queue {
public:
  threadsafe_queue();

  ~threadsafe_queue();

  void wait_and_pop(T& value);

  std::shared_ptr<T> wait_and_pop();

  bool try_pop(T& value);

  std::shared_ptr<T> try_pop();

  bool empty() const;

  void push(T new_value);

public:
  mutable std::mutex mut;
  std::queue<std::shared_ptr<T> > data_queue;
  std::condition_variable data_cond;
};
template<typename T>
threadsafe_queue<T>::threadsafe_queue(){

}

template<typename T>
threadsafe_queue<T>::~threadsafe_queue(){}

template<typename T>
void threadsafe_queue<T>::wait_and_pop(T& value) {
  std::unique_lock<std::mutex> lk(mut);
  data_cond.wait(lk,[this]{return !data_queue.empty();});
  value=std::move(*data_queue.front());
  data_queue.pop();
}

template<typename T>
bool threadsafe_queue<T>::try_pop(T& value) {
  std::lock_guard<std::mutex> lk(mut);
  if(data_queue.empty())
    return false;
  value=std::move(*data_queue.front());
  data_queue.pop();
}

template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::wait_and_pop() {
  std::unique_lock<std::mutex> lk(mut);
  data_cond.wait(lk,[this]{return !data_queue.empty();});
  std::shared_ptr<T> res=data_queue.front();
  data_queue.pop();
  return res;
}

template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::try_pop() {
  std::lock_guard<std::mutex> lk(mut);
  if(data_queue.empty())
    return std::shared_ptr<T>();
  std::shared_ptr<T> res=data_queue.front();
  data_queue.pop();
  return res;
}

template<typename T>
bool threadsafe_queue<T>::empty() const {
  std::lock_guard<std::mutex> lk(mut);
  return data_queue.empty();
}

template<typename T>
void  threadsafe_queue<T>::push(T new_value) {
  std::shared_ptr<T> data(
          std::make_shared<T>(std::move(new_value)));
  std::lock_guard<std::mutex> lk(mut);
  data_queue.push(data);
  data_cond.notify_one();
}


#endif //TEST2_THREADSAFE_QUEUE_H
