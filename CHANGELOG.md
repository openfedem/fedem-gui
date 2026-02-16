<!---
  SPDX-FileCopyrightText: 2023 SAP SE

  SPDX-License-Identifier: Apache-2.0

  This file is part of FEDEM - https://openfedem.org
--->

# Open FEDEM Changelog

## [fedem-8.1.4] (2026-02-16)

### :rocket: Added

- Issue https://github.com/openfedem/fedem-gui/issues/128:
  The direction of point loads can now be defined by selecting triads
  which are not connected to a part.
- Issue https://github.com/openfedem/fedem-gui/issues/126:
  A Beam diagram curve can now be defined by selecting a group of triads or joints
  which not are connected through a beamstring.

### :bug: Fixed

- Issue https://github.com/openfedem/fedem-gui/issues/130:
  The Friction property editor panel does not work properly.
- Issue https://github.com/openfedem/fedem-gui/issues/125:
  The GUI may crash when plotting a Beam diagram curve,
  and changing the time field in the property editor panel.

## [fedem-8.1.3] (2026-01-20)

### :rocket: Added

- The FE Node tab of the Part propety panel now displays a list of connected elements.

### :bug: Fixed

- Issue https://github.com/openfedem/fedem-gui/issues/120:
  Multi-selection does not work when changing the DOF status for joints and triads.

## [fedem-8.1.2.7] (2025-12-28)

### :rocket: Added

- A Generic part can now be created by importing a Wavefront OBJ-file.

### :bug: Fixed

- Issue https://github.com/openfedem/fedem-gui/issues/115:
  Plotted curves pointing to non-existing results do not update when changing to an existing result item.
- Issue https://github.com/openfedem/fedem-gui/issues/113:
  The sea surface visualization does not work properly.
- Issue https://github.com/openfedem/fedem-gui/issues/111:
  The Global/Local Reference settings in the Load objects do not preserve their values.
- Issue https://github.com/openfedem/fedem-gui/issues/108:
  Component modes can not be animated directly after solving them.
- Issue https://github.com/openfedem/fedem-gui/issues/105:
  The Generic Part visualization does not update when detaching a triad from it.
- Issue https://github.com/openfedem/fedem-gui/issues/104:
  The local axis visualization for Parts and Triads does not disappear when it should.
