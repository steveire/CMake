/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2014 Stephen Kelly <steveire@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#ifndef cmCPackAPKGenerator_h
#define cmCPackAPKGenerator_h


#include "cmCPackGenerator.h"

/** \class cmCPackAPKGenerator
 * \brief A generator for APK packages
 *
 */
class cmCPackAPKGenerator : public cmCPackGenerator
{
public:
  cmCPackTypeMacro(cmCPackAPKGenerator, cmCPackGenerator);

  /**
   * Construct generator
   */
  cmCPackAPKGenerator();
  virtual ~cmCPackAPKGenerator();

  virtual int PackageFiles();

  static bool CanGenerate();

protected:
  virtual const char* GetOutputExtension() { return ".bar"; }
};

#endif
