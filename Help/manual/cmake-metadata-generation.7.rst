.. cmake-manual-description: CMake Metadata Generation

cmake-metadata-generation(7)
****************************

.. only:: html

   .. contents::

Introduction
============

:manual:`cmake(1)` generates files containing metadata about the
buildsystem if the :variable:`CMAKE_GENERATE_METADATA` variable is ``ON``.
Such metadata files are designed to be useful to IDEs, refactoring tools,
and other tools which have a need to understand the buildsystem and the
code being built.  A single :manual:`cmake-buildsystem(7)` may describe
buildsystem contents and build properties which differ based on
:manual:`generation-time context <cmake-generator-expressions(7)>`
including:

* The Platform (eg, Windows, APPLE, Linux).
* The build configuration (eg, Debug, Release, Coverage).
* The Compiler (eg, MSVC, GCC, Clang) and compiler version.
* The language of the source files compiled.
* Available compile features (eg CXX variadic templates).
* CMake policies.

The metadata file aims to provide information to tooling to satisfy
several needs:

#. Access the compile options used when compiling sources for semantic
   analysis of source code, autocompletion, code navigation etc.
#. Provide a complete and easily parsed source of all information relevant
   to the tooling.  There should be no need for tooling to parse generated
   buildsystems.
#. Semantic information about the CMake buildsystem itself.
#. Required runtime information for running targets with other tools.

The contents of this file give an accurate description of the compilation
properties for each file in the buildsystem, such as compile options and
definitions used, include directories used etc.  This allows tooling to
offer autocompletion and semantic refactoring for example.

For each target in the buildsystem, output locations and link dependencies
are specified.  This allows tools to configure the environment correctly to
run each target. (TODO: Document this properly. What exactly should be in
the file?)

For each target in the buildsystem, a backtrace is provided indicating
where the target is declared.

Generating Metadata
-------------------

Tooling wishing to consume the metadata file is expected to execute a
command in the top-level of the build directory such as:

.. code-block:: console

  $ cmake . -DCMAKE_GENERATE_METADATA=3.3

The value passed to the command line corresponds to the
desired 'compatibility version' of the metadata file, and it refers to a
version of :manual:`cmake(1)`.  Tooling should specify either the latest
version of the metadata output it knows how to consume, or the actual
version of :manual:`cmake(1)` which is to be executed, if that is a lower
version and the tooling knows how to consume the metadata it produces.

:manual:`cmake(1)` generates one metadata file for each configuration
active for the build. The files are placed in the top-level build
directory and are named ``cmake-metadata.json`` or
``cmake-metadata-<CONFIG>.json``.  Consumers of the metadata files should
consume the config-specific version if a desired config is known and the
file is present, and fall back to the generic version.

Additionally, :manual:`cmake(1)` generates a schema file describing in
machine-readable form the structure of the metadata document, properties
it contains, and whether those properties are always present or
optional.  That is, the schema is a machine-readable version of the
description of supported properties below.

The schema document corresponding to the generated files is copied into
the build directory by :manual:`cmake(1)`.

Metadata Contents
=================

Each metadata property may be 'unconditionally present',
'conditionally present', 'optionally present', or
'conditionally optionally present'.

Unconditional Properties
------------------------

The top-level of each metadata file always contains the following
properties:

``generator``
  The :manual:`generator <cmake-generators(7)>` used to configure the build.
``platform_id``
  The ``PLATFORM_ID`` name defined by CMake.

  .. Project name, version?

Optional Properties
-------------------

The top-level of each metadata file might contain the following properties,
depending on whether a compiler for a language is set up and whether any
targets are defined:

``<lang>_compiler``
  Full path to the compiler used for files of :prop_sf:`LANGUAGE` ``lang``.
``<lang>_compiler_id``
  The ``<LANG>_COMPILER_ID`` name defined by CMake.
``<lang>_compiler_version``
  The ``<LANG>_COMPILER_VERSION`` name defined by CMake.
``<lang>_compile_command``
  Template specifying the order of build properties specified on the command
  line.  Of the
  form ``<COMPILE_OPTIONS> <COMPILE_DEFINITIONS> <INCLUDE_DIRECTORIES> <SOURCE_FILE_COMPILE_OPTIONS>``.
  TODO: Make a list instead? Document replacement variables.
``targets``
  The mapping of target names to target objects in the buildsystem.  Each object in this
  property has properties described in `Target Properties`_.
``sources``
  The list of sources with specific extra build properties specified.  Each
  object in this property has properties described
  in `Source File Properties`_.


Target Properties
=================

Unconditional properties
------------------------

The following properties are always present for each target:

``name``
  The :prop_tgt:`NAME` of the target in the generated buildsystem.
``type``
  The :prop_tgt:`TYPE` of the target.

Conditional Properties
----------------------

The following properties may be expected to be present for targets of type
``STATIC_LIBRARY``, ``SHARED_LIBRARY``, ``MODULE_LIBRARY``,
``OBJECT_LIBRARY``, ``EXECUTABLE`` and ``UTILITY``:

