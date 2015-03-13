#! /usr/bin/env python

import subprocess
import sys
import json
import codecs
from pprint import pprint

main_file = sys.argv[2] + "/main.cpp"

for line, content in enumerate(open(main_file), 1):
  column = content.find("// AUTOCOMPLETE_CURSOR_POSITION")
  if column != -1:
    location = main_file + ":" + str(line) + ":" + str(column)
    break

class Buildsystem:
  def __init__(self):
    self.targets = []
    self.generator = ""

  def parse(self, json_filename, schema_filename):
    # Read the schema first and store it.
    # Read the json_file, validate it and store it.

    with codecs.open(schema_filename, "rU", "utf-8") as schema_file:
      self.schema = json.load(schema_file)

    with codecs.open(json_filename, "rU", "utf-8") as json_file:
      self.json = json.load(json_file)

  def languageForFile(self, target, filename):
    # Awkward to need the target here.
    tgt = self.json["targets"][target]
    if filename in tgt["object_sources_cxx"]:
      return "cxx"
    if filename in tgt["object_sources_c"]:
      return "c"

  def compilerForFile(self, target, filename):
    # Awkward to need the target here.
    fl = self.languageForFile(target, filename)
    return self.json[fl + "_compiler"]

  def compilerVersionForFile(self, target, filename):
    # Awkward to need the target here.
    fl = self.languageForFile(target, filename)
    return self.json[fl + "_compiler_version"]

  def compilerIdForFile(self, target, filename):
    # Awkward to need the target here.
    fl = self.languageForFile(target, filename)
    return self.json[fl + "_compiler_id"]

  def getCompileProperty(self, target, filename, propertyName):
    # TODO: Schema check

    # Get from the target, then get the additions from the filename.
    fl = self.languageForFile(target, filename)
    tgt = self.json["targets"][target]
    defs = []
    if (propertyName + "_" + fl) in tgt:
      defs = tgt[propertyName + "_" + fl][:]
    else:
      defs = tgt[propertyName][:]

    file_defs = []
    if filename in self.json["sources"]:
      if propertyName in self.json["sources"][filename]:
        file_defs = self.json["sources"][filename][propertyName][:]
        defs += file_defs

    return defs

  def getCompileDefinitions(self, target, filename):
    return self.getCompileProperty(target, filename, "compile_definitions")

  def getIncludeDirectories(self, target, filename):
    return self.getCompileProperty(target, filename, "include_directories")

  def getCompileOptions(self, target, filename):
    return self.getCompileProperty(target, filename, "compile_options")

bs = Buildsystem()
bs.parse(sys.argv[3] + "/cmake-metadata.json",
         sys.argv[3] + "/cmake-metadata-schema.json")

print bs.getCompileDefinitions("main",
  "/home/stephen/dev/src/cmake/Tests/Metadata/main.cpp")
print bs.getCompileDefinitions("main",
  "/home/stephen/dev/src/cmake/Tests/Metadata/bar_file.cpp")
print bs.getCompileDefinitions("main",
  "/home/stephen/dev/src/cmake/Tests/Metadata/c_file.c")
print bs.getCompileDefinitions("main",
  "/home/stephen/dev/src/cmake/Tests/Metadata/compiled_as_cxx.c")
print bs.getIncludeDirectories("main",
  "/home/stephen/dev/src/cmake/Tests/Metadata/main.cpp")
print bs.getIncludeDirectories("main",
  "/home/stephen/dev/src/cmake/Tests/Metadata/bar_file.cpp")
print bs.getIncludeDirectories("main",
  "/home/stephen/dev/src/cmake/Tests/Metadata/c_file.c")
print bs.getIncludeDirectories("main",
  "/home/stephen/dev/src/cmake/Tests/Metadata/compiled_as_cxx.c")
print bs.getCompileOptions("main",
  "/home/stephen/dev/src/cmake/Tests/Metadata/main.cpp")
