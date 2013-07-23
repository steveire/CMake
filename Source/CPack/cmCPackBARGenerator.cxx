/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2013 Stephen Kelly <steveire@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#include "cmCPackBARGenerator.h"

#include "cmCPackLog.h"

//----------------------------------------------------------------------------
cmCPackBARGenerator::cmCPackBARGenerator()
{

}


//----------------------------------------------------------------------------
cmCPackBARGenerator::~cmCPackBARGenerator()
{

}

//----------------------------------------------------------------------------
bool cmCPackBARGenerator::CanGenerate()
{
  std::string nativePackagerPath =
                      cmSystemTools::FindProgram("blackberry-nativepackager");

  return nativePackagerPath != "" ? true : false;
}

//----------------------------------------------------------------------------
int cmCPackBARGenerator::PackageFiles()
{
  std::string nativePackagerPath =
                      cmSystemTools::FindProgram("blackberry-nativepackager");
  std::string command = nativePackagerPath;
  command += " -package " + packageFileNames[0];

  const char *barDescriptor = this->GetOption("CPACK_BAR_DESCRIPTOR");

  std::string tempDir = this->GetOption("CPACK_TEMPORARY_DIRECTORY");
  std::string generatedDescriptor = tempDir;
  generatedDescriptor += "/bar-descriptor.xml";

  if (barDescriptor)
    {
    cmSystemTools::CopyFileIfDifferent(barDescriptor, tempDir.c_str());
    }
  else
    {
    std::string res;
    {
    std::string inFile = this->GetOption("CMAKE_ROOT");
    inFile += "/Modules/bar-descriptor-header.xml.in";
    std::ifstream ifs(inFile.c_str());
    std::string packageHeaderText;
    std::string line;
    while ( cmSystemTools::GetLineFromStream(ifs, line) )
      {
      packageHeaderText += line + "\n";
      }

    const char *extraLdLibraryPaths =
                          this->GetOption("CPACK_BAR_EXTRA_LD_LIBRARY_PATHS");
    if (extraLdLibraryPaths)
      {
      std::string processed = ":";
      processed += extraLdLibraryPaths;
      std::replace(processed.begin(), processed.end(), ';', ':');
      this->SetOption("CPACK_BAR_EXTRA_LD_LIBRARY_PATHS", processed.c_str());
      }

    this->ConfigureString(packageHeaderText, res);
    }
    const char *entryPoint = this->GetOption("CPACK_BAR_ENTRY_POINT");

    if (!entryPoint)
      {
      cmCPackLogger(cmCPackLog::LOG_ERROR,
        "CPACK_BAR_ENTRY_POINT must be set." << std::endl);
      return 0;
      }

    std::vector<std::string> binaryFiles;
    cmSystemTools::ExpandListArgument(this->GetOption("CPACK_BAR_BINARIES"),
                                      binaryFiles);

    res += "\n";
    for(std::vector<std::string>::const_iterator it = this->files.begin();
        it != this->files.end(); ++it)
      {
      res += "    <asset path=\"";
      std::string relPath =
        cmSystemTools::RelativePath(
            tempDir.c_str(),
            it->c_str());

      res += relPath + "\"";
      std::string fname = cmSystemTools::GetFilenameName(*it);
      const bool isEntryPoint = fname == entryPoint;
      if (isEntryPoint)
        {
        res += " entry=\"true\"";
        }

      if (std::find(binaryFiles.begin(), binaryFiles.end(),
          ("./" + relPath)) != binaryFiles.end())
        {
        res += " type=\"Qnx/Elf\"";
        }

      res += ">" + relPath +"</asset>\n";

      const char *c = this->GetOption("CPACK_BUILD_CONFIG");
      std::string config =
              cmSystemTools::UpperCase(c?c:"");

      if(isEntryPoint
          && config == "RELEASE"
          && this->IsOn("CPACK_BAR_CASCADES"))
        {
        res += "    <entryPointType>Qnx/Cascades</entryPointType>\n";
        }
      }

    if(const char *category = this->GetOption("CPACK_BAR_CATEGORY"))
      {
      res += "\n    <category>";
      res += category;
      res += "</category>\n";
      }
    if(const char *permissions = this->GetOption("CPACK_BAR_PERMISSIONS"))
      {
      res += "\n";
      std::vector<std::string> perms;
      cmSystemTools::ExpandListArgument(permissions, perms);
      for(std::vector<std::string>::const_iterator it = perms.begin();
          it != perms.end(); ++it)
        {
        res += "    <permission>";
        res += *it;
        res += "</permission>\n";
        }
      }
    if(const char *authorId = this->GetOption("CPACK_BAR_AUTHOR_ID"))
      {
      res += "\n";
      if (this->GetOption("CPACK_BAR_AUTHOR_ID_FROM_DEBUG_TOKEN"))
        {
        cmCPackLogger(cmCPackLog::LOG_ERROR,
          "Incompatible package specification." << std::endl);
        return 0;
        }
      res += "    <authorId>";
      res += authorId;
      res += "</authorId>\n";
      }
    else if(const char *token
                    = this->GetOption("CPACK_BAR_AUTHOR_ID_FROM_DEBUG_TOKEN"))
      {
      command += " -devMode -debugToken ";
      command += token;
      }

    const char *chrome = this->GetOption("CPACK_BAR_WINDOW_CHROME");
    const char *transparent = this->GetOption("CPACK_BAR_WINDOW_TRANSPARENT");
    const char *orientation = this->GetOption("CPACK_BAR_WINDOW_ORIENTATION");

    if(chrome || transparent || orientation)
      {
      res += "\n";
      res += "    <initialWindow>\n";
      if(chrome)
        {
        res += "        <systemChrome>";
        res += chrome;
        res += "</systemChrome>\n";
        }
      if(transparent)
        {
        res += "        <transparent>true</transparent>\n";
        }
      if(orientation)
        {
        if (strcmp(orientation, "Default"))
          {
          // Do nothing.
          }
        else if (strcmp(orientation, "Portrait"))
          {
          res += "        <autoOrients>false</autoOrients>\n";
          res += "        <aspectRatio>portrait</aspectRatio>\n";
          }
        else if (strcmp(orientation, "Landscape"))
          {
          res += "        <autoOrients>false</autoOrients>\n";
          res += "        <aspectRatio>landscape</aspectRatio>\n";
          }
        else if (strcmp(orientation, "Auto"))
          {
          res += "        <autoOrients>true</autoOrients>\n";
          }
        else
          {
          // Error. Invalid value.
          }
        res += "        <systemChrome>";
        res += chrome;
        res += "</systemChrome>";
        }
      res += "    </initialWindow>\n";
      }

    std::vector<std::string> opts = this->GetOptions();
    for(std::vector<std::string>::const_iterator
        it = opts.begin(); it != opts.end(); ++it)
      {
      if (cmHasLiteralPrefix(*it, "CPACK_BAR_ENV_VAR_"))
        {
        std::string var = it->c_str() + sizeof("CPACK_BAR_ENV_VAR_") - 1;
        res += "    <env var=\"" + var + "\" "
                "value=\"" + this->GetOption(*it) + "\"/>\n";
        }
      }



    {
    std::string inFile = this->GetOption("CMAKE_ROOT");
    inFile += "/Modules/bar-descriptor-footer.xml.in";
    std::ifstream ifs(inFile.c_str());
    std::string packageFooterText;
    std::string line;
    while ( cmSystemTools::GetLineFromStream(ifs, line) )
      {
      packageFooterText += line + "\n";
      }
    std::string foot;
    this->ConfigureString(packageFooterText, foot);
    res += foot;
    }

    std::ofstream ofs(generatedDescriptor.c_str());

    ofs << res;
    ofs.close();
    }

  command += " ";
  command += generatedDescriptor;

  std::string output;
  int retVal = 1;

  cmSystemTools::RunSingleCommand(command.c_str(), &output,
                                             &retVal, 0,
                                             this->GeneratorVerbose, 0);

  return files.size();
}
