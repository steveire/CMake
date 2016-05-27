/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2016 Tobias Hunger <tobias.hunger@qt.io>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#include "cmServerProtocol.h"

#include "cmExternalMakefileProjectGenerator.h"
#include "cmGlobalGenerator.h"
#include "cmServer.h"
#include "cmake.h"

#if defined(CMAKE_BUILD_WITH_CMAKE)
#include "cm_jsoncpp_reader.h"
#include "cm_jsoncpp_value.h"
#endif

namespace {
// Vocabulary:

char GLOBAL_SETTINGS_TYPE[] = "globalSettings";
char SET_GLOBAL_SETTINGS_TYPE[] = "setGlobalSettings";

char BUILD_DIRECTORY_KEY[] = "buildDirectory";
char CAPABILITIES_KEY[] = "capabilities";
char CHECK_SYSTEM_VARS_KEY[] = "checkSystemVars";
char COOKIE_KEY[] = "cookie";
char DEBUG_OUTPUT_KEY[] = "debugOutput";
char EXTRA_GENERATOR_KEY[] = "extraGenerator";
char GENERATOR_KEY[] = "generator";
char SOURCE_DIRECTORY_KEY[] = "sourceDirectory";
char TRACE_EXPAND_KEY[] = "traceExpand";
char TRACE_KEY[] = "trace";
char TYPE_KEY[] = "type";
char WARN_UNINITIALIZED_KEY[] = "warnUninitialized";
char WARN_UNUSED_CLI_KEY[] = "warnUnusedCli";
char WARN_UNUSED_KEY[] = "warnUnused";

} // namespace

cmServerRequest::cmServerRequest(cmServer* server, const std::string& t,
                                 const std::string& c, const Json::Value& d)
  : Type(t)
  , Cookie(c)
  , Data(d)
  , m_Server(server)
{
}

void cmServerRequest::ReportProgress(int min, int current, int max,
                                     const std::string& message) const
{
  this->m_Server->WriteProgress(*this, min, current, max, message);
}

void cmServerRequest::ReportMessage(const std::string& message,
                                    const std::string& title) const
{
  m_Server->WriteMessage(*this, message, title);
}

cmServerResponse cmServerRequest::Reply(const Json::Value& data) const
{
  cmServerResponse response(*this);
  response.SetData(data);
  return response;
}

cmServerResponse cmServerRequest::ReportError(const std::string& message) const
{
  cmServerResponse response(*this);
  response.SetError(message);
  return response;
}

cmServerResponse::cmServerResponse(const cmServerRequest& request)
  : Type(request.Type)
  , Cookie(request.Cookie)
{
}

void cmServerResponse::SetData(const Json::Value& data)
{
  assert(this->m_Payload == UNKNOWN);
  if (!data[COOKIE_KEY].isNull() || !data[TYPE_KEY].isNull()) {
    this->SetError("Response contains cookie or type field.");
    return;
  }
  this->m_Payload = DATA;
  this->m_Data = data;
}

void cmServerResponse::SetError(const std::string& message)
{
  assert(this->m_Payload == UNKNOWN);
  this->m_Payload = ERROR;
  this->m_ErrorMessage = message;
}

bool cmServerResponse::IsComplete() const
{
  return this->m_Payload != UNKNOWN;
}

bool cmServerResponse::IsError() const
{
  assert(this->m_Payload != UNKNOWN);
  return this->m_Payload == ERROR;
}

std::string cmServerResponse::ErrorMessage() const
{
  if (this->m_Payload == ERROR)
    return this->m_ErrorMessage;
  else
    return std::string();
}

Json::Value cmServerResponse::Data() const
{
  assert(this->m_Payload != UNKNOWN);
  return this->m_Data;
}

cmServerProtocol::~cmServerProtocol() = default;

bool cmServerProtocol::Activate(const cmServerRequest& request,
                                std::string* errorMessage)
{
  this->m_CMakeInstance = std::make_unique<cmake>();
  const bool result = this->DoActivate(request, errorMessage);
  if (!result)
    this->m_CMakeInstance = CM_NULLPTR;
  return result;
}

cmake* cmServerProtocol::CMakeInstance() const
{
  return this->m_CMakeInstance.get();
}

bool cmServerProtocol::DoActivate(const cmServerRequest& /*request*/,
                                  std::string* /*errorMessage*/)
{
  return true;
}

std::pair<int, int> cmServerProtocol1_0::ProtocolVersion() const
{
  return std::make_pair(1, 0);
}

