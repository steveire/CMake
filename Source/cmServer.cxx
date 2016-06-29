/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2015 Stephen Kelly <steveire@gmail.com>
  Copyright 2016 Tobias Hunger <tobias.hunger@qt.io>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#include "cmServer.h"

#include "cmServerConnection.h"
#include "cmServerProtocol.h"
#include "cmVersionMacros.h"
#include "cmake.h"

#if defined(CMAKE_BUILD_WITH_CMAKE)
#include "cm_jsoncpp_reader.h"
#include "cm_jsoncpp_value.h"
#endif

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>

const char TYPE_KEY[] = "type";
const char COOKIE_KEY[] = "cookie";
const char REPLY_TO_KEY[] = "inReplyTo";
const char ERROR_MESSAGE_KEY[] = "errorMessage";

const char ERROR_TYPE[] = "error";
const char REPLY_TYPE[] = "reply";
const char PROGRESS_TYPE[] = "progress";
const char MESSAGE_TYPE[] = "message";

const char START_MAGIC[] = "[== CMake Server ==[";
const char END_MAGIC[] = "]== CMake Server ==]";

class cmServer::DebugInfo
{
public:
  DebugInfo()
    : StartTime(std::chrono::high_resolution_clock::now())
  {
  }

  bool PrintStatistics = false;

  std::string OutputFile;
  std::chrono::high_resolution_clock::time_point StartTime;
};

cmServer::cmServer(cmServerConnection* conn, bool supportExperimental)
  : Connection(conn)
  , SupportExperimental(supportExperimental)
{
  assert(this->Connection);
  this->Connection->SetServer(this);

  // Register supported protocols:
  this->RegisterProtocol(new cmServerProtocol1_0);
}

cmServer::~cmServer()
{
  if (!this->Protocol) { // Server was never fully started!
    return;
  }

  for (cmServerProtocol* p : this->SupportedProtocols) {
    delete p;
  }

  delete this->Connection;
}

void cmServer::PopOne()
{
  if (this->Queue.empty()) {
    return;
  }

  Json::Reader reader;
  Json::Value value;
  const std::string input = this->Queue.front();
  this->Queue.erase(this->Queue.begin());

  if (!reader.parse(input, value)) {
    this->WriteParseError("Failed to parse JSON input.");
    return;
  }

  std::unique_ptr<DebugInfo>(debug);
  Json::Value debugValue = value["debug"];
  if (!debugValue.isNull()) {
    debug = std::make_unique<DebugInfo>();
    debug->OutputFile = debugValue["dumpToFile"].asString();
    debug->PrintStatistics = debugValue["showStats"].asBool();
  }

  const cmServerRequest request(this, value[TYPE_KEY].asString(),
                                value[COOKIE_KEY].asString(), value);

  if (request.Type == "") {
    cmServerResponse response(request);
    response.SetError("No type given in request.");
    this->WriteResponse(response, nullptr);
    return;
  }

  cmSystemTools::SetMessageCallback(reportMessage,
                                    const_cast<cmServerRequest*>(&request));
  if (this->Protocol) {
    this->Protocol->CMakeInstance()->SetProgressCallback(
      reportProgress, const_cast<cmServerRequest*>(&request));
    this->WriteResponse(this->Protocol->Process(request), debug.get());
  } else {
    this->WriteResponse(this->SetProtocolVersion(request), debug.get());
  }
}

void cmServer::RegisterProtocol(cmServerProtocol* protocol)
{
  if (protocol->IsExperimental() && !this->SupportExperimental) {
    return;
  }
  auto version = protocol->ProtocolVersion();
  assert(version.first >= 0);
  assert(version.second >= 0);
  auto it = std::find_if(this->SupportedProtocols.begin(),
                         this->SupportedProtocols.end(),
                         [version](cmServerProtocol* p) {
                           return p->ProtocolVersion() == version;
                         });
  if (it == this->SupportedProtocols.end())
    this->SupportedProtocols.push_back(protocol);
}

void cmServer::PrintHello() const
{
  Json::Value hello = Json::objectValue;
  hello[TYPE_KEY] = "hello";

  Json::Value& protocolVersions = hello["supportedProtocolVersions"] =
    Json::arrayValue;

  for (auto const& proto : this->SupportedProtocols) {
    auto version = proto->ProtocolVersion();
    Json::Value tmp = Json::objectValue;
    tmp["major"] = version.first;
    tmp["minor"] = version.second;
    if (proto->IsExperimental()) {
      tmp["experimental"] = true;
    }
    protocolVersions.append(tmp);
  }

  this->WriteJsonObject(hello, nullptr);
}

void cmServer::QueueRequest(const std::string& request)
{
  this->Queue.push_back(request);
  this->PopOne();
}

void cmServer::reportProgress(const char* msg, float progress, void* data)
{
  const cmServerRequest* request = static_cast<const cmServerRequest*>(data);
  assert(request);
  if (progress < 0.0 || progress > 1.0) {
    request->ReportMessage(msg, "");
  } else {
    request->ReportProgress(0, static_cast<int>(progress * 1000), 1000, msg);
  }
}

void cmServer::reportMessage(const char* msg, const char* title,
                             bool& /* cancel */, void* data)
{
  const cmServerRequest* request = static_cast<const cmServerRequest*>(data);
  assert(request);
  assert(msg);
  std::string titleString;
  if (title)
    titleString = title;
  request->ReportMessage(std::string(msg), titleString);
}

