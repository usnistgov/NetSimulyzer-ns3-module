# Table of Contents

* [About](#about)
* [Installation](#installation)
    * [Clone (Recommended)](#clone-recommended)
    * [Download ZIP](#download-zip)
    * [Connecting the Module Quickly](#connecting-the-module-quickly)
    * [Connecting the Module Safely](#connecting-the-module-safely)
        * [Waf (Linking)](#waf-linking)
        * [Code](#code)

# About
This is the [ns-3](https://www.nsnam.org/) companion module the
Visualizer
[GitLab-internal](https://gitlab.nist.gov/gitlab/wnd-publicsafety/visualizer/visualization) or
[GitHub-public](https://github.com/usnistgov/ns3-visualizer3d).
Link this module & run your scenario to see it in 3D.

# Installation
## Clone (Recommended)
Clone the project into a directory called `visualizer3d` in
the `src` folder of a supported version of ns-3

1) `cd` into the `src` folder of `ns-3`

```shell
cd src/
```

2) Clone the project from one of the below URLs

### GitHub (Public)

Use this one if you wish to clone the project outside of the NIST
network
```shell
# Pick one of the below
# HTTPS
git clone https://github.com/usnistgov/ns3-visualizer3d-module.git visualizer3d

# SSH
git clone git@github.com:usnistgov/ns3-visualizer3d-module.git visualizer3d
```


### GitLab (Internal)

Use this one if you're working only on the NIST network
```shell
# Pick one of the below
# HTTPS
git clone https://gitlab.nist.gov/gitlab/wnd-publicsafety/visualizer/visualizer3d.git visualizer3d

# SSH
git clone git@gitlab.nist.gov:wnd-publicsafety/visualizer/visualizer3d.git visualizer3d
```

3) (Re)configure & (Re)build `ns-3`

## Download ZIP
If, for whatever reason, `git` is not available. Just download the
project & unzip it into the `src` directory of `ns-3`.

Note that updates will have to be performed manually using this method

1) Download the ZIP of the project from one of the below URLs

### GitHub (Public)
https://github.com/usnistgov/ns3-visualizer3d-module/archive/master.zip

### GitLab (Internal)
https://gitlab.nist.gov/gitlab/wnd-publicsafety/visualizer/visualizer3d/-/archive/master/visualizer3d-master.zip

2) Unzip the file into the `ns-3` `src/` directory
### GitHub
```shell
unzip ns3-visualizer3d-module-master.zip
```

### GitLab
```shell
unzip visualizer3d-master
```

3) Rename the resulting directory to `visualizer3d` (for consistency)
### GitHub
```shell
mv ns3-visualizer3d-module-master visualizer3d
```

### GitLab
```shell
mv visualizer3d-master visualizer3d
```

## Connecting the Module Quickly
If you are linking your module/program to the `visualizer3d` module add the following to your `wscript`

```python
# Program
obj = bld.create_ns3_program('program-name', ['visualizer3d', '''...'''])


# Module
module = bld.create_ns3_module('module-name', ['visualizer3d', '''...'''])
```

You may now include & use the `visualizer3d` module in code:
```cpp
#include <ns3/visualizer3d-module.h>
//...

int main ()
{
    // ...
    auto orchestrator = CreateObject<visualizer3d::Orchestrator> ("example.json");
    // ...
}
```

## Connecting the Module Safely
You may wish for your module to not have a hard dependency on the `visualizer3d` module.
The following steps will allow you to link the module & still allow your code to build &
run without 

### Waf (Linking)
If you wish for your module/program to be able to build without the `visualizer3d` module
you may check for it's existence by reading `bld.env['HAS_VISUALIZER3D']` in your `wscript`. See below:

```python
def build(bld):
    # Create a list of your required modules to link
    # 'core' & 'mobility' are just examples here
    linked_modules = ['core', 'mobility']

    # Check if 'HAS_VISUALIZER3D' was defined during configuration
    if 'HAS_VISUALIZER3D' in bld.env:
        # If it was defined, then the 'visualizer3d' is presant and we may link it
        linked_modules.append('visualizer3d')

    # Be sure to pass your list of `linked_modules` to `create_ns3_program`
    # or `create_ns3_module`
    obj = bld.create_ns3_program('application-name', linked_modules)
```

### Code
In addition to the variable in the build environment, the module also defines a C++ macro
also named `HAS_VISUALIZER3D`. This macro may be used in C++ code to check for the presence
of the `visualizer3d` module.

See the below code sample:
```cpp
// Guard the include with the macro
#ifdef HAS_VISUALIZER3D
#include <ns3/visualizer3d-module.h>
#endif

// ...

int main ()
{
    // ...

    // Guard any visualizer references in code with the macro as well
#ifdef HAS_VISUALIZER3D
    auto orchestrator = CreateObject<visualizer3d::Orchestrator> ("example.json");
    // ...
#endif
}
```
