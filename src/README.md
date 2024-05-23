<!---
  SPDX-FileCopyrightText: 2023 SAP SE

  SPDX-License-Identifier: Apache-2.0

  This file is part of FEDEM - https://openfedem.org
--->

# Source code organization

The FEDEM GUI sources are organized into several modules in a multi-level
file structure. The top-level modules are as follows.

* [vpmApp](vpmApp) - The application classes.
  These classes constitute the glue between the different part of the code,
  and handle most application logic.
  The `vpmAppUAMap` classes have all the logic needed to
  transport data between the UI and the DB.
  The `vpmAppCmds` classes implement of the various commands the user can invoke
  from the menus of the GUI, or at least the entry point from the UI for these
  commands. Some calls are just forwarded to other parts of the code.
  The `vpmAppProcess` classes manage input generation for and execution of the
  solvers as child processes.
  The `vpmAppDisplay` sub-folder has classes to perform the animation and graph
  reading and manipulation.

* [vpmDB](https://github.com/openfedem/fedem-mdb/tree/master/vpmDB) -
  The mechanism database.
  Contains classes representing the model entities of a FEDEM model,
  with methods for file input/output, topology, etc.

* [vpmDisplay](vpmDisplay) - Classes that use OpenInventor
  to generate the visualization in the 3D modeler view.

* [vpmPM](vpmPM) - Process communication and results database management.

* [vpmUI](vpmUI) - The Fedem-specific user-interface classes.
  The `vpmUIComponents` classes are typically used several times
  or at least once inside some other UI-classes.
  The `vpmUITopLevels` classes are typically single window UIs.

* [assemblyCreators](https://github.com/openfedem/fedem-mdb/tree/master/assemblyCreators) -
  Generation of parameterized wind turbine and marine structure models.

* [FFuLib](FFuLib) - Application-independent user interface classes.
  This is mainly a wrapper around Qt.

* [FFdCadModel](FFdCadModel) - Classes for representation of CAD models as Parts.

* [comApi](comApi) - COM-based API for model creation and manipulation.

Note that the `vpmDB` and `assemblyCreators` modules are included through
a [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules)
reference to the repository [fedem-mdb](https://github.com/SAM/fedem-mdb).

## Class name convention

The classes are typically named by what module they belong to.
Classes prefixed with a double 'F', e.g., `FFaVec3`, belong to the
"Fedem Foundation" set of classes. They are application-independent classes that
can be used as a building block. Most of the FF-classes are located in the
repository [fedem-foundation](https://github.com/openfedem/fedem-foundation),
which is included as a submodule of the
[fedem-mdb](https://github.com/SAM/fedem-mdb) repository.

* FFa - Auxiliary classes
* FFl - FE part handling
* FFr - Result file API
* FFp - Post-processing of curve data
* FFu - User interface components
* Fui - vpmUI user interface
* Fm  - vpmDB mechanism database
* Fd  - vpmDisplay display management
* Fp  - vpmPM process management
* Fap - vpmApp application components
* FapUA - vpmAppUA user interface to application mapping

## Static classes for application logic

All the "old" modules `vpmDB`, `vpmUI`, `vpmDisplay` and `vpmPM`
have a big static class that does "everything" that could not be put
inside one of the more sensible objects.
These classes contain application logic stuff that now really belongs to the
`vpmApp` module, but then had its place in the different static utility classes.
The classes are `FmDB`, `Fui`, `FdDB` and `FpPM`.

* `FmDB` contains the initialization of the database, the control of the
  model file file reading, access methods to traverse the DB objects, etc.

* `Fui` contains a lot of application definitions for the user interface.

* `FdDB` contains a lot of functionality for modelling and rendering.
  It initializes the Inventor graph and has implementations of a lot of
  commands that interact with the 3D visualization.
  The major part of this class is implementations of user processes
  (first pick something, if selected then highlight, then if done then ...)
  like attach, (smart)move, create joint, etc.

* `FpPM` contains some RDB-handling, the top-most code of file open/save, etc.
  This class is also a bit "oldish" and should have been cleaned up and
  perhaps removed.
