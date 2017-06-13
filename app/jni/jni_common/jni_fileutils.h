
#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace jniutils {

bool fileExists(const char* name);

bool dirExists(const char* name);

bool fileExists(const std::string& name);

bool dirExists(const std::string& name);

}  // end jniutils
   /* FILEUTILS_H */