``backtrace``
  A list of objects containing file paths and line numbers which show
  the path to the line of code which specifies the target.

The following properties may be expected to be present for targets of type
``STATIC_LIBRARY``, ``SHARED_LIBRARY``, ``MODULE_LIBRARY``, and
``EXECUTABLE``:

``target_file``
  Full path to main file (.exe, .so.1.2, .a) where ``tgt`` is the name of a target.

The following properties may be expected to be present for targets which are
linkable on Windows:

``target_linker_file``
  File used to link (.a, .lib, .so) where ``tgt`` is the name of a target.

Optional Properties
-------------------

The following properties may or may not be present for targets of type
``STATIC_LIBRARY``, ``SHARED_LIBRARY``, ``MODULE_LIBRARY``,
``EXECUTABLE``:

``target_pdb_file``
  Full path to the linker generated program database file (.pdb)
  where ``tgt`` is the name of a target.

  See also the :prop_tgt:`PDB_NAME` and :prop_tgt:`PDB_OUTPUT_DIRECTORY`
  target properties and their configuration specific variants
  :prop_tgt:`PDB_NAME_<CONFIG>` and :prop_tgt:`PDB_OUTPUT_DIRECTORY_<CONFIG>`.


The following properties may or may not be present for targets of type
``SHARED_LIBRARY``:

``target_soname_file``
  File with soname (.so.3) where ``tgt`` is the name of a target.

Conditional Optional Properties
-------------------------------

The following properties may or may not be present for targets of type
``STATIC_LIBRARY``, ``SHARED_LIBRARY``, ``MODULE_LIBRARY``,
``OBJECT_LIBRARY``, and ``EXECUTABLE``.  At least one of the lists will
be present for each target.

``object_sources_<LANG>``
  The list of object source files of :prop_sf:`LANGUAGE` ``LANG`` for
  the target, if it is a compilable target.  The files listed in this
  property are compiled into object files with the ``LANG`` compiler. Each
  entry in the list is the full path to the file.  The list contains at
  least one file.
``generated_object_sources_<LANG>``
  List of 'objects sources' listed in the target which are
  :prop_sf:`GENERATED` by the buildsystem.  The list, if present, contains
  at least one file.
``header_sources``
  List of 'header files' listed in the target.  These files will not be
  compiled individually.  The list, if present, contains at least one
  file.
``generated_header_sources``
  List of 'header files' listed in the target which are
  :prop_sf:`GENERATED` by the buildsystem.  The list, if present, contains
  at least one file.
``extra_sources``
  List of files not associated with any compiler, but listed as part of
  the project.  The list, if present, contains at least one file.
``generated_extra_sources``
  List of :prop_sf:`GENERATED` files not associated with any compiler,
  but listed as part of the project.  The list, if present, contains at
  least one file.
``excluded_sources``
  List of files which may be part of the build for other platforms or
  configurations, but not part of the build described in this metadata
  file.  The list, if present, contains at least one file.

The following properties may or may not be present for targets of type
``UTILITY``

``comment``
  A comment for the execution of the utility.

Language Fallback Properties
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some properties may be present in language specific and language agnostic
versions.  Consumers of the metadata file should first attempt to consume
the language-specific property, and if it is not present, fall back to the
language agnostic property.

``compile_options_<LANG>``
  List of :prop_tgt:`compile options <COMPILE_OPTIONS>` passed to the
  compiler when compiling objects of :prop_sf:`LANGUAGE` ``LANG``.  Note
  that each source file might specify additional flags used.
  See `Source File Properties`_.  The flags are in an unspecified but
  deterministic order, and are passed to the compiler in the same order.
``compile_options``
  Fallback version of ``compile_options_<LANG>``.
``compile_definitions_<LANG>``
  List of :prop_tgt:`compile definitions <COMPILE_DEFINITIONS>` passed to
  the compiler when compiling objects of :prop_sf:`LANGUAGE` ``LANG``.  The
  definitions are specified without a ``-D`` prefix.  The flags are in an
  unspecified but deterministic order, and are passed to the compiler in
  the same order.
``compile_definitions``
  Fallback version of ``compile_definitions_<LANG>``.
``include_directories_<LANG>``
  List of :prop_tgt:`include directories <INCLUDE_DIRECTORIES>` passed to
  the compiler when compiling objects of :prop_sf:`LANGUAGE` ``LANG``.
``include_directories``
  Fallback version of ``include_directories_<LANG>``.

Source File Properties
======================

The following properties are available on source files.

Unconditional properties
------------------------

The following properties are always present for each source file:

``file_path``
  The full path to the file.


Optional Properties
-------------------

The following properties may or may not be present for each source file.  At
least one of the properties will be present:

``compile_flags``
  Additional :prop_sf:`compile flags <COMPILE_FLAGS>` which are used when
  compiling the source file.
``compile_definitions``
  Additional :prop_sf:`compile definitions <COMPILE_DEFINITIONS>` which are
  used when compiling the source file.  This may contain duplication
  with what is defined in the target, and duplicates should be removed by
  the tool.
