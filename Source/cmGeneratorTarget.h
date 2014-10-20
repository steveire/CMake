/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2012 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef cmGeneratorTarget_h
#define cmGeneratorTarget_h

#include "cmStandardIncludes.h"
#include "cmGeneratorExpression.h"

class cmComputeLinkInformation;
class cmCustomCommand;
class cmGlobalGenerator;
class cmLocalGenerator;
class cmMakefile;
class cmSourceFile;
class cmTarget;

// Basic information about each link item.
class cmLinkItem: public std::string
{
  typedef std::string std_string;
public:
  cmLinkItem(): std_string(), Target(0) {}
  cmLinkItem(const std_string& n,
             cmTarget const* t): std_string(n), Target(t) {}
  cmLinkItem(cmLinkItem const& r): std_string(r), Target(r.Target) {}
  cmTarget const* Target;
};
class cmLinkImplItem: public cmLinkItem
{
public:
  cmLinkImplItem(): cmLinkItem(), Backtrace(0), FromGenex(false) {}
  cmLinkImplItem(std::string const& n,
                 cmTarget const* t,
                 cmListFileBacktrace const& bt,
                 bool fromGenex):
    cmLinkItem(n, t), Backtrace(bt), FromGenex(fromGenex) {}
  cmLinkImplItem(cmLinkImplItem const& r):
    cmLinkItem(r), Backtrace(r.Backtrace), FromGenex(r.FromGenex) {}
  cmListFileBacktrace Backtrace;
  bool FromGenex;
};

struct cmTargetLinkInformationMap:
  public std::map<std::pair<cmTarget const*, std::string>,
                           cmComputeLinkInformation*>
{
  typedef std::map<std::pair<cmTarget const*, std::string>,
                   cmComputeLinkInformation*> derived;
  cmTargetLinkInformationMap() {}
  cmTargetLinkInformationMap(cmTargetLinkInformationMap const& r);
  ~cmTargetLinkInformationMap();
};

struct TargetConfigPair : public std::pair<cmTarget const*, std::string> {
  TargetConfigPair(cmTarget const* tgt, const std::string &config)
    : std::pair<cmTarget const*, std::string>(tgt, config) {}
};

class cmGeneratorTarget
{
public:
  cmGeneratorTarget(cmTarget*);
  ~cmGeneratorTarget();

  bool IsImported() const;
  const char *GetLocation(const std::string& config) const;

  /** Get the location of the target in the build tree with a placeholder
      referencing the configuration in the native build system.  This
      location is suitable for use as the LOCATION target property.  */
  const char* GetLocationForBuild() const;

  int GetType() const;
  std::string GetName() const;
  const char *GetProperty(const std::string& prop) const;
  bool GetPropertyAsBool(const std::string& prop) const;
  void GetSourceFiles(std::vector<cmSourceFile*>& files,
                      const std::string& config) const;

  void GetObjectSources(std::vector<cmSourceFile const*> &,
                        const std::string& config) const;
  const std::string& GetObjectName(cmSourceFile const* file);

  bool HasExplicitObjectName(cmSourceFile const* file) const;
  void AddExplicitObjectName(cmSourceFile const* sf);

  void GetResxSources(std::vector<cmSourceFile const*>&,
                      const std::string& config) const;
  void GetIDLSources(std::vector<cmSourceFile const*>&,
                     const std::string& config) const;
  void GetExternalObjects(std::vector<cmSourceFile const*>&,
                          const std::string& config) const;
  void GetHeaderSources(std::vector<cmSourceFile const*>&,
                        const std::string& config) const;
  void GetExtraSources(std::vector<cmSourceFile const*>&,
                       const std::string& config) const;
  void GetCustomCommands(std::vector<cmSourceFile const*>&,
                         const std::string& config) const;
  void GetExpectedResxHeaders(std::set<std::string>&,
                              const std::string& config) const;
  void GetAppManifest(std::vector<cmSourceFile const*>&,
                      const std::string& config) const;
  void GetCertificates(std::vector<cmSourceFile const*>&,
                       const std::string& config) const;

  void ComputeObjectMapping();

  /** Get the full path to the target according to the settings in its
      makefile and the configuration type.  */
  std::string GetFullPath(const std::string& config="", bool implib = false,
                          bool realname = false) const;
  std::string NormalGetFullPath(const std::string& config, bool implib,
                                bool realname) const;
  std::string NormalGetRealName(const std::string& config) const;

  /** Get the full name of the target according to the settings in its
      makefile.  */
  std::string GetFullName(const std::string& config="",
                          bool implib = false) const;

