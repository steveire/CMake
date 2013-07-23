/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2013 Stephen Kelly <steveire@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#ifndef cmCPackBARGenerator_h
#define cmCPackBARGenerator_h


#include "cmCPackGenerator.h"

/** \class cmCPackBARGenerator
 * \brief A generator for BAR packages
 *
 */
class cmCPackBARGenerator : public cmCPackGenerator
{
public:
  cmCPackTypeMacro(cmCPackBARGenerator, cmCPackGenerator);

  /**
   * Construct generator
   */
  cmCPackBARGenerator();
  virtual ~cmCPackBARGenerator();

  virtual int PackageFiles();

  static bool CanGenerate();

protected:
  virtual const char* GetOutputExtension() { return ".bar"; }
};

#endif
