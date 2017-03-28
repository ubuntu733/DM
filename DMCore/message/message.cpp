#include "message.h"

threadsafe_queue<std::string> input_queue;
threadsafe_queue<std::string> output_queue;
