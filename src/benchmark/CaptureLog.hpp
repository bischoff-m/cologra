#pragma once

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class CaptureLog {
private:
  streambuf *oldCout;
  streambuf *oldCerr;
  stringstream bufferCout;
  stringstream bufferCerr;

public:
  CaptureLog();
  ~CaptureLog();
  string getCout();
  string getCerr();
};
