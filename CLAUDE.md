# Project Workflow Guidelines #

## Overview ##
This project currently does not have a specific direction.
It is mostly for experimentation with C++ and several of the
technologies available to it. Understanding its limitations
and capabilities. Educational.

## Project Design ##
This project uses C++23, at the time of prototyping and
design, this was the most recent standard available. We will
eventually be defining a project that acts as an
engine. At that point it needs to be well thought out with
plenty of time and considerations made.

### Explored Options & Concepts ###
These ideas have already been explored and/or are currently
in progress.

#### Concepts Explored ####
| Ref | Status | Concept | Decision |
| :---: | ------ | -------- | ------- |
| 0 | **Incomplete** | Desktop UI development using `ImGui` as the frontend with `SDL3` as the backend using the `SDL3` native renderer. | Possible best option for Desktop UI dev in C++ |
| 1 | **Documenting** | Plugin management systems using DLL loading. | Prototype works, still need to define final design. |
| 2 | **Complete** | Project configuration using `CMake`. | Industry standard, but cumbersome to use. |
| 3 | **Complete** | Project configuration using `meson`. | Does not play well with external tools. Highly inflexible. |
| 4 | **Exploring** | Project configuration using `xmake`. |  |
| 5 | **Complete** | Using abstractions to define interfaces. | Basic OOP concept. Need to build mental model for structure. |
| 6 | **Exploring** | Creating systems using passable contexts/packages. | Works, but should probably be limited to specific scope(s) and use-cases. |
| 7 | **Complete** | Wrapping dependencies in project-level interfaces. | Works, but should probably be used only on low-level interfaces, only implementing validation and error handling. |
| 8 | **Exploring** | Event handled errors. | Works fine. Should be used sparingly. |
| 9 | **Exploring** | Utilizing `<expected>` for modal based systems. | `C++23` standard header. Need to build mental model of how to use properly. |
| a | **Exploring** | Utilizing `<memory>` for memory management. | `C++23` standard header. Need to build mental model of how to use properly. |

## Metadata & Directives ##
This section defines the expected behaviors. Read carefully,
then read it again.

#### Rules ####
Use these rules, with reference to the table in the
**Metadata** section.

| Ref | Rule |
| :---: | ---- |
| 0 | If *LastUpdate* has a datetime older than a week, ask if I need to review and update this document. |
| 1 | If *Change Log* does not exist, create the file. |
| 2 | If *Thread Logs* does not exist, create the directory. |
| 3 | When I say, "summarize", take the main data points from our conversation and compile them as a summary in *Thread Logs* using the template *Thread Log Name* as the naming convention. |
| 4 | When I say, "summarize changes", read last commit and update *Change Log*. |
| 5 | Unless I say, "build", do not make changes to the project. Do not suggest changes. |
| 6 | Unless overridden by another rule, conversations will be discovery, exploration and analysis only. |
| 7 | Use a sober-minded disposition with responses. Enthusiasm not required. |
| 8 | During analysis, if something appears incomplete and not otherwise asked, do not report partial or incomplete implementations as "problems". If it is first-party (myself) code, it is likely known; likely not a root-cause during RCA. |
| 9 | Provide links and references from where knowledge was found. |

#### Metadata ####
Data used to manage this document and our conversations.

| Ref | Name | Type | Value |
| :---: | ---- | ---- | ----- |
| 0 | **Last Update** | Datetime | `Mon Dec 15 11:48:29 CST 2025` |
| 1 | **Change Log** | File Path | `CHANGELOG.md` |
| 2 | **Thread Logs** | Directory Path | `.claude/summaries` |
| 3 | **Thread Log Name** | Name Template | `<day_of_week>-<current_datetime>.md` |
