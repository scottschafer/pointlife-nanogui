#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <map>
#include "getOpenCLCode.h"

const std::string path = "/Users/sschafer/projhome/MegaTriBots/src/gpuOrganism/core/";

static std::map<std::string, std::string> filenameToContents;
static std::string headers;

std::string get_file_contents(const char *filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    std::ostringstream contents;
    contents << in.rdbuf();
    in.close();
    return(contents.str());
  }
  throw(errno);
}

inline std::string getCachedFile(std::string filename) {
  if (filenameToContents.count(filename) == 1) {
    return filenameToContents[filename];
  }
  std::string pathToFile = path + filename;
  if (pathToFile.find(".") == std::string::npos) {
    pathToFile += "_cl.h";
  }
  
  std::string contents = get_file_contents(pathToFile.c_str());
  filenameToContents[filename] = contents;
  return contents;
}

std::string getOpenCLCode(std::string filename) {
  if (headers.length() == 0) {
    headers = getCachedFile("_inc/CellVO.h") +
      getCachedFile("_inc/VO.h"); /* +
      getCachedFile("_inc/VO_size.h") */;
  }
  return headers + getCachedFile(filename);
  /*
  if (filenameToContents.count(filename) == 1) {
    return filenameToContents[filename];
  }
  std::string pathToFile = path + filename + "_cl.h";
  std::string contents = get_file_contents(pathToFile.c_str());
  filenameToContents[filename] = contents;
  return contents;
  */
}
