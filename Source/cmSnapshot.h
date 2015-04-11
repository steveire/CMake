
#ifndef cmSnapshot_H
#define cmSnapshot_H

#include "cmStandardIncludes.h"

#include <memory>
#include <vector>
#include <string>
#include <iostream>

#define CM_FOR_EACH_SNAPSHOT_PROPERTY_NAME(F, OP) \
  F(Cache, OP) \
  F(GlobalProperty, OP)

#define CM_FOR_EACH_SNAPSHOT_OBJECT_PROPERTY_NAME(F, OP) \
  F(Target, OP) \
  F(Cache, OP) \
  F(Directory, OP) \
  F(SourceFile, OP)

#define COMBINE(T, SUF) T ## SUF

#define WRAP(PRE, T, SUF) PRE ## T ## SUF

#define WRAP_OP(OP, PRE, T, SUF) OP(PRE ## T ## SUF)

#define SUFFIX_KEYS(T) COMBINE(T, Keys)
#define SUFFIX_VALUES(T) COMBINE(T, Values)
#define SUFFIX_PROPERTY(T) COMBINE(T, Property)

#define CM_FOR_EACH_SNAPSHOT_PROPERTY(F) \
  CM_FOR_EACH_SNAPSHOT_PROPERTY_NAME(F, SUFFIX_KEYS) \
  CM_FOR_EACH_SNAPSHOT_PROPERTY_NAME(F, SUFFIX_VALUES)

#define CM_FOR_EACH_SNAPSHOT_PROPERTY_METHODS(F) \
  CM_FOR_EACH_SNAPSHOT_PROPERTY_NAME(F, DUMMY)

#define CM_FOR_EACH_SNAPSHOT_OBJECT_PROPERTY(F) \
  CM_FOR_EACH_SNAPSHOT_OBJECT_PROPERTY_NAME(F, SUFFIX_KEYS) \
  CM_FOR_EACH_SNAPSHOT_OBJECT_PROPERTY_NAME(F, SUFFIX_VALUES)

#define CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(F) \
  CM_FOR_EACH_SNAPSHOT_OBJECT_PROPERTY_NAME(F, DUMMY)

class cmMakefile;
class cmCacheManager;

struct cmSnapshot
{
  cmSnapshot();

  cmSnapshot(const cmSnapshot& other);

  std::string Key;

  enum SnapshotType
  {
    UndefinedType,
    SubDirectoryStart,
    SubDirectoryResult
  };
  SnapshotType Type;

  void SetDefinition(std::string const& key, std::string const& value);

#define DECLARE_PROPERTY_MUTATOR(P, OP) void COMBINE(Set, P)(std::string k, std::string v);
  CM_FOR_EACH_SNAPSHOT_PROPERTY_METHODS(DECLARE_PROPERTY_MUTATOR)
#undef DECLARE_PROPERTY_MUTATOR

#define DECLARE_OBJECT_PROPERTY_MUTATOR(P, OP) void WRAP(Set, P, Property)(std::string o, std::string k, std::string v);
      CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(DECLARE_OBJECT_PROPERTY_MUTATOR)
#undef DECLARE_OBJECT_PROPERTY_MUTATOR

#define DECLARE_PROPERTY_ACCESSOR(P, OP) std::string COMBINE(Get, P)(std::string k) const;
  CM_FOR_EACH_SNAPSHOT_PROPERTY_METHODS(DECLARE_PROPERTY_ACCESSOR)
#undef DECLARE_PROPERTY_ACCESSOR

#define DECLARE_OBJECT_PROPERTY_ACCESSOR(P, OP) std::string WRAP(Get, P, Property)(std::string o, std::string k) const;
      CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(DECLARE_OBJECT_PROPERTY_ACCESSOR)
#undef DECLARE_OBJECT_PROPERTY_ACCESSOR

#define DECLARE_PROPERTY_KEYS_ACCESSOR(P, OP) std::vector<std::string> WRAP(Get, P, Keys)() const;
  CM_FOR_EACH_SNAPSHOT_PROPERTY_METHODS(DECLARE_PROPERTY_KEYS_ACCESSOR)
#undef DECLARE_PROPERTY_KEYS_ACCESSOR

#define DECLARE_OBJECT_PROPERTY_KEYS_ACCESSOR(P, OP) std::vector<std::string> WRAP(Get, P, PropertyKeys)(std::string o) const;
      CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(DECLARE_OBJECT_PROPERTY_KEYS_ACCESSOR)
#undef DECLARE_OBJECT_PROPERTY_KEYS_ACCESSOR

#define DECLARE_OBJECT_PROPERTY_OBJECTS_ACCESSOR(P, OP) std::vector<std::string> WRAP(Get, P, Objects)() const;
      CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(DECLARE_OBJECT_PROPERTY_OBJECTS_ACCESSOR)
#undef DECLARE_OBJECT_PROPERTY_OBJECTS_ACCESSOR

//   F(CachePropertiesTypes) // ??

  static cmSnapshot& Enter(cmMakefile* mf);
  static cmSnapshot& Leave(cmMakefile *mf);
  static cmSnapshot& Create(cmCacheManager* cacheManager);

  void RemoveDefinition(std::string const& key);

private:
  std::vector<std::string> DefinitionKeys;
  std::vector<std::string> DefinitionValues;

// #if 0
#define DECLARE_PROPERTY(P, OP) std::shared_ptr<std::vector<std::string> > OP(P);
  CM_FOR_EACH_SNAPSHOT_PROPERTY(DECLARE_PROPERTY)
#undef DECLARE_PROPERTY

#define DECLARE_OBJECT_PROPERTY(P, OP) std::shared_ptr<std::vector<std::string> > COMBINE(P, Objects);
  CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(DECLARE_OBJECT_PROPERTY)
#undef DECLARE_OBJECT_PROPERTY

#define DECLARE_OBJECT_PROPERTY_KEYS(P, OP) std::vector<std::shared_ptr<std::vector<std::string> > > COMBINE(P, PropertyKeys);
  CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(DECLARE_OBJECT_PROPERTY_KEYS)
#undef DECLARE_OBJECT_PROPERTY_KEYS

#define DECLARE_OBJECT_PROPERTY_VALUES(P, OP) std::vector<std::shared_ptr<std::vector<std::string> > > COMBINE(P, PropertyValues);
  CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(DECLARE_OBJECT_PROPERTY_VALUES)
#undef DECLARE_OBJECT_PROPERTY_KEYS
// #endif

//   GNU 4.9 libstdc++ size: 328 bytes
//   libc++ size: 344 bytes
//   GNU 5.0 libstdc++ size: 352 bytes
};

#endif

