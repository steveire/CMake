
#ifndef cmToolchain_h
#define cmToolchain_h

class cmMakefile;

class cmToolchain
{
public:
  cmToolchain(cmMakefile const *mf);

  const char *GetDefinition(const char *) const;
  const char *GetSafeDefinition(const char *) const;
  const char* GetRequiredDefinition(const char* name) const;

  bool IsOn(const char* name) const;
  bool IsSet(const char* name) const;

  bool ReadListFile(const char* listfile,
                    const char* external= 0);

  void SetMakefile(cmMakefile const *mf) { this->Makefile = mf; }

private:
  cmMakefile const *Makefile;
};

#endif