  /** @return the Mac framework directory without the base. */
  std::string GetFrameworkDirectory(const std::string& config,
                                    bool rootDir) const;

  /** @return the Mac CFBundle directory without the base */
  std::string GetCFBundleDirectory(const std::string& config,
                                   bool contentOnly) const;

  /** @return the Mac App directory without the base */
  std::string GetAppBundleDirectory(const std::string& config,
                                    bool contentOnly) const;

  /** Return the install name directory for the target in the
    * build tree.  For example: "\@rpath/", "\@loader_path/",
    * or "/full/path/to/library".  */
  std::string GetInstallNameDirForBuildTree(const std::string& config) const;

  /** Return the install name directory for the target in the
    * install tree.  For example: "\@rpath/" or "\@loader_path/". */
  std::string GetInstallNameDirForInstallTree() const;

  /** Get the soname of the target.  Allowed only for a shared library.  */
  std::string GetSOName(const std::string& config) const;

  void GetFullNameComponents(std::string& prefix,
                             std::string& base, std::string& suffix,
                             const std::string& config="",
                             bool implib = false) const;

  /** Append to @a base the mac content directory and return it. */
  std::string BuildMacContentDirectory(const std::string& base,
                                       const std::string& config = "",
                                       bool contentOnly = true) const;

  /** @return the mac content directory for this target. */
  std::string GetMacContentDirectory(const std::string& config = 0,
                                     bool implib = false) const;

  cmTarget* Target;
  cmMakefile* Makefile;
  cmLocalGenerator* LocalGenerator;
  cmGlobalGenerator const* GlobalGenerator;

  std::string GetModuleDefinitionFile(const std::string& config) const;

  /** Link information from the transitive closure of the link
      implementation and the interfaces of its dependencies.  */
  struct LinkClosure
  {
    // The preferred linker language.
    std::string LinkerLanguage;

    // Languages whose runtime libraries must be linked.
    std::vector<std::string> Languages;
  };

  LinkClosure const* GetLinkClosure(const std::string& config) const;
  void ComputeLinkClosure(const std::string& config, LinkClosure& lc) const;

  /** Full path with trailing slash to the top-level directory
      holding object files for this target.  Includes the build
      time config name placeholder if needed for the generator.  */
  std::string ObjectDirectory;

  void UseObjectLibraries(std::vector<std::string>& objs,
                          const std::string& config) const;

  mutable cmTargetLinkInformationMap LinkInformation;

  cmComputeLinkInformation* GetLinkInformation(const std::string& config,
                                               cmTarget const* head = 0) const;

  void GetTransitiveTargetClosure(const std::string& config,
                                        cmTarget const* headTarget,
                                        std::vector<cmTarget*> &libs) const;

  bool IsLinkInterfaceDependentBoolProperty(const std::string& p,
                                            const std::string& config) const;
  bool IsLinkInterfaceDependentStringProperty(const std::string& p,
                                            const std::string& config) const;
  bool IsLinkInterfaceDependentNumberMinProperty(const std::string& p,
                                            const std::string& config) const;
  bool IsLinkInterfaceDependentNumberMaxProperty(const std::string& p,
                                            const std::string& config) const;

  bool GetLinkInterfaceDependentBoolProperty(const std::string& p,
                                            const std::string& config) const;
  const char *GetLinkInterfaceDependentStringProperty(const std::string& p,
                                            const std::string& config) const;
  const char *GetLinkInterfaceDependentNumberMinProperty(const std::string& p,
                                            const std::string& config) const;
  const char *GetLinkInterfaceDependentNumberMaxProperty(const std::string& p,
                                            const std::string& config) const;

  void CheckPropertyCompatibility(cmComputeLinkInformation *info,
                                  const std::string& config) const;

  cmMakefile* GetMakefile() const;

  /** Whether this library has \@rpath and platform supports it.  */
  bool HasMacOSXRpathInstallNameDir(const std::string& config) const;

  /** Whether this library defaults to \@rpath.  */
  bool MacOSXRpathInstallNameDirDefault() const;

  void GetAppleArchs(const std::string& config,
                     std::vector<std::string>& archVec) const;

  /** Return the rule variable used to create this type of target.  */
  std::string GetCreateRuleVariable(std::string const& lang,
                                    std::string const& config) const;

  /** Get the include directories for this target.  */
  std::vector<std::string> GetIncludeDirectories(
      const std::string& config) const;

  bool IsSystemIncludeDirectory(const std::string& dir,
                                const std::string& config) const;

