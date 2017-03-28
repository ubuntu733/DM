#pragma once
#ifndef _MESSAGE_H_
#define _MESSAGE_H_

//***********************************************
//
// Filename: DMCore/message/message.h
//
// Author: ShenChengEn - ubuntu733@gmail.com
// Description: ---
// Create: 2017-03-22 14:33:54
// Last Modified: 2017-03-22 14:33:54
//***********************************************
//
#include "threadsafe_queue.h"
#include "memory"
extern threadsafe_queue<std::string> input_queue;
extern threadsafe_queue<std::string> output_queue;

template<class T>
void pushMessage(threadsafe_queue<T> &q,T value) {
  q.push(value);
}



template<typename T>
std::shared_ptr<T> waitMessage(threadsafe_queue<T> &q) {
  return q.wait_and_pop();
}

#endif
