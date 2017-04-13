//C++ file
//***********************************************
//
// Filename: main.cpp
//
// Author: ShenChengEn - ubuntu733@gmail.com
// Description: ---
// Create: 2017-03-17 15:48:38
// Last Modified: 2017-03-28 17:10:09
//***********************************************

#include "DMCore/DMCore.h"
#include "DMCore/message/threadsafe_queue.h"
#include "DMCore/message/message.h"
#include "Utils/Utils.h"
#include <thread>
#include <iostream>
std::string analysisInput(std::string input) {
  return input;
}

std::string output(std::string prompt) {
  return prompt;
}


int main(){
  std::thread t(DialogCoreThread);
  t.detach();
  //int i=0;
  bool flag = true;
  while(flag) {
    std::string output_string,tmp;
    tmp = *waitMessage(output_queue);
    output_string = tmp.substr(1,tmp.size());
    //std::cout << "--------"+   output_string + "--------"<<std::endl;
    string state,value;
    SplitOnFirst(output_string,"|",state,value);
    //std::cout << "state:"+state+",value="+value<<std::endl;
    std::string input;
    if(state == "inform"){
      printf("%s\n",value.c_str());
    }else if(state == "request"){
      printf("%s\n",value.c_str());
      printf("%s\n","Please enter words");
      getline(std::cin,input);
      pushMessage(input_queue,input);
    } else if (state == "end"){
      printf("%s\n",value.c_str());
      break;
    }
    
  }
  return 1;
}
