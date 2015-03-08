#! /usr/bin/env python

import subprocess
import sys

main_file = sys.argv[2] + "/main.cpp"

for line, content in enumerate(open(main_file), 1):
  column = content.find("// AUTOCOMPLETE_CURSOR_POSITION")
  if column != -1:
    location = main_file + ":" + str(line) + ":" + str(column)
    break

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
