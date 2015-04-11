
#include "cmSnapshot.h"
#include "cmMakefile.h"
#include "cmLocalGenerator.h"
#include "cmCacheManager.h"
#include "cmake.h"

#include <assert.h>

cmSnapshot& cmSnapshot::Leave(cmMakefile *mf)
{
  cmake* cm = mf->GetCMakeInstance();
  --cm->SnapshotPosition;
  --cm->SnapshotPosition;
  assert(cm->SnapshotPosition >= 0 && cm->SnapshotPosition < cm->Snapshots.size());
  return cm->Snapshots[cm->SnapshotPosition];
}

cmSnapshot& cmSnapshot::Enter(cmMakefile *mf)
{
  cmake* cm = mf->GetCMakeInstance();

  assert(!cm->Snapshots.empty());
  cmSnapshot& snp = *cm->Snapshots.insert(cm->Snapshots.begin() + cm->SnapshotPosition + 1, cm->Snapshots.at(cm->SnapshotPosition));
  snp.Key = mf->GetCurrentDirectory() + std::string("/CMakeLists.txt");
  snp.Type = cmSnapshot::SubDirectoryStart;
  ++cm->SnapshotPosition;
  cmSnapshot& snp2 = *cm->Snapshots.insert(cm->Snapshots.begin() + cm->SnapshotPosition + 1, cm->Snapshots.at(cm->SnapshotPosition));
  snp2.Type = cmSnapshot::SubDirectoryResult;
  ++cm->SnapshotPosition;
  return snp2;
}

cmSnapshot& cmSnapshot::Create(cmCacheManager *cacheManager)
{
  cmake* cm = cacheManager->GetCMakeInstance();
  assert(cm->Snapshots.empty());
  cm->Snapshots.push_back(cmSnapshot());
  cmSnapshot& snp = cm->Snapshots.back();
  snp.Key = "CMakeCache.txt";

  return snp;
}


void cmSnapshot::SetDefinition(std::string const& key, std::string const& value)
{
  this->DefinitionKeys.push_back(key);
  this->DefinitionValues.push_back(key);
}

cmSnapshot::cmSnapshot()
  :
#define INIT_PROPERTY(P, OP) OP(P)(std::make_shared<std::vector<std::string> >()),
    CM_FOR_EACH_SNAPSHOT_PROPERTY(INIT_PROPERTY)
#undef INIT_PROPERTY
#define INIT_OBJECT_PROPERTY(P, OP) COMBINE(P, Objects)(std::make_shared<std::vector<std::string> >()),
    CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(INIT_OBJECT_PROPERTY)
#undef INIT_OBJECT_PROPERTY
    Type(UndefinedType)
{

}

cmSnapshot::cmSnapshot(const cmSnapshot& other)
  :
#define COPY_PROPERTY(P, OP) OP(P)(std::make_shared<std::vector<std::string> >(*other.OP(P))),
    CM_FOR_EACH_SNAPSHOT_PROPERTY(COPY_PROPERTY)
#undef COPY_PROPERTY
#define INIT_OBJECT_PROPERTY(P, OP) COMBINE(P, Objects)(std::make_shared<std::vector<std::string> >(*other.COMBINE(P, Objects))),
    CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(INIT_OBJECT_PROPERTY)
#undef INIT_OBJECT_PROPERTY
    Key(other.Key),
    Type(other.Type)
{
#define COPY_OBJECT_PROPERTY_KEYS(P, OP) COMBINE(P, PropertyKeys).reserve(other.COMBINE(P, PropertyKeys).size()); \
  for (std::vector<std::shared_ptr<std::vector<std::string> > >::const_iterator it = other.COMBINE(P, PropertyKeys).begin(); \
      it != other.COMBINE(P, PropertyKeys).end(); ++it) { \
    this->COMBINE(P, PropertyKeys).push_back(std::make_shared<std::vector<std::string> >(**it)); \
  }
  CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(COPY_OBJECT_PROPERTY_KEYS)
#undef COPY_OBJECT_PROPERTY_KEYS

#define COPY_OBJECT_PROPERTY_VALUES(P, OP) COMBINE(P, PropertyValues).reserve(other.COMBINE(P, PropertyValues).size()); \
  for (std::vector<std::shared_ptr<std::vector<std::string> > >::const_iterator it = other.COMBINE(P, PropertyValues).begin(); \
      it != other.COMBINE(P, PropertyValues).end(); ++it) { \
    this->COMBINE(P, PropertyValues).push_back(std::make_shared<std::vector<std::string> >(**it)); \
  }
  CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(COPY_OBJECT_PROPERTY_VALUES)
#undef COPY_OBJECT_PROPERTY_VALUES
}