bool cmServerProtocol1_0::DoActivate(const cmServerRequest& request,
                                     std::string* errorMessage)
{
  const std::string sourceDirectory =
    request.Data[SOURCE_DIRECTORY_KEY].asString();
  const std::string buildDirectory =
    request.Data[BUILD_DIRECTORY_KEY].asString();
  const std::string generator = request.Data[GENERATOR_KEY].asString();
  const std::string extraGenerator =
    request.Data[EXTRA_GENERATOR_KEY].asString();

  const std::string fullGeneratorName =
    cmExternalMakefileProjectGenerator::CreateFullGeneratorName(
      generator, extraGenerator);

  if (sourceDirectory.empty() || buildDirectory.empty() || generator.empty()) {
    if (errorMessage) {
      *errorMessage = "Mandatory field missing (\"sourceDirectory\", "
                      "\"buildDirectory\" or \"generator\")";
    }
    return false;
  }

  cmake* cm = this->CMakeInstance();

  cmGlobalGenerator* gg = cm->CreateGlobalGenerator(fullGeneratorName);
  if (!gg) {
    if (errorMessage)
      *errorMessage = "Could not set up the requested combination of "
                      "\"generator\" and \"extraGenerator\"";
    return false;
  }

  cm->SetGlobalGenerator(gg);
  cm->SetHomeDirectory(sourceDirectory);
  cm->SetHomeOutputDirectory(buildDirectory);

  this->m_State = ACTIVE;
  return true;
}

const cmServerResponse cmServerProtocol1_0::Process(
  const cmServerRequest& request)
{
  assert(this->m_State >= ACTIVE);

  if (request.Type == GLOBAL_SETTINGS_TYPE)
    return this->ProcessGlobalSettings(request);
  if (request.Type == SET_GLOBAL_SETTINGS_TYPE)
    return this->ProcessSetGlobalSettings(request);

  return request.ReportError("Unknown command!");
}

bool cmServerProtocol1_0::IsExperimental() const
{
  return true;
}

cmServerResponse cmServerProtocol1_0::ProcessGlobalSettings(
  const cmServerRequest& request)
{
  cmake* cm = this->CMakeInstance();
  Json::Value obj = Json::objectValue;

  // Capabilities information:
  obj[CAPABILITIES_KEY] = cm->ReportCapabilitiesJson();

  obj[DEBUG_OUTPUT_KEY] = cm->GetDebugOutput();
  obj[TRACE_KEY] = cm->GetTrace();
  obj[TRACE_EXPAND_KEY] = cm->GetTraceExpand();
  obj[WARN_UNINITIALIZED_KEY] = cm->GetWarnUninitialized();
  obj[WARN_UNUSED_KEY] = cm->GetWarnUnused();
  obj[WARN_UNUSED_CLI_KEY] = cm->GetWarnUnusedCli();
  obj[CHECK_SYSTEM_VARS_KEY] = cm->GetCheckSystemVars();

  obj[SOURCE_DIRECTORY_KEY] = cm->GetHomeDirectory();
  obj[BUILD_DIRECTORY_KEY] = cm->GetHomeOutputDirectory();

  // Currently used generator:
  cmGlobalGenerator* gen = cm->GetGlobalGenerator();
  obj[GENERATOR_KEY] = gen ? gen->GetName() : std::string();
  obj[EXTRA_GENERATOR_KEY] =
    gen ? gen->GetExtraGeneratorName() : std::string();

  return request.Reply(obj);
}

static void setBool(const cmServerRequest& request, const std::string& key,
                    std::function<void(bool)> setter)
{
  if (request.Data[key].isNull())
    return;
  setter(request.Data[key].asBool());
}

cmServerResponse cmServerProtocol1_0::ProcessSetGlobalSettings(
  const cmServerRequest& request)
{
  const std::vector<std::string> boolValues = {
    DEBUG_OUTPUT_KEY,       TRACE_KEY,       TRACE_EXPAND_KEY,
    WARN_UNINITIALIZED_KEY, WARN_UNUSED_KEY, WARN_UNUSED_CLI_KEY,
    CHECK_SYSTEM_VARS_KEY
  };
  for (auto i : boolValues) {
    if (!request.Data[i].isNull() && !request.Data[i].isBool()) {
      return request.ReportError("\"" + i +
                                 "\" must be unset or a bool value.");
    }
  }

  cmake* cm = this->CMakeInstance();

  setBool(request, DEBUG_OUTPUT_KEY,
          [cm](bool e) { cm->SetDebugOutputOn(e); });
  setBool(request, TRACE_KEY, [cm](bool e) { cm->SetTrace(e); });
  setBool(request, TRACE_EXPAND_KEY, [cm](bool e) { cm->SetTraceExpand(e); });
  setBool(request, WARN_UNINITIALIZED_KEY,
          [cm](bool e) { cm->SetWarnUninitialized(e); });
  setBool(request, WARN_UNUSED_KEY, [cm](bool e) { cm->SetWarnUnused(e); });
  setBool(request, WARN_UNUSED_CLI_KEY,
          [cm](bool e) { cm->SetWarnUnusedCli(e); });
  setBool(request, CHECK_SYSTEM_VARS_KEY,
          [cm](bool e) { cm->SetCheckSystemVars(e); });

  return request.Reply(Json::Value());
}