  /** Add the target output files to the global generator manifest.  */
  void GenerateTargetManifest(const std::string& config) const;

  /**
   * Trace through the source files in this target and add al source files
   * that they depend on, used by all generators
   */
  void TraceDependencies();

  /** The link interface specifies transitive library dependencies and
      other information needed by targets that link to this target.  */
  struct LinkInterfaceLibraries
  {
    // Libraries listed in the interface.
    std::vector<cmLinkItem> Libraries;
  };
  struct LinkInterface: public LinkInterfaceLibraries
  {
    // Languages whose runtime libraries must be linked.
    std::vector<std::string> Languages;

    // Shared library dependencies needed for linking on some platforms.
    std::vector<cmLinkItem> SharedDeps;

    // Number of repetitions of a strongly connected component of two
    // or more static libraries.
    int Multiplicity;

    bool ImplementationIsInterface;

    LinkInterface(): Multiplicity(0), ImplementationIsInterface(false) {}
  };
  // Cache link interface computation from each configuration.
  struct OptionalLinkInterface: public LinkInterface
  {
    OptionalLinkInterface():
      LibrariesDone(false), AllDone(false),
      Exists(false), HadHeadSensitiveCondition(false),
      ExplicitLibraries(0) {}
    bool LibrariesDone;
    bool AllDone;
    bool Exists;
    bool HadHeadSensitiveCondition;
    const char* ExplicitLibraries;
  };
  struct HeadToLinkInterfaceMap:
    public std::map<cmTarget const*, OptionalLinkInterface> {};
  typedef std::map<std::string, HeadToLinkInterfaceMap>
                                                          LinkInterfaceMapType;
  mutable LinkInterfaceMapType LinkInterfaceMap;
  mutable LinkInterfaceMapType LinkInterfaceUsageRequirementsOnlyMap;
  mutable bool PolicyWarnedCMP0022;

  /** Get the link interface for the given configuration.  Returns 0
      if the target cannot be linked.  */
  LinkInterface const* GetLinkInterface(const std::string& config,
                                        cmTarget const*) const;
  LinkInterfaceLibraries const* GetLinkInterfaceLibraries(const std::string& config,
                                        cmTarget const* headTarget,
                                    bool usage_requirements_only) const;
  void ComputeLinkInterfaceLibraries(const std::string& config,
                                            OptionalLinkInterface& iface,
                                            cmTarget const* head,
                                            bool usage_requirements_only) const;
  LinkInterface const*
    GetImportLinkInterface(const std::string& config, cmTarget const* head,
                           bool usage_requirements_only) const;

  /** Get the directory in which this target will be built.  If the
      configuration name is given then the generator will add its
      subdirectory for that configuration.  Otherwise just the canonical
      output directory is given.  */
  std::string GetDirectory(const std::string& config = "",
                           bool implib = false) const;

  /** Get the directory in which to place the target compiler .pdb file.
      If the configuration name is given then the generator will add its
      subdirectory for that configuration.  Otherwise just the canonical
      compiler pdb output directory is given.  */
  std::string GetCompilePDBDirectory(const std::string& config = "") const;

  // Compute the set of languages compiled by the target.  This is
  // computed every time it is called because the languages can change
  // when source file properties are changed and we do not have enough
  // information to forward these property changes to the targets
  // until we have per-target object file properties.
  void GetLanguages(std::set<std::string>& languages,
                    std::string const& config) const;

  void ComputeLinkInterface(const std::string& config,
                            OptionalLinkInterface& iface) const;

  void ExpandLinkItems(std::string const& prop, std::string const& value,
                       std::string const& config, cmTarget const* headTarget,
                       bool usage_requirements_only,
                       std::vector<cmLinkItem>& items,
                       bool& hadHeadSensitiveCondition) const;
  void LookupLinkItems(std::vector<std::string> const& names,
                       std::vector<cmLinkItem>& items) const;

  /** Get the target major and minor version numbers interpreted from
      the VERSION property.  Version 0 is returned if the property is
      not set or cannot be parsed.  */
  void GetTargetVersion(int& major, int& minor) const;

  /** Get the target major, minor, and patch version numbers
      interpreted from the VERSION or SOVERSION property.  Version 0
      is returned if the property is not set or cannot be parsed.  */
  void
  GetTargetVersion(bool soversion, int& major, int& minor, int& patch) const;

  /** Get sources that must be built before the given source.  */
  std::vector<cmSourceFile*> const*
  GetSourceDepends(cmSourceFile const* sf) const;