- Issue https://github.com/openfedem/fedem-gui/issues/102:
  The Center of Gravity fields for Parts do not work properly.
  (see also issue https://github.com/openfedem/fedem-gui/issues/71).
- Issue https://github.com/openfedem/fedem-gui/issues/101:
  The step and time counters do not show up when running animations on Linux.

## [fedem-8.1.2] (2025-11-01)

### :rocket: Added

- FMU export for the Linux version.

### :bug: Fixed

- Issue https://github.com/openfedem/fedem-gui/issues/97:
  Generic part Center of Gravity coordinates are reset to zero after save.
- Exported FMUs with the default model description "<default_m.fmm>" does not work.

## [fedem-8.1.1] (2025-10-15)

### :rocket: Added

- Issue https://github.com/openfedem/fedem-gui/issues/88:
  Check box on part to ignore it when doing stress recovery .
- Issue https://github.com/openfedem/fedem-gui/issues/91:
  Type node numbers directly when creating/editing Strain Rosette objects.

### :bug: Fixed

- Issue https://github.com/openfedem/fedem-gui/issues/89:
  Repeat curve for all objects does not work.
- Issue https://github.com/openfedem/fedem-gui/issues/94:
  When deleting a "Time history input file" object a hidden Function appears.

## [fedem-8.1.0.8] (2025-09-16)

### :rocket: Added

No new features in this release, but the use of Qt layout managers of positioning
the various GUI components is now complete, also for all the property views.

### :bug: Fixed

- The `Qt6OpenGLWidgets.dll` file is missing in the Windows installation.
- The GUI freezes when the Airfoil Browser dialog is launched on Windows.
- Some GUI components appear and disappear instantly when the Dynamics Solver dialog is launched the first time.
- The "File of type:" menu in the file selection dialog contains "All files (*)" only.
- When creating a sea wave function, the default function type is wrong (should be Sine).
- Issue https://github.com/openfedem/fedem-gui/issues/83:
  Deleting and pasting does not work for Linear derivative functions.

## [fedem-8.1.0] (2025-07-10)

### :rocket: Initial release of FEDEM R8.1

The FEDEM GUI is now built with the open-source
[Qt 6.8](https://www.qt.io/product/framework) framework.
This involves a lot of code changes to port the FEDEM sources from Qt 4 to Qt 6,
while maintaining all existing functionality.
The porting effort has been performed incrementally.
That is, first the necessary changes to build with Qt 5 were added in
a separate PR (https://github.com/openfedem/fedem-gui/pull/77),
followed by another PR (https://github.com/openfedem/fedem-gui/pull/78)
with the changes required to build with Qt 6.
Finally, through https://github.com/openfedem/fedem-gui/pull/79,
further modifications where done to make the Qt-based widgets more portable
using layout managers instead of the elaborate explicit geometry calculations.

If there is a need for continuing working with either Qt 4 or Qt 5, this can be
done if you checkout branch `fedem-8.0-qt4` or `fedem-8.1-qt5`, respectively.
These branches will not receive any significant updates in the future,
except perhaps for smaller bug-fixes which are not related to Qt 6.
All new features and major changes will be added to the main branch only.

This release also contains the first installation package for Linux platform
(built with gcc 11 and Qt 6.2 on Ubuntu 22.04).
To try out this package, just unpack the file
[Fedem-R8.1.0_Ubuntu-22.04.tar.gz](https://github.com/openfedem/fedem-gui/releases/download/fedem-8.1.0/Fedem-R8.1.0_Ubuntu-22.04.tar.gz)
in some arbitrary location `<installation-folder>`,
and launch FEDEM through the command:

        $ <installation-folder>/Fedem-8.1.0/fedem [<options>]

You will need to install the Qt 6 runtime libraries through the package manager
before doing this, as they are not included in the FEDEM distribution:

        $ sudo apt install libqt6core6 libqt6gui6 libqt6widgets6 libqt6opengl6

## [fedem-8.0.9.7] (2025-07-01)

### :rocket: Added

- Issue https://github.com/openfedem/fedem-gui/issues/41:
  Multi-selection update for the position property fields.
- Issue https://github.com/openfedem/fedem-gui/issues/66:
  Button for switching between prismatic and cylindric joint.
- SET1 Nastran bulk data entries are now processes for definition of element groups.

### :bug: Fixed

- Issue https://github.com/openfedem/fedem-gui/issues/68:
  When changing the FE model of an existing Part, any description field command
  defined with the '#'-character is erased.
- Issue https://github.com/openfedem/fedem-gui/issues/70:
  The GUI crashes on creating a prismatic- or cylindric joint between triads,
  unless a third triad for the follower also is picked.
- Issue https://github.com/openfedem/fedem-gui/issues/72:
  The Dismiss dialog is blocking the rest of the GUI. It is supposed to be non-modal.
- Issue https://github.com/openfedem/fedem-gui/issues/74:
  The GUI crashes when run batch
  (using command-line options `-f` and either `-solve` or `-prepareBatch`).
- Nastran bulk data files with a non-blank offset flag field for CBAR and CBEAM
  entries are not parsed properly.
- Extraction of element group names from NX property comments and Hypermesh
  comments in Nastran bulk data files does not work.
- Parsing RBE2 Nastran bulk data entries with a float value as its last field does not work.

## [fedem-8.0.9] (2025-03-20)

### :rocket: Added

- <u>Functionality change</u>:
  To _move_ a Curve object from one Graph to another,
  keep the **Shift** key pressed while dropping the Curve.
  To _copy_ a Curve object, drop the Curve without pressing any key.
  Pressing the **Ctrl** key to copy the Curve is no longer needed.
- FE nodes and properties which are not referred by any elements
  will be deleted when the FE part is imported into FEDEM.
- A separate group for Beam objects is added in the Objects Browser dialog box.

### :bug: Fixed

- Issue https://github.com/openfedem/fedem-gui/issues/61 :
  The GUI crashes when deleting a Graph object while its graph view is open.
- Issue https://github.com/openfedem/fedem-gui/issues/59 :
  It is not possible to select curve components for Combined curves.
- Issue https://github.com/openfedem/fedem-gui/issues/58 :
  Copying a Curve within the same Graph using drag-and-drop
  in the objects list view does not work.
- Issue https://github.com/openfedem/fedem-gui/issues/56 :
  When attaching joints, the joint symbol in the 3D modeler view
  does not update its colors until the joint is re-selected after
  the attach procedure is finished.
- The time/step information in the upper right corner of the 3D modeler
  view is not placed correctly when an Animation is running,
  such that some of the digits in the time are cut off.

## [fedem-8.0.8] (2025-01-04)

### :rocket: Added

- Issue https://github.com/openfedem/fedem-gui/issues/44 :
  `#MassScaleEngine <bid>` can now be used for Beams and FE parts,
  in addition to for Triads as before.

### :bug: Fixed

- Issue https://github.com/openfedem/fedem-gui/issues/48 :
  Selecting curve objects by clicking in the graph view does not work.
- Issue https://github.com/openfedem/fedem-gui/issues/47 :
  Auto-detection of triads along a prismatic/cylindric joints have tolerance issues.
- Issue https://github.com/openfedem/fedem-gui/issues/45 :
  Copy curves by dragging into a new Graph group does not work.
- Minor adjustments of GUI fields in the Solver setup dialog box
  and in the Scale and Shift tab of the Curve property editor panel.

## [fedem-8.0.7] (2024-11-17)

### :rocket: Added

- Issue https://github.com/openfedem/fedem-gui/issues/7 :
  Support MPC-records in Nastran bulk data files through conversion to
  WAVGM elements with explicit coupling coefficients.

### :bug: Fixed

- Issue https://github.com/openfedem/fedem-gui/issues/39 :
  Copy-pasting comma-separated data into the polyline function
  property field does not work.
- Issue https://github.com/openfedem/fedem-gui/issues/37 :
  Copying multi-argument math expression functions does not work.
- Issue https://github.com/openfedem/fedem-gui/issues/35 :
  Using csv as ASCII file extension for Polyline-from-file functions
  does not work.
- Issue https://github.com/openfedem/fedem-gui/issues/34 :
  FE part reduction goes into an infinite loop if two parts
  share the same FE data file.

## [fedem-8.0.6] (2024-10-18)

### :rocket: Added

- Issue https://github.com/openfedem/fedem-gui/issues/28 :
  Option to connect a triad with surface connector elements to
  existing FE node when changing the FE part and then erase the connector.
- Issue https://github.com/openfedem/fedem-gui/issues/27 :
  Append `.fmm` extension to the file name in the Save As... dialog
  if not explicitly specified.

### :bug: Fixed

- Issue https://github.com/openfedem/fedem-gui/issues/32 :
  Wrong data format in exported FMU. Exclude reduction data which
  is not used by FMU simulation to reduce the file size.
  Ensure all frs-output options are switched off in the exported FMU.
- Issue https://github.com/openfedem/fedem-gui/issues/30 :
  Creating prismatic and cylindric joints on FE parts that are
  translated and/or rotated after import does not work.

## [fedem-8.0.5] (2024-09-27)

### :rocket: Added

- The Channel field is updated to contain the column label for
  poly-line-from-file functions, also if the file used is a two-colum file.
  If the file have no headings, "n/a" is displayed in the Channel field.
- The built GUI will now enable the Windpower features, if the installation
  contains the AeroDyn binary with associated dynamics solver (not part of the package).
- The Users Guide now contains chapters on Windpower and Marine modeling.

### :bug: Fixed

- Issue https://github.com/openfedem/fedem-foundation/issues/11 :
  Previewing poly-line-from-file functions may cause crash or wrong results
  if the definition domain is smaller than the evaluation domain.
- Issue https://github.com/openfedem/fedem-gui/issues/24 :
  Modelling prismatic/cylindric joints by selecting existing triads does not work properly.
- Issue https://github.com/openfedem/fedem-gui/issues/23 :
  Using a time interval for a graph plotting simulation results have some round-off errors.
- Issue https://github.com/openfedem/fedem-gui/issues/20 :
  Multi-select does not work when updating joint DOF properties.
- Issue https://github.com/openfedem/fedem-gui/issues/17 :
  The Fourier Analysis and Differentiate/Integrate toggles on curves do not work properly.
- Issue https://github.com/openfedem/fedem-gui/issues/15 :
  The Additional Solver Options field for the Dynamics Solver does not work always.

## [fedem-8.0.4] (2024-07-12)

### :bug: Fixed

- Issue https://github.com/openfedem/fedem-gui/issues/13 :
  Dynamics solver does not start if the model uses single-channel input functions.

## [fedem-8.0.3] (2024-06-13)

### :rocket: Added

- Support for L-profiles from PBEAML-records in Nastran bulk data files.
- Account for non-symmetric beam cross sections from Nastran bulk data files.
- A progress bar in the bottom of the main GUI running while creating
  the visualization model of the FE parts.
- The content of the View control toolbar in the GUI will depend on whether
  Modeler view, at least one Graph view, or both are open.
- The links to external web-sites in the Fedem.chm file are now opened
  in the default web-browser.

### :bug: Fixed

- Nastran bulk data files with PBEAM records with an empty or blank line
  immediately after the SO-field is not parsed correctly.
- Nastran bulk data files containing case control statements before the start
  of the bulk data is not parsed correctly.
- Parsing of Nastran bulk data files containing CBEAM records where the 7th
  field contains a text string fails.
- Multiple SPC (or SPC1) entries in a Nastran bulk data file for the same node
  is not handled correctly.
- Issue https://github.com/openfedem/fedem-gui/issues/1 :
  Loading animations in the GUI with the "Load line contours" toggle activated
  may cause crash.
- Issue https://github.com/openfedem/fedem-gui/issues/4 :
  The "Move to Center" command in the GUI does not work.

## [fedem-8.0.2] (2024-02-29)

### :rocket: Added

- A "Recent models" sub-menu is added in the "File" menu, from where (up to)
  the 10 last opened models in the current session can be reopened.
- If the model contains Strain Rosette elements which are used by Sensors,
  the "Perform strain rosette recovery during dynamics simulation" toggle
  is automatically enabled for FE Parts containing such Strain Rosette elements.

### :bug: Fixed

- If a template file is not available the default model name is not set and the
  current working directory is attempted cleaned with strange console messages.
- The command for opening the Users Guide is missing in the "Help" menu.
- The GUI crashes if selecting "Generic DB Object" from the "Mechanism" menu.
- The GUI crashes if an invalid expression with unbalanced parantheses
  is entered for Math Expression functions.
- The COM Type library for Fedem is missing.

## [fedem-8.0.1] (2024-01-25)

### :rocket: Added

- The "Marine" menu with hydrodynamics features is back in the GUI.

### :bug: Fixed

- Graph views show only the first point in each curve when plotting
  time history results.
- Curve plotting is deactivated and Curve/Graph properties are hidden.
- Incorrect default location of file browser on "Open" and "Save As...".
- False error messages on failure to delete files when doing "Save As...".

## fedem-8.0.0 (2023-12-21)

### :rocket: FEDEM R8.0

- This is the first open-source version of FEDEM,
  including binaries for GUI and solvers on Windows platform.

[fedem-8.0.1]: https://github.com/openfedem/fedem-gui/compare/fedem-8.0.0...fedem-8.0.1
[fedem-8.0.2]: https://github.com/openfedem/fedem-gui/compare/fedem-8.0.1...fedem-8.0.2
[fedem-8.0.3]: https://github.com/openfedem/fedem-gui/compare/fedem-8.0.2...fedem-8.0.3
[fedem-8.0.4]: https://github.com/openfedem/fedem-gui/compare/fedem-8.0.3...fedem-8.0.4
[fedem-8.0.5]: https://github.com/openfedem/fedem-gui/compare/fedem-8.0.4...fedem-8.0.5
[fedem-8.0.6]: https://github.com/openfedem/fedem-gui/compare/fedem-8.0.5...fedem-8.0.6
[fedem-8.0.7]: https://github.com/openfedem/fedem-gui/compare/fedem-8.0.6...fedem-8.0.7
[fedem-8.0.8]: https://github.com/openfedem/fedem-gui/compare/fedem-8.0.7...fedem-8.0.8
[fedem-8.0.9]: https://github.com/openfedem/fedem-gui/compare/fedem-8.0.8...fedem-8.0.9
[fedem-8.0.9.7]: https://github.com/openfedem/fedem-gui/compare/fedem-8.0.9.2...fedem-8.0.9.7
[fedem-8.1.0]: https://github.com/openfedem/fedem-gui/compare/fedem-8.0.9...fedem-8.1.0
[fedem-8.1.0.8]: https://github.com/openfedem/fedem-gui/compare/fedem-8.1.0...fedem-8.1.0.8
[fedem-8.1.1]: https://github.com/openfedem/fedem-gui/compare/fedem-8.1.0.8...fedem-8.1.1
[fedem-8.1.2]: https://github.com/openfedem/fedem-gui/compare/fedem-8.1.1...fedem-8.1.2
[fedem-8.1.2.7]: https://github.com/openfedem/fedem-gui/compare/fedem-8.1.2...fedem-8.1.2.7
[fedem-8.1.3]: https://github.com/openfedem/fedem-gui/compare/fedem-8.1.2.7...fedem-8.1.3
[fedem-8.1.4]: https://github.com/openfedem/fedem-gui/compare/fedem-8.1.3...fedem-8.1.4
