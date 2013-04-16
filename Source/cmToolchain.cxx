
#include "cmToolchain.h"
#include "cmMakefile.h"

cmToolchain::cmToolchain(cmMakefile const *mf)
  : Makefile(mf)
{

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
  return const_cast<cmMakefile*>(this->Makefile)
                                          ->ReadListFile(listfile, external);
}
