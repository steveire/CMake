/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2015 Stephen Kelly <steveire@gmail.com>

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#include "cmMetadataWriter.h"

#include "cmGlobalGenerator.h"
#include "cmLocalGenerator.h"
#include "cmMakefile.h"
#include "cmSourceFile.h"
#include "cmGeneratedFileStream.h"

#if defined(CMAKE_BUILD_WITH_CMAKE)
# include "cm_jsoncpp_value.h"
# include "cm_jsoncpp_writer.h"
#endif

// TODO:
// * Break the method below up into multiple and share where possible.
// * Change the output file to list all sources independent of targets
//   and in language-specific groups, so that consumers can access the
//   language independently of the targets.  Use non-language-specific
//   lists in the target source lists.
// * Investigate whether/how the source_group() command and FOLDER target
//   property should be exposed.


cmMetadataWriter::cmMetadataWriter(cmGlobalGenerator* gg)
  : GG(gg)
{

}

void cmMetadataWriter::Write(const char* version)
{
  cmMakefile* mf = this->GG->GetLocalGenerators()[0]->GetMakefile();

  std::string schemaVersion = version;

  std::vector<std::string> configs;
  std::string cfg = mf->GetConfigurations(configs);
  if (configs.empty())
    {
    configs.push_back(cfg);
    }
  for(std::vector<std::string>::const_iterator it = configs.begin();
      it != configs.end(); ++it)
    {
    this->Write(schemaVersion, *it);
    }
  cmake* cm = this->GG->GetCMakeInstance();
  std::string outputDir = cm->GetHomeOutputDirectory();

  std::string schemaFilePath = outputDir + "/cmake-metadata-schema.json";

  // TODO: This should be generated from a template instead of copied.
  std::string root = mf->GetRequiredDefinition("CMAKE_ROOT");
  cmSystemTools::CopyFileIfDifferent(
    (root + "/Modules/MetadataSchema-" + schemaVersion + ".json").c_str(),
    schemaFilePath.c_str()
  );
}