cmServerResponse cmServer::SetProtocolVersion(const cmServerRequest& request)
{
  if (request.Type != "handshake")
    return request.ReportError("Waiting for type \"handshake\".");

  Json::Value requestedProtocolVersion = request.Data["protocolVersion"];
  if (requestedProtocolVersion.isNull())
    return request.ReportError(
      "\"protocolVersion\" is required for \"handshake\".");

  if (!requestedProtocolVersion.isObject())
    return request.ReportError("\"protocolVersion\" must be a JSON object.");

  Json::Value majorValue = requestedProtocolVersion["major"];
  if (!majorValue.isInt())
    return request.ReportError("\"major\" must be set and an integer.");

  Json::Value minorValue = requestedProtocolVersion["minor"];
  if (!minorValue.isNull() && !minorValue.isInt())
    return request.ReportError("\"minor\" must be unset or an integer.");

  const int major = majorValue.asInt();
  const int minor = minorValue.isNull() ? -1 : minorValue.asInt();
  if (major < 0)
    return request.ReportError("\"major\" must be >= 0.");
  if (!minorValue.isNull() && minor < 0)
    return request.ReportError("\"minor\" must be >= 0 when set.");

  this->Protocol =
    this->FindMatchingProtocol(this->SupportedProtocols, major, minor);
  if (!this->Protocol) {
    return request.ReportError("Protocol version not supported.");
  }

  std::string errorMessage;
  if (!this->Protocol->Activate(request, &errorMessage)) {
    this->Protocol = CM_NULLPTR;
    return request.ReportError("Failed to activate protocol version:" +
                               errorMessage);
  }
  return request.Reply(Json::objectValue);
}

bool cmServer::Serve(std::string* errorMessage)
{
  if (this->SupportedProtocols.empty()) {
    *errorMessage =
      "No protocol versions defined. Maybe you need --experimental?";
    return false;
  }
  assert(!this->Protocol);

  return Connection->ProcessEvents(errorMessage);
}

void cmServer::WriteJsonObject(const Json::Value& jsonValue,
                               const DebugInfo* debug) const
{
  Json::FastWriter writer;

  auto beforeJson = std::chrono::high_resolution_clock::now();
  std::string result = std::string("\n") + std::string(START_MAGIC) +
    std::string("\n") + writer.write(jsonValue) + std::string(END_MAGIC) +
    std::string("\n");

  if (debug) {
    Json::Value copy = jsonValue;
    if (debug->PrintStatistics) {
      Json::Value stats = Json::objectValue;
      auto endTime = std::chrono::high_resolution_clock::now();

      auto serializationDiff = endTime - beforeJson;
      stats["jsonSerialization"] =
        std::chrono::duration<double, std::milli>(serializationDiff).count();
      auto totalDiff = endTime - debug->StartTime;
      stats["totalTime"] =
        std::chrono::duration<double, std::milli>(totalDiff).count();
      stats["size"] = static_cast<int>(result.size());
      if (!debug->OutputFile.empty()) {
        stats["dumpFile"] = debug->OutputFile;
      }

      copy["zzzDebug"] = stats;
    }

    if (!debug->OutputFile.empty()) {
      std::ofstream myfile;
      myfile.open(debug->OutputFile);
      myfile << result;
      myfile.close();
    }
  }

  Connection->WriteData(result);
}

cmServerProtocol* cmServer::FindMatchingProtocol(
  const std::vector<cmServerProtocol*>& protocols, int major, int minor)
{
  cmServerProtocol* bestMatch = nullptr;
  for (auto protocol : protocols) {
    auto version = protocol->ProtocolVersion();
    if (major != version.first)
      continue;
    if (minor == version.second)
      return protocol;
    if (!bestMatch || bestMatch->ProtocolVersion().second < version.second)
      bestMatch = protocol;
  }
  return minor < 0 ? bestMatch : nullptr;
}

void cmServer::WriteProgress(const cmServerRequest& request, int min,
                             int current, int max,
                             const std::string& message) const
{
  assert(min <= current && current <= max);
  assert(message.length() != 0);

  Json::Value obj = Json::objectValue;
  obj[TYPE_KEY] = PROGRESS_TYPE;
  obj[REPLY_TO_KEY] = request.Type;
  obj[COOKIE_KEY] = request.Cookie;
  obj["progressMessage"] = message;
  obj["progressMinimum"] = min;
  obj["progressMaximum"] = max;
  obj["progressCurrent"] = current;

  this->WriteJsonObject(obj, nullptr);
}

void cmServer::WriteMessage(const cmServerRequest& request,
                            const std::string& message,
                            const std::string& title) const
{
  if (message.empty())
    return;

  Json::Value obj = Json::objectValue;
  obj[TYPE_KEY] = MESSAGE_TYPE;
  obj[REPLY_TO_KEY] = request.Type;
  obj[COOKIE_KEY] = request.Cookie;
  obj["message"] = message;
  if (!title.empty()) {
    obj["title"] = title;
  }

  WriteJsonObject(obj, nullptr);
}

void cmServer::WriteParseError(const std::string& message) const
{
  Json::Value obj = Json::objectValue;
  obj[TYPE_KEY] = ERROR_TYPE;
  obj[ERROR_MESSAGE_KEY] = message;
  obj[REPLY_TO_KEY] = "";
  obj[COOKIE_KEY] = "";

  this->WriteJsonObject(obj, nullptr);
}

void cmServer::WriteResponse(const cmServerResponse& response,
                             const DebugInfo* debug) const
{
  assert(response.IsComplete());

  Json::Value obj = response.Data();
  obj[COOKIE_KEY] = response.Cookie;
  obj[TYPE_KEY] = response.IsError() ? ERROR_TYPE : REPLY_TYPE;
  obj[REPLY_TO_KEY] = response.Type;
  if (response.IsError()) {
    obj[ERROR_MESSAGE_KEY] = response.ErrorMessage();
  }

  this->WriteJsonObject(obj, debug);
}