  // Cache target output paths for each configuration.
  struct OutputInfo
  {
    std::string OutDir;
    std::string ImpDir;
    std::string PdbDir;
  };

  OutputInfo const* GetOutputInfo(const std::string& config) const;

  /** Get the name of the pdb file for the target.  */
  std::string GetPDBName(const std::string& config="") const;

  /** Whether this library has soname enabled and platform supports it.  */
  bool HasSOName(const std::string& config) const;

  struct CompileInfo
  {
    std::string CompilePdbDir;
  };

  CompileInfo const* GetCompileInfo(const std::string& config) const;

  typedef std::map<std::string, cmTarget::CompileInfo> CompileInfoMapType;
  CompileInfoMapType CompileInfoMap;

  /** Get the name of the compiler pdb file for the target.  */
  std::string GetCompilePDBName(const std::string& config="") const;

  /** Get the path for the MSVC /Fd option for this target.  */
  std::string GetCompilePDBPath(const std::string& config="") const;

  class TargetPropertyEntry {
    static cmLinkImplItem NoLinkImplItem;
  public:
    TargetPropertyEntry(cmsys::auto_ptr<cmCompiledGeneratorExpression> cge,
                        cmLinkImplItem const& item = NoLinkImplItem)
      : ge(cge), LinkImplItem(item)
    {}
    const cmsys::auto_ptr<cmCompiledGeneratorExpression> ge;
    cmLinkImplItem const& LinkImplItem;
  };

  /**
   * Flags for a given source file as used in this target. Typically assigned
   * via SET_TARGET_PROPERTIES when the property is a list of source files.
   */
  enum SourceFileType
  {
    SourceFileTypeNormal,
    SourceFileTypePrivateHeader, // is in "PRIVATE_HEADER" target property
    SourceFileTypePublicHeader,  // is in "PUBLIC_HEADER" target property
    SourceFileTypeResource,      // is in "RESOURCE" target property *or*
                                 // has MACOSX_PACKAGE_LOCATION=="Resources"
    SourceFileTypeMacContent     // has MACOSX_PACKAGE_LOCATION!="Resources"
  };
  struct SourceFileFlags
  {
    SourceFileFlags(): Type(SourceFileTypeNormal), MacFolder(0) {}
    SourceFileFlags(SourceFileFlags const& r):
      Type(r.Type), MacFolder(r.MacFolder) {}
    SourceFileType Type;
    const char* MacFolder; // location inside Mac content folders
  };
  void GetAutoUicOptions(std::vector<std::string> &result,
                         const std::string& config) const;

  void ReportPropertyOrigin(const std::string &p,
                            const std::string &result,
                            const std::string &report,
                            const std::string &compatibilityType) const;


  /** Get the names of the executable needed to generate a build rule
      that takes into account executable version numbers.  This should
      be called only on an executable target.  */
  void GetExecutableNames(std::string& name, std::string& realName,
                          std::string& impName, std::string& pdbName,
                          const std::string& config) const;

  /** Get the names of the library needed to generate a build rule
      that takes into account shared library version numbers.  This
      should be called only on a library target.  */
  void GetLibraryNames(std::string& name, std::string& soName,
                       std::string& realName, std::string& impName,
                       std::string& pdbName, const std::string& config) const;

  /**
   * Compute whether this target must be relinked before installing.
   */
  bool NeedRelinkBeforeInstall(const std::string& config) const;

  /** Return true if builtin chrpath will work for this target */
  bool IsChrpathUsed(const std::string& config) const;

  ///! Return the preferred linker language for this target
  std::string GetLinkerLanguage(const std::string& config = "") const;

  /** The link implementation specifies the direct library
      dependencies needed by the object files of the target.  */
  struct LinkImplementationLibraries
  {
    // Libraries linked directly in this configuration.
    std::vector<cmLinkImplItem> Libraries;
  };
  struct LinkImplementation: public LinkImplementationLibraries
  {
    // Languages whose runtime libraries must be linked.
    std::vector<std::string> Languages;
  };
  LinkImplementation const*
    GetLinkImplementation(const std::string& config) const;

  LinkImplementationLibraries const*
    GetLinkImplementationLibraries(const std::string& config) const;

  LinkImplementationLibraries const*
  GetLinkImplementationLibrariesInternal(const std::string& config,
                                                 cmTarget const* head) const;

  int ComputeLinkType(const std::string& config) const;

  cmTarget const* FindTargetToLink(std::string const& name) const;

  bool HaveBuildTreeRPATH(const std::string& config) const;
  bool HaveInstallTreeRPATH() const;

  std::string GetOutputName(const std::string& config, bool implib) const;

