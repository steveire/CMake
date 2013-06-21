
#include "cmToolchain.h"
#include "cmMakefile.h"
#include "cmLocalGenerator.h"
#include "cmGlobalGenerator.h"

#include "cmSourceFile.h"

cmToolchain::cmToolchain(cmMakefile const *mf_)
  : Makefile(mf_)
{
  cmMakefile *mf = const_cast<cmMakefile*>(mf_);
  cmake *cm =
            mf->GetLocalGenerator()->GetGlobalGenerator()->GetCMakeInstance();
  this->CacheManager = new cmCacheManager(cm);
}

const char *cmToolchain::GetDefinition(const char *input) const
{
  return this->Makefile->GetDefinitionImpl(input);
}

const char *cmToolchain::GetSafeDefinition(const char *input) const
{
  return this->Makefile->GetSafeDefinition(input);
}

const char* cmToolchain::GetRequiredDefinition(const char* name) const
{
  return this->Makefile->GetRequiredDefinition(name);
}

bool cmToolchain::IsOn(const char* name) const
{
  return this->Makefile->IsOn(name);
}

bool cmToolchain::IsSet(const char* name) const
{
  return this->Makefile->IsSet(name);
}

bool cmToolchain::ReadListFile(const char* listfile,
                  const char* external)
{
  cmMakefile *mf = const_cast<cmMakefile*>(this->Makefile);
  cmGlobalGenerator* gg = mf->GetLocalGenerator()->GetGlobalGenerator();
  cmake *cm = gg->GetCMakeInstance();
  if (cmSystemTools::FileExists((cm->GetHomeOutputDirectory()
                        + std::string("/toolchain/CMakeCache.txt")).c_str()))
    {
    this->CacheManager->LoadCache((cm->GetHomeOutputDirectory()
                                      + std::string("/toolchain/")).c_str());
    }
  else
    {
    cmCacheManager::CacheIterator it =
                                    cm->GetCacheManager()->GetCacheIterator();
    for ( it.Begin(); !it.IsAtEnd(); it.Next() )
      {
      this->CacheManager->AddCacheEntry(it.GetName(),
                                        it.GetValue(),
                                        it.GetProperty("HELPSTRING"),
                                        it.GetType());
      }
    }
  mf->SetCacheManager(this->CacheManager);
  bool result = mf->ReadListFile(listfile, external);
  this->CacheManager->SaveCache((cm->GetHomeOutputDirectory()
                                      + std::string("/toolchain/")).c_str());
  return result;
}

bool cmToolchain::GetLanguageEnabled(const char* l) const
{
  return (this->LanguageEnabled.find(l)!= this->LanguageEnabled.end());
}

void cmToolchain::ClearEnabledLanguages()
{
  this->LanguageEnabled.clear();
}

void cmToolchain::GetEnabledLanguages(std::vector<std::string>& lang)
{
  for(std::map<cmStdString, bool>::iterator i =
        this->LanguageEnabled.begin(); i != this->LanguageEnabled.end(); ++i)
    {
    lang.push_back(i->first);
    }
}

bool cmToolchain::IgnoreFile(const char* l)
{
  if(this->GetLanguageFromExtension(l))
    {
    return false;
    }
  return (this->IgnoreExtensions.count(l) > 0);
}

void cmToolchain::FillExtensionToLanguageMap(const char* l,
                                                   cmMakefile* mf)
{
  std::string extensionsVar = std::string("CMAKE_") +
    std::string(l) + std::string("_SOURCE_FILE_EXTENSIONS");
  std::string exts = mf->GetSafeDefinition(extensionsVar.c_str());
  std::vector<std::string> extensionList;
  cmSystemTools::ExpandListArgument(exts, extensionList);
  for(std::vector<std::string>::iterator i = extensionList.begin();
      i != extensionList.end(); ++i)
    {
    this->ExtensionToLanguage[*i] = l;
    }
}

//----------------------------------------------------------------------------
const char*
cmToolchain::GetLanguageOutputExtension(cmSourceFile const& source)
{
  if(const char* lang = source.GetLanguage())
    {
    if(this->LanguageToOutputExtension.count(lang) > 0)
      {
      return this->LanguageToOutputExtension[lang].c_str();
      }
    }
  else
    {
    // if no language is found then check to see if it is already an
    // ouput extension for some language.  In that case it should be ignored
    // and in this map, so it will not be compiled but will just be used.
    std::string const& ext = source.GetExtension();
    if(!ext.empty())
      {
      if(this->OutputExtensions.count(ext))
        {
        return ext.c_str();
        }
      }
    }
  return "";
}


const char* cmToolchain::GetLanguageFromExtension(const char* ext)
{
  // if there is an extension and it starts with . then move past the
  // . because the extensions are not stored with a .  in the map
  if(ext && *ext == '.')
    {
    ++ext;
    }
  if(this->ExtensionToLanguage.count(ext) > 0)
    {
    return this->ExtensionToLanguage[ext].c_str();
    }
  return 0;
}

