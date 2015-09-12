/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2015 Stephen Kelly <steveire@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#pragma once

#include "cmState.h"

#if defined(CMAKE_BUILD_WITH_CMAKE)
# include "cm_jsoncpp_value.h"
# include "cm_jsoncpp_writer.h"
# include "cm_jsoncpp_reader.h"
# include <uv.h>
#endif

class cmMetadataServer
{
public:
  cmMetadataServer();

  ~cmMetadataServer();

  void ServeMetadata(const std::string& buildDir);

  void PopOne();

  void handleData(std::string const& data);

private:
  void processRequest(const std::string& json);

  void ProcessHandshake(const std::string& protocolVersion);
  void ProcessVersion();

  void writeContent(cmState::Snapshot snp);

private:
  void WriteResponse(Json::Value const& jsonValue);

  std::string m_buildDir;
  cmake *CMakeInstance;
  std::vector<std::string> mQueue;
  std::string mDataBuffer;
  std::string mJsonData;

  uv_loop_t *mLoop;
  uv_pipe_t mStdin_pipe;
  uv_pipe_t mStdout_pipe;

  enum ServerState {
    Uninitialized,
    Started,
    Initializing,
    ProcessingRequests
  };

  ServerState State;
  bool Writing;
};
