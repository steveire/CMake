/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2015 Stephen Kelly <steveire@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#ifndef cmMetadataWriter_h
#define cmMetadataWriter_h

#include "cmStandardIncludes.h"

class cmGlobalGenerator;

class cmMetadataWriter
{
public:
  cmMetadataWriter(cmGlobalGenerator* gg);

  void Write(const char* version);

private:
  void Write(std::string version, std::string config);

private:
  cmGlobalGenerator* GG;
};

#endif