void cmMetadataWriter::Write(std::string version, std::string config)
{
  (void)version;
  cmake* cm = this->GG->GetCMakeInstance();
  std::string outputDir = cm->GetHomeOutputDirectory();

  Json::Value root(Json::objectValue);

  root["sourceDir"] = cm->GetHomeDirectory();
  root["generator"] = this->GG->GetName();

  cmMakefile* mf = this->GG->GetLocalGenerators()[0]->GetMakefile();
  root["platform"] = mf->GetDefinition("CMAKE_SYSTEM_NAME");

  // TODO: Does the NONE language appear here?

  std::vector<std::string> enabledLangs;
  this->GG->GetEnabledLanguages(enabledLangs);
  for(std::vector<std::string>::const_iterator i =
        enabledLangs.begin(); i != enabledLangs.end(); ++i)
    {
    const char* def = mf->GetDefinition("CMAKE_" + *i + "_COMPILER");
    if (def)
      {
      root[cmSystemTools::LowerCase(*i) + "_compiler"] = def;
      }
    def = mf->GetDefinition("CMAKE_" + *i + "_COMPILER_ID");
    if (def)
      {
      root[cmSystemTools::LowerCase(*i) + "_compiler_id"] = def;
      }
    def = mf->GetDefinition("CMAKE_" + *i + "_COMPILER_VERSION");
    if (def)
      {
      root[cmSystemTools::LowerCase(*i) + "_compiler_version"] = def;
      }
    // compile command?
    }

  std::vector<cmSourceFile const*> sourcesWithProperties;

  // Generate summary information files for each target.
  Json::Value& targets = root["targets"] = Json::objectValue;
  for(cmGlobalGenerator::TargetMap::const_iterator ti =
        this->GG->GetTotalTargets().begin(); ti != this->GG->GetTotalTargets().end(); ++ti)
    {
    if ((ti->second)->GetType() == cmTarget::INTERFACE_LIBRARY)
      {
      continue;
      }

    cmTarget* t = ti->second;
    Json::Value & targetValue = targets[ti->first] = Json::objectValue;
    targetValue["name"] = ti->first;
    targetValue["type"] = cmTarget::GetTargetTypeName(t->GetType());

    if (t->GetType() <= cmTarget::UTILITY)
      {
      Json::Value& backtrace = targetValue["backtrace"] = Json::arrayValue;

    cmListFileBacktrace const& bt = t->GetBacktrace();
    for(cmListFileBacktrace::const_iterator it = bt.begin(); it!=bt.end(); ++it)
      {
      const cmListFileContext& ctx = *it;
      Json::Value context = Json::objectValue;
      context["path"] = ctx.FilePath;
      std::stringstream line;
      line << ctx.Line;
      context["line"] = line.str();
      backtrace.append(context);
      }

      if (t->GetType() == cmTarget::UTILITY)
        {
        // Comment?
        }
      else
        {
        if (t->GetType() != cmTarget::OBJECT_LIBRARY)
          {
          std::string fp = t->GetFullPath(config, false, true);
          targetValue["target_file"] = fp;
          }
        if (t->HasImportLibrary())
          {
          targetValue["target_linker_file"] =
                  t->GetFullPath(config, true);
          }
          // PDB TODO.
        if(!t->IsDLLPlatform() && // Add platform constraint to schema. Make constraints and conditions a list.
            (t->GetType() == cmTarget::SHARED_LIBRARY))
          {
          std::string result = t->GetDirectory(config);
          result += "/";
          result += t->GetSOName(config);
          targetValue["target_soname_file"] = result; // REFACTOR, SHARE.
          }

        cmGeneratorTarget* gt = this->GG->GetGeneratorTarget(t);

        std::vector<cmSourceFile const*> files;
        gt->GetObjectSources(files, config);

        for(std::vector<cmSourceFile const*>::const_iterator it
            = files.begin(); it != files.end(); ++it)
          {
          cmSourceFile const* sf = *it;
          if (sf->GetProperty("COMPILE_FLAGS") || sf->GetProperty("COMPILE_DEFINITIONS"))
            {
            sourcesWithProperties.push_back(sf);
            }
          if (sf->GetPropertyAsBool("GENERATED"))
            {
            std::string key = "generated_object_sources_"
                              + cmSystemTools::LowerCase(sf->GetLanguage());

            if (!targetValue.isMember(key))
              {
              targetValue[key] = Json::arrayValue;
              }
            targetValue[key].append(sf->GetFullPath());
            continue;
            }
          std::string key = "object_sources_"
                            + cmSystemTools::LowerCase(sf->GetLanguage());
          if (!targetValue.isMember(key))
            {
            targetValue[key] = Json::arrayValue;
            }
          targetValue[key].append(sf->GetFullPath());
          }

        files.clear();
        gt->GetHeaderSources(files, config);

        for(std::vector<cmSourceFile const*>::const_iterator it
            = files.begin(); it != files.end(); ++it)
          {
          cmSourceFile const* sf = *it;
          if (sf->GetProperty("COMPILE_FLAGS") || sf->GetProperty("COMPILE_DEFINITIONS"))
            {
            sourcesWithProperties.push_back(sf);
            }
          if (sf->GetPropertyAsBool("GENERATED"))
            {
            std::string key = "generated_header_sources";

            if (!targetValue.isMember(key))
              {
              targetValue[key] = Json::arrayValue;
              }
            targetValue[key].append(sf->GetFullPath());
            continue;
            }
          std::string key = "header_sources";
          if (!targetValue.isMember(key))
            {
            targetValue[key] = Json::arrayValue;
            }
          targetValue[key].append(sf->GetFullPath());
          }

        files.clear();
        gt->GetExtraSources(files, config);
        // Resx? IDL ? object files specified in source list?

        for(std::vector<cmSourceFile const*>::const_iterator it
            = files.begin(); it != files.end(); ++it)
          {
          cmSourceFile const* sf = *it;
          if (sf->GetProperty("COMPILE_FLAGS") || sf->GetProperty("COMPILE_DEFINITIONS"))
            {
            sourcesWithProperties.push_back(sf);
            }
          if (sf->GetPropertyAsBool("GENERATED"))
            {
            std::string key = "generated_extra_sources";

            if (!targetValue.isMember(key))
              {
              targetValue[key] = Json::arrayValue;
              }
            targetValue[key].append(sf->GetFullPath());
            continue;
            }
          std::string key = "extra_sources";
          if (!targetValue.isMember(key))
            {
            targetValue[key] = Json::arrayValue;
            }
          targetValue[key].append(sf->GetFullPath());
          }

        files.clear();
        t->GetExcludedSourceFiles(files, config);

        for(std::vector<cmSourceFile const*>::const_iterator it
            = files.begin(); it != files.end(); ++it)
          {
          cmSourceFile const* sf = *it;
          std::string key = "excluded_sources";
          if (!targetValue.isMember(key))
            {
            targetValue[key] = Json::arrayValue;
            }
          targetValue[key].append(sf->GetFullPath());
          }

        cmLocalGenerator* lg = t->GetMakefile()->GetLocalGenerator();
        std::set<std::string> langs;
        t->GetLanguages(langs, config);
        if (langs.empty())
          {
          continue;
          }
        if (langs.size() == 1)
          {
          std::string opts;
          lg->AddCompileOptions(opts, t, *langs.begin(), config);
          if (!opts.empty())
            {
            targetValue["compile_options"] = opts;
            }

          std::set<std::string> defines;
          lg->AddCompileDefinitions(defines, t, config, *langs.begin());
          if (!defines.empty())
            {
            targetValue["compile_definitions"] = Json::arrayValue;
            for (std::set<std::string>::iterator i = defines.begin();
                 i != defines.end(); ++i)
              {
              targetValue["compile_definitions"].append(*i);
              }
            }

          std::vector<std::string> dirs;
          lg->GetIncludeDirectories(dirs, gt, *langs.begin(), config);
          if (!dirs.empty())
            {
            targetValue["include_directories"] = Json::arrayValue;
            for (std::vector<std::string>::iterator i = dirs.begin();
                 i != dirs.end(); ++i)
              {
              targetValue["include_directories"].append(*i);
              }
            }
          }
        else
          {
          bool isMultiLang = false;
          std::vector<std::string> lang_opts(langs.size());
          std::set<std::string>::const_iterator langIt = langs.begin();
          lg->AddCompileOptions(lang_opts.front(), t, *langIt, config);
          ++langIt;
          std::vector<std::string>::iterator v = lang_opts.begin() + 1;
          for( ; langIt != langs.end(); ++langIt, ++v)
            {
            lg->AddCompileOptions(*v, t, *langIt, config);
            if (*v != lang_opts.front())
              {
              isMultiLang = true;
              }
            }
          if (isMultiLang)
            {
            langIt = langs.begin();
            v = lang_opts.begin();
            for( ; langIt != langs.end(); ++langIt, ++v)
              {
              if (!v->empty())
                { // This is a string
                targetValue["compile_options_"
                                    + cmSystemTools::LowerCase(*langIt)] = *v;
                }
              }
            }
          else
            {
            if (!lang_opts.front().empty())
              {
              targetValue["compile_options"] = lang_opts.front();
              }
            }

          isMultiLang = false;
          std::vector<std::set<std::string> > lang_defs(langs.size());
          langIt = langs.begin();
          lg->AddCompileDefinitions(lang_defs.front(), t, config, *langIt);
          ++langIt;
          std::vector<std::set<std::string> >::iterator vD = lang_defs.begin() + 1;
          for( ; langIt != langs.end(); ++langIt, ++vD)
            {
            lg->AddCompileDefinitions(*vD, t, config, *langIt);
            if (*vD != lang_defs.front())
              {
              isMultiLang = true;
            std::cout << vD->size() << std::endl;
            for (std::set<std::string>::iterator i = vD->begin(); i != vD->end(); ++i)
            {
              std::cout << *i << std::endl;
            }
            std::cout << lang_defs.front().size() << std::endl;
            for (std::set<std::string>::iterator i = lang_defs.front().begin(); i != lang_defs.front().end(); ++i)
            {
              std::cout << *i << std::endl;
            }

              }
            }
          if (isMultiLang)
            {
            langIt = langs.begin();
            vD = lang_defs.begin();
            for( ; langIt != langs.end(); ++langIt, ++vD)
              {
              if (!vD->empty())
                {
                targetValue["compile_definitions_"
                      + cmSystemTools::LowerCase(*langIt)] = Json::arrayValue;
                for (std::set<std::string>::iterator i = vD->begin();
                    i != vD->end(); ++i)
                  {
                  targetValue["compile_definitions_"
                        + cmSystemTools::LowerCase(*langIt)].append(*i);
                  }
                }
              }
            }
          else
            {
            if (!lang_defs.front().empty())
              {
              targetValue["compile_definitions"] = Json::arrayValue;
              for (std::set<std::string>::iterator i = lang_defs.front().begin();
                  i != lang_defs.front().end(); ++i)
                {
                targetValue["compile_definitions"].append(*i);
                }
              }
            }

          isMultiLang = false;
          std::vector<std::vector<std::string> > lang_dirs(langs.size());
          langIt = langs.begin();
          lg->GetIncludeDirectories(lang_dirs.front(), gt, *langIt, config);
          ++langIt;
          std::vector<std::vector<std::string> >::iterator vDi = lang_dirs.begin() + 1;
          for( ; langIt != langs.end(); ++langIt, ++vDi)
            {
            lg->GetIncludeDirectories(*vDi, gt, *langIt, config);
            if (*vDi != lang_dirs.front())
              {
              isMultiLang = true;
              }
            }
          if (isMultiLang)
            {
            langIt = langs.begin();
            vDi = lang_dirs.begin();
            for( ; langIt != langs.end(); ++langIt, ++vDi)
              {
              // resolve symlinks?

                if (!vDi->empty())
                {
                targetValue["include_directories_"
                      + cmSystemTools::LowerCase(*langIt)] = Json::arrayValue;
                for (std::vector<std::string>::iterator i = vDi->begin();
                    i != vDi->end(); ++i)
                  {
                  targetValue["include_directories_"
                        + cmSystemTools::LowerCase(*langIt)].append(*i);
                  }
                }
              }
            }
          else
            {
            if (!lang_dirs.front().empty())
              {
              targetValue["include_directories"] = Json::arrayValue;
              for (std::vector<std::string>::iterator i = lang_dirs.front().begin();
                  i != lang_dirs.front().end(); ++i)
                {
                targetValue["include_directories"].append(*i);
                }
              }
            }
          }
        }
      }
    }

  if (!sourcesWithProperties.empty())
    {
    Json::Value& sources = root["sources"] = Json::objectValue;
    for(std::vector<cmSourceFile const*>::const_iterator ti =
          sourcesWithProperties.begin(); ti != sourcesWithProperties.end(); ++ti)
      {
      cmSourceFile const* sf = *ti;
      Json::Value & sourceValue = sources[sf->GetFullPath()] = Json::objectValue;
      sourceValue["path"] = sf->GetFullPath();
      if (const char* sfDef = sf->GetProperty("COMPILE_FLAGS")) // This is a string
        {
        sourceValue["compile_flags"] = sfDef;
        }
      if (const char* sfDef = sf->GetProperty("COMPILE_DEFINITIONS")) // is this escaped?
        {
        sourceValue["compile_definitions"] = Json::arrayValue;
        std::vector<std::string> defs;
        cmSystemTools::ExpandListArgument(sfDef, defs);
        for (std::vector<std::string>::iterator i = defs.begin();
            i != defs.end(); ++i)
          {
          sourceValue["compile_definitions"].append(*i);
          }
        }
      }
    }
  std::string metadataFilePath = outputDir + "/cmake-metadata";
  if (this->GG->IsMultiConfig())
    {
    metadataFilePath += "-" + std::string(config);
    }
  metadataFilePath += ".json";

  cmGeneratedFileStream aout(metadataFilePath.c_str());
  aout << root;
}