static inline void mutate(std::shared_ptr<std::vector<std::string> >& keys,
                          std::shared_ptr<std::vector<std::string> >& values,
                          std::string k, std::string v)
{
  std::vector<std::string>::iterator keyIt =
    std::lower_bound(keys->begin(), keys->end(), k);
  size_t dist = std::distance(keys->begin(), keyIt);

  std::vector<std::string>::iterator valueIt = (values->begin() + dist);

  if (keyIt != keys->end() && *keyIt == k)
    {
    *valueIt = v;
    }
  else
    {
    keys->insert(keyIt, k);
    values->insert(valueIt, v);
    }
}

#define DEFINE_PROPERTY_MUTATOR(P, OP) \
  void cmSnapshot::COMBINE(Set, P)(std::string k, std::string v) \
  { \
    mutate(this->COMBINE(P, Keys), this->COMBINE(P, Values), k, v); \
  }
  CM_FOR_EACH_SNAPSHOT_PROPERTY_METHODS(DEFINE_PROPERTY_MUTATOR)
#undef DEFINE_PROPERTY_MUTATOR

void cmSnapshot::RemoveDefinition(std::string const& key)
{
  std::vector<std::string>::iterator keyIt =
    std::lower_bound(DefinitionKeys.begin(), DefinitionKeys.end(), key);

  if (keyIt != DefinitionKeys.end() && *keyIt == key)
    {
    size_t dist = std::distance(DefinitionKeys.begin(), keyIt);
    DefinitionKeys.erase(keyIt);
    DefinitionValues.erase(DefinitionValues.begin() + dist);
    }
}

static inline void mutate(std::vector<std::string>& objects,
                          std::vector<std::shared_ptr<std::vector<std::string> > >& keys,
                          std::vector<std::shared_ptr<std::vector<std::string> > >& values,
                          std::string o, std::string k, std::string v)
{
  std::vector<std::string>::iterator objIt =
    std::lower_bound(objects.begin(), objects.end(), o);
  size_t dist = std::distance(objects.begin(), objIt);

  std::vector<std::shared_ptr<std::vector<std::string> > >::iterator keyIt = (keys.begin() + dist);
  std::vector<std::shared_ptr<std::vector<std::string> > >::iterator valueIt = (values.begin() + dist);
  if (objIt == objects.end() || *objIt != o)
    {
    objects.insert(objIt, o);
    keyIt = keys.insert(keyIt, std::make_shared<std::vector<std::string> >(std::vector<std::string>()));
    valueIt = values.insert(valueIt, std::make_shared<std::vector<std::string> >(std::vector<std::string>()));
    }
  mutate(*keyIt, *valueIt, k, v);
}

#define DEFINE_OBJECT_PROPERTY_MUTATOR(P, OP) \
  void cmSnapshot::WRAP(Set, P, Property)(std::string o, std::string k, std::string v) \
  { \
    mutate(*this->COMBINE(P, Objects), COMBINE(P, PropertyKeys), COMBINE(P, PropertyValues), o, k, v); \
  }
  CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(DEFINE_OBJECT_PROPERTY_MUTATOR)
#undef DEFINE_OBJECT_PROPERTY_MUTATOR