  struct SourceFileFlags
  GetTargetSourceFileFlags(const cmSourceFile* sf) const;

  bool IsNullImpliedByLinkLibraries(const std::string &p) const;
  struct ResxData {
    mutable std::set<std::string> ExpectedResxHeaders;
    mutable std::vector<cmSourceFile const*> ResxSources;
  };
private:
  friend class cmTargetTraceDependencies;
  struct SourceEntry { std::vector<cmSourceFile*> Depends; };
  typedef std::map<cmSourceFile const*, SourceEntry> SourceEntriesType;
  SourceEntriesType SourceEntries;
  mutable std::map<cmSourceFile const*, std::string> Objects;
  std::set<cmSourceFile const*> ExplicitObjectName;
  std::set<std::string> ExpectedResxHeaders;
  mutable std::map<std::string, std::vector<std::string> > SystemIncludesCache;

  void ConstructSourceFileFlags() const;
  mutable bool SourceFileFlagsConstructed;
  mutable std::map<cmSourceFile const*, SourceFileFlags> SourceFlagsMap;
  mutable bool DebugIncludesDone;
  mutable std::map<std::string, std::vector<TargetPropertyEntry*> >
                                CachedLinkInterfaceIncludeDirectoriesEntries;
  mutable std::map<std::string, bool> CacheLinkInterfaceIncludeDirectoriesDone;
  mutable std::map<std::string, bool> DebugCompatiblePropertiesDone;
  mutable std::set<std::string> LinkImplicitNullProperties;

  std::map<std::string, std::string> const&
  GetMaxLanguageStandards() const
  {
    return this->MaxLanguageStandards;
  }
  mutable std::map<std::string, std::string> MaxLanguageStandards;

  std::string GetFullNameInternal(const std::string& config, bool implib) const;
  void GetFullNameInternal(const std::string& config, bool implib,
                           std::string& outPrefix, std::string& outBase,
                           std::string& outSuffix) const;
  // Cache link implementation computation from each configuration.
  struct OptionalLinkImplementation: public LinkImplementation
  {
    OptionalLinkImplementation():
      LibrariesDone(false), LanguagesDone(false),
      HadHeadSensitiveCondition(false) {}
    bool LibrariesDone;
    bool LanguagesDone;
    bool HadHeadSensitiveCondition;
  };
  void ComputeLinkImplementationLibraries(const std::string& config,
                                          OptionalLinkImplementation& impl,
                                          cmTarget const* head) const;
  void ComputeLinkImplementationLanguages(const std::string& config,
                                          OptionalLinkImplementation& impl
                                          ) const;
  struct HeadToLinkImplementationMap:
    public std::map<cmTarget const*, OptionalLinkImplementation> {};
  typedef std::map<std::string,
                   HeadToLinkImplementationMap> LinkImplMapType;
  mutable LinkImplMapType LinkImplMap;
  void ComputeVersionedName(std::string& vName,
                            std::string const& prefix,
                            std::string const& base,
                            std::string const& suffix,
                            std::string const& name,
                            const char* version) const;

  typedef std::map<std::string, LinkClosure> LinkClosureMapType;
  mutable LinkClosureMapType LinkClosureMap;

  cmGeneratorTarget(cmGeneratorTarget const&);
  void operator=(cmGeneratorTarget const&);

  struct CompatibleInterfaces
  {
    std::set<std::string> PropsBool;
    std::set<std::string> PropsString;
    std::set<std::string> PropsNumberMax;
    std::set<std::string> PropsNumberMin;
  };
  CompatibleInterfaces const&
    GetCompatibleInterfaces(std::string const& config) const;

  struct CompatibleInterfacesIntl: CompatibleInterfaces
  {
    CompatibleInterfacesIntl(): Done(false) {}
    bool Done;
  };
  mutable std::map<std::string, CompatibleInterfacesIntl> CompatibleInterfacesMap;

  struct LinkImplClosure: public std::vector<cmTarget const*>
  {
    LinkImplClosure(): Done(false) {}
    bool Done;
  };
  mutable std::map<std::string, LinkImplClosure> LinkImplClosureMap;

  bool IsApple;

public:
  std::vector<cmTarget const*> const&
    GetLinkImplementationClosure(const std::string& config) const;

};

struct cmStrictTargetComparison {
  bool operator()(cmTarget const* t1, cmTarget const* t2) const;
};

typedef std::map<cmTarget const*,
                 cmGeneratorTarget*,
                 cmStrictTargetComparison> cmGeneratorTargetsType;

#endif
