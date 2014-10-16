/*
 * Blablub
 *
 * main.cpp
 */

#include <stdio.h>

#include <boost/filesystem.hpp>
#include <vector>

#include "box.hpp"

int main(int argc, const char* argv[])
{
  boost::filesystem::path p = "/home/alex/bin/sync-box/bib";
  //boost::filesystem::path p = "/home/alex/documents/documents";
  std::vector<boost::filesystem::directory_entry> subdirs;
  Box* box = new Box(p);
  box->recursivePrint();
  //std::cout << sizeof(box) << std::endl;
}