static inline std::string access(std::shared_ptr<std::vector<std::string> > const& keys,
                          std::shared_ptr<std::vector<std::string> >const& values,
                          std::string k)
{
  std::vector<std::string>::iterator keyIt =
    std::lower_bound(keys->begin(), keys->end(), k);
  if (keyIt == keys->end() || *keyIt != k)
    {
    return std::string();
    }
  size_t dist = std::distance(keys->begin(), keyIt);
  std::vector<std::string>::iterator valueIt = (values->begin() + dist);
  return *valueIt;
}

#define DEFINE_PROPERTY_ACCESSOR(P, OP) \
  std::string cmSnapshot::COMBINE(Get, P)(std::string k) const \
  { \
    return access(this->COMBINE(P, Keys), this->COMBINE(P, Values), k); \
  }
  CM_FOR_EACH_SNAPSHOT_PROPERTY_METHODS(DEFINE_PROPERTY_ACCESSOR)
#undef DEFINE_PROPERTY_ACCESSOR

static inline std::string access(std::vector<std::string> const& objects,
                                 std::vector<std::shared_ptr<std::vector<std::string> > > const& keys,
                                 std::vector<std::shared_ptr<std::vector<std::string> > > const& values,
                                 std::string o, std::string k)
{
  std::vector<std::string>::const_iterator objIt =
    std::lower_bound(objects.begin(), objects.end(), o);
  if (objIt == objects.end() || *objIt != o)
    {
    return std::string();
    }
  size_t dist = std::distance(objects.begin(), objIt);

  std::vector<std::shared_ptr<std::vector<std::string> > >::const_iterator keyIt = (keys.begin() + dist);
  assert(keyIt != keys.end());
  std::vector<std::shared_ptr<std::vector<std::string> > >::const_iterator valueIt = (values.begin() + dist);
  assert(valueIt != values.end());
  return access(*keyIt, *valueIt, k);
}

#define DEFINE_OBJECT_PROPERTY_ACCESSOR(P, OP) \
  std::string cmSnapshot::WRAP(Get, P, Property)(std::string o, std::string k) const \
  { \
    return access(*this->COMBINE(P, Objects), this->COMBINE(P, PropertyKeys), this->COMBINE(P, PropertyValues), o, k); \
  }
  CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(DEFINE_OBJECT_PROPERTY_ACCESSOR)
#undef DEFINE_OBJECT_PROPERTY_ACCESSOR

#define DEFINE_PROPERTY_KEYS_ACCESSOR(P, OP) \
  std::vector<std::string> cmSnapshot::WRAP(Get, P, Keys)() const \
  { \
    return *this->COMBINE(P, Keys); \
  }
  CM_FOR_EACH_SNAPSHOT_PROPERTY_METHODS(DEFINE_PROPERTY_KEYS_ACCESSOR)
#undef DEFINE_PROPERTY_KEYS_ACCESSOR

#define DEFINE_OBJECT_PROPERTY_KEYS_ACCESSOR(P, OP) \
  std::vector<std::string> cmSnapshot::WRAP(Get, P, PropertyKeys)(std::string o) const \
  { \
    std::vector<std::string>::iterator objIt = \
      std::lower_bound(COMBINE(P, Objects)->begin(), COMBINE(P, Objects)->end(), o); \
    assert(objIt != COMBINE(P, Objects)->end()); \
    size_t dist = std::distance(COMBINE(P, Objects)->begin(), objIt); \
    return *(*(this->COMBINE(P, PropertyKeys).begin() + dist)); \
  }
  CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(DEFINE_OBJECT_PROPERTY_KEYS_ACCESSOR)
#undef DEFINE_OBJECT_PROPERTY_KEYS_ACCESSOR

#define DEFINE_OBJECT_PROPERTY_OBJECTS_ACCESSOR(P, OP) \
  std::vector<std::string> cmSnapshot::WRAP(Get, P, Objects)() const \
  { \
    return *this->COMBINE(P, Objects); \
  }
  CM_FOR_EACH_SNAPSHOT_OBJECT_METHOD(DEFINE_OBJECT_PROPERTY_OBJECTS_ACCESSOR)
#undef DEFINE_OBJECT_PROPERTY_OBJECTS_ACCESSOR