print bs.getCompileOptions("main",
  "/home/stephen/dev/src/cmake/Tests/Metadata/bar_file.cpp")
print bs.getCompileOptions("main",
  "/home/stephen/dev/src/cmake/Tests/Metadata/c_file.c")
print bs.getCompileOptions("main",
  "/home/stephen/dev/src/cmake/Tests/Metadata/compiled_as_cxx.c")

print
print
print bs.compilerForFile("main", "/home/stephen/dev/src/cmake/Tests/Metadata/compiled_as_cxx.c")
print bs.compilerIdForFile("main", "/home/stephen/dev/src/cmake/Tests/Metadata/compiled_as_cxx.c"), bs.compilerVersionForFile("main", "/home/stephen/dev/src/cmake/Tests/Metadata/compiled_as_cxx.c")
print

# TODO: Read these from the metadata file.
compile_args = [
  "-DCONFIG_IS_",
  "-DCONFIG_IS_NOT_DEBUG=0",
  "-DCONFIG_IS_NOT_RELEASE=0",
  "-DCXX_COMPILER_HAS_VARIADIC_TEMPLATES=1",
  "-DCXX_COMPILER_IS_Clang",
  "-DC_COMPILER_IS_Clang",
  "-DMAIN_DEF",
  "-DPLATFORM_IS_Linux",
  "-DSHARED_PUBLIC",
  "-DSTATIC_PUBLIC",
  "-DLANG_IS_CXX",
  "-I/home/stephen/dev/src/cmake/Tests/Metadata/build",
  "-I/home/stephen/dev/src/cmake/Tests/Metadata",
  "-I/home/stephen/dev/src/cmake/Tests/Metadata/main_dir",
  "-I/home/stephen/dev/src/cmake/Tests/Metadata/static_public_dir",
  "-I/home/stephen/dev/src/cmake/Tests/Metadata/shared_public_dir",
  "-DCOMPILE_LANGUAGE_CXX",
  "-DTHIS_IS_THE_MAIN_FILE"
]

if False:
  output = subprocess.check_output(
    [
      sys.argv[1],
      "-fsyntax-only",
      "-Xclang", "-code-completion-at=" + location,
      main_file
    ] + compile_args
  )

  expected_completions = [
    'COMPLETION: shared_dep : [#int#]shared_dep()',
    'COMPLETION: static_dep : [#int#]static_dep()',
    'COMPLETION: obj_dep : [#int#]obj_dep()',
    'COMPLETION: compiled_as_cxx : [#int#]compiled_as_cxx()',
    'COMPLETION: static_use_symbol : [#int#]static_use_symbol()',
    'COMPLETION: bar_file : [#int#]bar_file()',
    'COMPLETION: c_file : [#int#]c_file()',
    'COMPLETION: foo_linux : [#int#]foo_linux()',
    'COMPLETION: foo_windows_or_linux : [#int#]foo_windows_or_linux()',
    'COMPLETION: foo_linux_clang : [#int#]foo_linux_clang()',
  ]

  expected_not_completions = [
    'COMPLETION: foo_windows : [#int#]foo_windows()',
    'COMPLETION: foo_linux_gcc : [#int#]foo_linux_gcc()',
    'COMPLETION: foo_gcc_gt_44 : [#int#]foo_gcc_gt_44()',
    'COMPLETION: foo_gcc_lt_44 : [#int#]foo_gcc_lt_44()',
    'COMPLETION: foo_gcc_eq_44 : [#int#]foo_gcc_eq_44()',
    'COMPLETION: foo_gcc_gte_44 : [#int#]foo_gcc_gte_44()',
  ]

  for entry in expected_completions:
    if not entry in output:
      print "Failed to find: ", entry
      sys.exit(1)

  for entry in expected_not_completions:
    if entry in output:
      print "Unexpected entry: ", entry
      sys.exit(1)
