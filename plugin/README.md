# Ex0 Engine Plugin API and Management System <a id="ex0-dx0000"></a>
Defines the feature of being able to load dynamically linked libraries as submodules [^Module] to the main application.

# Ex0-Dx1000 Overview <a id="ex0-dx1000"></a>
---

## Ex0-Dx1100 TODOs
The Enhancement is still incomplete and needs a few things in order to be production ready.

- [ ] [Ex0-Dx3100](#ex0-dx3100) Handle cleanups and shutdowns and initialization.
- [ ] [Ex0-Dx3200](#ex0-dx3200) Declaring search paths for module objects.
- [ ] [Ex0-Dx3300](#ex0-dx3300) Generate hashes of built/loaded plugins.
- [ ] [Ex0-Dx3400](#ex0-dx3400) Plugin version matching/verification
- [ ] [Ex0-Dx3500](#ex0-dx3500) Plugin declaration.
- [ ] [Ex0-Dx3600](#ex0-dx3600) Hot reloading.

# Ex0-Dx2000 How to Read <a id="ex0-dx2000"></a>
---
This document has been divided into sections with the intent of organizing them in a easy-to-reference manner.

## Ex0-Dx2100 Organization
This document has been organized into a core group of features.

1. [Ex0-Dx0000](#ex0-dx0000) Description (The root header)
2. [Ex0-Dx1000](#ex0-dx1000) Overview
3. [Ex0-Dx2000](#ex0-dx2000) How to Read (This design section)
4. [Ex0-Dx3000](#ex0-dx3000) Features
5. [Ex0-Dx4000](#ex0-dx4000) References

Each section defines its own purpose. They define how the the enhancement is meant to be built, the development journey, and how it evolves as new features are added.

## Ex0-Dx2200 Header Points
You will notice each header is prefixed with a serial number. This number indicates where they belong in terms of order and who/what they belong to.

### Ex0-Dx2210 Subcomponents
The characters define and separate purpose. We will call the purpose thier *Subcomponent*

**Subcomponent Declarations** <a id="ex0-dx2210-Tx0"></a>
| Ref | Character | Subcomponent |
| :---: | :---: | --- |
| 0 | **E** | Enhancement |
| 1 | **D** | Design |
| 2 | **T** | Table |

### Ex0-Dx2220 Identifiers
The digits are hexidecimal, indicating what type the section is and where it falls in order of its relative siblings. We will call this portion their *Identifiers*.

**Subcomponent Identifiers** <a id="ex0-dx2220-Tx0>
| Ref | Subcomponent | Format | Enumerating |
| :---: | --- | --- | --- |
| 0 | **Enhancement** | x0 | Enhancement ID |
| 1 | **Design** | x0000 | Header ID; x[h1][h2][h3][h4] |

### Ex0-Dx2230 Component
Together, we can take the subcomponents described in the above to create a new component definition. Say we have a table nested somewhere in this text and we needed to find or reference it somewhere else, as an arbitrary example we will use the below:

**Example Reference ID**
```text
Exa-Dx4012-Tx0
```
We would be referring to the first table in the second **H4** of the first **H3** of the forth design section (**H1**).
Notice that **H2** was skipped. We ignore `0` values as this indicates there is no separation between the **H3** section and the design section.

> [!NOTE]
> The exception to this will be the root header which is, in this case, `Ex0-Dx0000`. For the purposes of moving forward, though, the root will be ignored.

# Ex0-Dx3000 Features <a id="ex0-dx3000"></a>
The plugin management API provides a number of features making the runtime as seamless as possible.

**Features Proposed & Statuses**
| Ref | Feature | Phase |
| :---: | ------ | ------ |
| 0 | [Ex0-Dx3100](#ex0-dx3100) | **Design** |
| 1 | [Ex0-Dx3200](#ex0-dx3200) | **Not Started** |
| 2 | [Ex0-Dx3300](#ex0-dx3300) | **Not Started** |
| 3 | [Ex0-Dx3400](#ex0-dx3400) | **Not Started** |
| 4 | [Ex0-Dx3500](#ex0-dx3500) | **Not Started** |
| 5 | [Ex0-Dx3600](#ex0-dx3600) | **Not Started** |

## Ex0-Dx3100 Cleanups, Setups & Shutdowns <a id="ex0-dx3100"></a>
The bare minimum requirement in order to have a plugin system is to be able to do 2 items:

1. [x] Dynamically load shared objects as modules.
2. [ ] Destroy shared objects and close descriptors.

### Ex0-Dx3110 Why
We need this functionality in order to allow, first of all, the ability for user-defined plugins to be loaded and installed. Then, secondly, we need to be able to shutdown and clean up these plugins-- regardless of end-of-runtime or fatal events.

The second point is the most important as we need to take a *Trust nothing. Verify everything* approach. If something goes wrong during plugin management, it is necessary to be able to prevent a loaded plugin from causing the global runtime to crash... or worse.

In addition to this, it should be noted that the plugin manager needs to be able to call for a shutdown if/when some failure occurs regarding a plugin module.

### Ex0-Dx3120 Required Elements
In order to complete this feature, we need to have collected the following:

- [x] Fault Scenarios (Defined [here](#ex0-d3130))
- [x] DLL Loading Library (Part of stdlib)
- [ ] Protocols Defintions

### Ex0-Dx3130 Failure Scenarios [^Fault] <a id="ex0-dx3130"></a>
Those failures can be categorized in the following scenarios.

**DLL Faults** [^DllFault] <a id="dll-faults-table"></a>
| Ref | Scenario | Description | Resolution |
| :---: | -------- | ------------| ---------- |
| 0 | Not Found | The module [^Module] file cannot be found. | Mark load as failed, append fault to log and move forward. |
| 1 | Object Not Loaded | Something went wrong when trying to load a module object. | Mark load as failed, determine fault point, append to log and move forward. |
| 2 | Not Loaded | The module file itself could not be loaded. | Refer to [**Ex0-Dx3130-Tx0.1**](#dll-faults-table) |
| 3 | Bad Format | The module is in a format not understood by the host OS. | Refer to [**Ex0-Dx3130-Tx0.1**](#dll-faults-table) |
| 4 | Object Not Found | Something went wrong when trying to get an object from the module. | Refer to [**Ex0-Dx3130-Tx0.1**](#dll-faults-table) |
| 5 | IHook Not Loaded | The expected interface hook could not be loaded. | Refer to [**Ex0-Dx3130-Tx0.1**](#dll-faults-table) |
| 6 | Loader Not Found | Tried to use a DLL loader that is not available. | The implementation of DLL loading is platform/OS specific. Make sure using correct implementation per platform. |

**Plugin Faults** [^PluginFault] <a id="plugin-faults-table"></a>
| Ref | Scenario | Description | Resolution |
| :---: | -------- | ------------| ---------- |
| 0 | Missing IHook [^IHook] | An interface hook [^Hook] is not available | Mark load as failed, append fault to log and move forward. |
| 1 | IHook Not Installed | An interface hook was not installed | Refer to [**Ex0-Dx3130-Tx0.0**](#plugin-faults-table) |
| 2 | IHook Not Loaded | An interface hook was not loaded. | Refer to [**Ex0-Dx3130-Tx0.0**](#plugin-faults-table) |
| 3 | IHook Call Failed | An interface hook returned a failure code | Append to log. If the failure code indicates catastrophy, or the hook is a critical callback, shutdown the plugin. Otherwise, ignore and move forward. |
| 4 | Install Failed | The installer for a plugin failed. | Refer to [**Ex0-Dx3130-Tx0.0**](#plugin-faults-table) |
| 5 | Missing Install Hook | The installer was not implemented or could not be loaded | Refer to [**Ex0-Dx3130-Tx0.0**](#plugin-faults-table) |
| 6 | Missing Release Hook | The release hook was not implemented or could not be loaded | Refer to [**Ex0-Dx3130-Tx0.0**](#plugin-faults-table) |

### Ex0-Dx3120 Protocols
Plugin management requires that that protocols are defined. While this feature is for cleanups, setups and shutdowns, the concepts this feature depends on are closer to atomic than the top-down view.

These protocols determine the shape of the feature, as well as the objects that will own them.

1. Plugin Manager
2. Plugin Installer
3. Plugin Destroyer
4. Plugin Artifact
5. Plugin Object

We'll define first protocals for the plugin itself.

> [!NOTE]
> The above list marks objects in order of scope.

#### Ex0-Dx3121 Plugin Objects <a id="ex0-dx3121"></a>
API methods allowing the plugin to interact with the manager. These protocols, in particular, should allow plugin developers optional hooks for pre and post handling of tasks that their constructor and destructor cannot themselves handle. Examples of this may include context-specific actions where the plugin needs to validate its environment.

**Plugin Object Protocols** <a id="ex0-dx3121-Tx0"></a>
| Ref | Identity | Description |
| :---: | --- | --- |
| 0 | OnDestroy | Cleanup/Removal hook for pre-disposal task. Should be optional, not required for plugin developers to use. |
| 1 | OnStartUp | Install hook for post-installation tasks. Should be optional, not required for plugin developers to use. |
| 2 | OnInstall | Install hook for post-installation tasks. Should be optional, not required for plugin developers to use. |
| 3 | PluginName | Should return the name of the plugin. Should also be predefined by a base implementation. |
| 4 | PluginVersion | Should return the version of the plugin that was loaded. |

> [!IMPORTANT]
> Each of these declarations should accept at least one shared argument, a `Context`.
> This context will be passed by the manager with publicly available data. For example, the the version of the `Plugin Manager API`.

#### Ex0-Dx3122 Plugin Interface <a id="ex0-dx3122"></a>
These protocols are items that the application should expect a plugin module to have available. If unavailable, the module should be considered invalid, and promptly dropped by the manager.

**Plugin Interface Protocols** <a id="ex0-dx3122-Tx0"></a>
| Ref | Identity | Description |
| :---: | --- | --- |
| 0 | PluginDestroy | Destroys the plugin instance. Should be required for a plugin module to be valid. |
| 1 | PluginInstall | Creates an instance of the plugin useable by the manager. |

> [!NOTE]
> The above should define callbacks [^Callback] that can be generated or defined automatically for plugin developers. These will either be common, redundant or otherwise ubiquitous actions.
> Avoiding boilerplate for developers is an important consideration.

#### Ex0-Dx3123 <a id="ex0-dx3123"></a>
These methods define tasks that work at the scope of plugin management. Actual implementation of each action will be handled by some other, specialized, management component.

**Plugin Management Protocols** <a id="ex0-dx3123-Tx0"></a>
| Ref | Identity | Description |
| :---: | --- | --- |
| 0 | LoadArtifacts | Loads plugin artifacts as preinstalled instances of registered plugins |
| 1 | InstallPlugins | Installs plugins from their preloaded artifacts. |
| 2 | DestroyPlugins | Shuts down and destroys plugin objects making them entirely unavailable.
| 3 | StartUpPlugins | Starts all installed plugins. |

## Ex0-Dx3200 Search Path Declaration <a id="ex0-dx3200"></a>
> [!NOTE]
> This feature has not yet been looked into.

## Ex0-Dx3300 Plugin Hash Retrieval <a id="ex0-dx3300"></a>
> [!NOTE]
> This feature has not yet been looked into.

## Ex0-Dx3400 Plugin Version Matching <a id="ex0-dx3400"></a>
> [!NOTE]
> This feature has not yet been looked into.

## Ex0-Dx3500 Plugin Declaration <a id="ex0-dx3500"></a>
> [!NOTE]
> This feature has not yet been looked into.

## Ex0-Dx3600 Hot Reloading <a id="ex0-dx3600"></a>
> [!NOTE]
> This feature has not yet been looked into.

# Ex0-Dxf000 References <a id="ex0-dx4000"></a>
Definitions and references made in this text.

[^Callback]: Callback
A function, procedure or protocol defined/passed from one scope to another to be called from that other scope.
[^DllFault]: DllFault
A failure which occurs when trying to load a module object.
[^Exception]: Exception
A failure in logic that is potentially catastropic to the runtime.
[^Error]: Error
A failure in logic to produce some expected outcome or logic that leads to *exception*.
[^Fault]: Fault
An *error* [^Error] or *exception* [^Exception] occuring at runtime.
[^Hook]: Hook
Some callback [^Callback] used to inject behavior within an event.
[^IHook]: Interface Hook
A hook specific to plugin management for managing a plugin.
[^Module]: Module
A dynamically loaded shared object.
[^Plugin]: Plugin
A library external to the main executable that can be installed and called on by the application.
[^PluginFault]: PluginFault
A failure which occurs within the plugin [^Plugin] implementation.