/* SetLanguageEnabled() is now split in two parts:
at first the enabled-flag is set. This can then be used in EnabledLanguage()
for checking whether the language is already enabled. After setting this
flag still the values from the cmake variables have to be copied into the
internal maps, this is done in SetLanguageEnabledMaps() which is called
after the system- and compiler specific files have been loaded.

This split was done originally so that compiler-specific configuration
files could change the object file extension
(CMAKE_<LANG>_OUTPUT_EXTENSION) before the CMake variables were copied
to the C++ maps.
*/
void cmToolchain::SetLanguageEnabled(const char* l, cmMakefile* mf)
{
  this->SetLanguageEnabledFlag(l, mf);
  this->SetLanguageEnabledMaps(l, mf);
}

void cmToolchain::SetLanguageEnabledFlag(const char* l, cmMakefile* mf)
{
  this->LanguageEnabled[l] = true;

  // Fill the language-to-extension map with the current variable
  // settings to make sure it is available for the try_compile()
  // command source file signature.  In SetLanguageEnabledMaps this
  // will be done again to account for any compiler- or
  // platform-specific entries.
  this->FillExtensionToLanguageMap(l, mf);
}

void cmToolchain::SetLanguageEnabledMaps(const char* l, cmMakefile* mf)
{
  // use LanguageToLinkerPreference to detect whether this functions has
  // run before
  if (this->LanguageToLinkerPreference.find(l) !=
                                        this->LanguageToLinkerPreference.end())
    {
    return;
    }

  std::string linkerPrefVar = std::string("CMAKE_") +
    std::string(l) + std::string("_LINKER_PREFERENCE");
  const char* linkerPref = mf->GetDefinition(linkerPrefVar.c_str());
  int preference = 0;
  if(linkerPref)
    {
    if (sscanf(linkerPref, "%d", &preference)!=1)
      {
      // backward compatibility: before 2.6 LINKER_PREFERENCE
      // was either "None" or "Prefered", and only the first character was
      // tested. So if there is a custom language out there and it is
      // "Prefered", set its preference high
      if (linkerPref[0]=='P')
        {
        preference = 100;
        }
      else
        {
        preference = 0;
        }
      }
    }

  if (preference < 0)
    {
    std::string msg = linkerPrefVar;
    msg += " is negative, adjusting it to 0";
    cmSystemTools::Message(msg.c_str(), "Warning");
    preference = 0;
    }

  this->LanguageToLinkerPreference[l] = preference;

  std::string outputExtensionVar = std::string("CMAKE_") +
    std::string(l) + std::string("_OUTPUT_EXTENSION");
  const char* outputExtension = mf->GetDefinition(outputExtensionVar.c_str());
  if(outputExtension)
    {
    this->LanguageToOutputExtension[l] = outputExtension;
    this->OutputExtensions[outputExtension] = outputExtension;
    if(outputExtension[0] == '.')
      {
      this->OutputExtensions[outputExtension+1] = outputExtension+1;
      }
    }

  // The map was originally filled by SetLanguageEnabledFlag, but
  // since then the compiler- and platform-specific files have been
  // loaded which might have added more entries.
  this->FillExtensionToLanguageMap(l, mf);

  std::string ignoreExtensionsVar = std::string("CMAKE_") +
    std::string(l) + std::string("_IGNORE_EXTENSIONS");
  std::string ignoreExts = mf->GetSafeDefinition(ignoreExtensionsVar.c_str());
  std::vector<std::string> extensionList;
  cmSystemTools::ExpandListArgument(ignoreExts, extensionList);
  for(std::vector<std::string>::iterator i = extensionList.begin();
      i != extensionList.end(); ++i)
    {
    this->IgnoreExtensions[*i] = true;
    }

}

int cmToolchain::GetLinkerPreference(const char* lang)
{
  std::map<cmStdString, int>::const_iterator it =
                                   this->LanguageToLinkerPreference.find(lang);
  if (it != this->LanguageToLinkerPreference.end())
    {
    return it->second;
    }
  return 0;
}

//----------------------------------------------------------------------------
std::string cmToolchain::GetSharedLibFlagsForLanguage(
                                                        std::string const& l)
{
  if(this->LanguageToOriginalSharedLibFlags.count(l) > 0)
    {
    return this->LanguageToOriginalSharedLibFlags[l];
    }
  return "";
}

void cmToolchain::EnableLanguagesFromToolchain(cmToolchain *tch)
{
  // copy the enabled languages
  this->LanguageEnabled = tch->LanguageEnabled;
  this->LanguagesReady = tch->LanguagesReady;
  this->ExtensionToLanguage = tch->ExtensionToLanguage;
  this->IgnoreExtensions = tch->IgnoreExtensions;
  this->LanguageToOutputExtension = tch->LanguageToOutputExtension;
  this->LanguageToLinkerPreference = tch->LanguageToLinkerPreference;
  this->OutputExtensions = tch->OutputExtensions;
}
