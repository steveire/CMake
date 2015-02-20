object-library-linking
----------------------

* Support linking OBJECT libraries to other targets to pick up usage
  requirements.
* Support linking to OBJECT libraries to pick up usage requirements. OBJECT
  libraries linked in this way still need to be added to the sources using
  $<TARGET_OBJECTS> so that transitive linking does not cause duplicate
  symbols.
