#include "CaptureLog.hpp"
#include <iostream>
#include <sstream>
#include <string>

CaptureLog::CaptureLog() : bufferCout(), bufferCerr() {
  oldCout = cout.rdbuf(bufferCout.rdbuf());
  oldCerr = cerr.rdbuf(bufferCerr.rdbuf());
}

CaptureLog::~CaptureLog() {
  cout.rdbuf(oldCout);
  cerr.rdbuf(oldCerr);
}

string CaptureLog::getCout() { return bufferCout.str(); }

string CaptureLog::getCerr() { return bufferCerr.str(); }
