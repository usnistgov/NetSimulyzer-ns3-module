# NIST-developed software is provided by NIST as a public service. You may use,
# copy and distribute copies of the software in any medium, provided that you
# keep intact this entire notice. You may improve,modify and create derivative
# works of the software or any portion of the software, and you may copy and
# distribute such modifications or works. Modified works should carry a notice
# stating that you changed the software and should note the date and nature of
# any such change. Please explicitly acknowledge the National Institute of
# Standards and Technology as the source of the software.
#
# NIST-developed software is expressly provided "AS IS." NIST MAKES NO
# WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
# LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT
# AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE
# OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT
# ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY
# REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF,
# INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY,
# OR USEFULNESS OF THE SOFTWARE.
#
# You are solely responsible for determining the appropriateness of using and
# distributing the software and you assume all risks associated with its use,
# including but not limited to the risks and costs of program errors,
# compliance with applicable laws, damage to or loss of data, programs or
# equipment, and the unavailability or interruption of operation. This
# software is not intended to be used in any situation where a failure could
# cause risk of injury or damage to property. The software developed by NIST
# employees is not subject to copyright protection within the United States.

# ----- User options -----
option(NETSIMULYZER_PRE_NS3_41_ENUM_VALUE "Force compatibility with ns-3 versions before ns-3.41" OFF)
option(NETSIMULYZER_CRASH_HANDLER "Adds a crash handler that writes NetSimulyzer output on an irregular exit of the program" ON)

find_package (Git)
# Find grep, just in case we need it for version detection later
find_program(NETSIMULYZER_GREP "grep")

# ----- ns-3 version detection -----

# Manually specified ns-3 version, no need to autodetect
if (DEFINED ENV{NETSIMULYZER_NS3_VERSION})
    set(NETSIMULYZER_NS3_VERSION "$ENV{NETSIMULYZER_NS3_VERSION}" CACHE STRING "Detected ns-3 version")

    # Strip the "ns-3." off of the version number
    string(REPLACE "ns-3." "" NETSIMULYZER_NS3_VERSION "${NETSIMULYZER_NS3_VERSION}")

    if (NOT NETSIMULYZER_NS3_VERSION MATCHES "^[0-9]+$")
        message(FATAL_ERROR "Environment variable `NETSIMULYZER_NS3_VERSION` in incorrect format, got `$ENV{NETSIMULYZER_NS3_VERSION}` expected: `ns-3.xx` e.g. `ns-3.45` or just `45`")
    endif ()

# We're on a fixed ns-3 release
elseif (DEFINED NS3_VER AND NOT "${NS3_VER}" STREQUAL "3-dev")
    set(NETSIMULYZER_NS3_VERSION "${NS3_VER}" CACHE STRING "Detected ns-3 version")
    string(REPLACE "3." "" NETSIMULYZER_NS3_VERSION "${NETSIMULYZER_NS3_VERSION}")

# Not manually specified ns-3 version, try and detect from git tags
elseif (GIT_FOUND)
    # Get ns-3 version tags
    # Hopefully not muddied by other project versions
    execute_process(COMMAND "git" "tag" "--list" "ns-3.*" "--sort" "-version:refname"
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE NETSIMULYZER_DETECTED_NS3_VERSIONS
            COMMAND_ECHO STDERR
    )

    string(REPLACE "ns-3." "" NETSIMULYZER_DETECTED_NS3_VERSIONS "${NETSIMULYZER_DETECTED_NS3_VERSIONS}")
    # Convert to CMake list & sort
    # so the highest number is first
    # Use `NATURAL` sorting to handle double digit numbers and decimals
    string(REPLACE "\n" ";" NETSIMULYZER_DETECTED_NS3_VERSIONS "${NETSIMULYZER_DETECTED_NS3_VERSIONS}")
    list(SORT NETSIMULYZER_DETECTED_NS3_VERSIONS COMPARE NATURAL ORDER DESCENDING)
    list(GET NETSIMULYZER_DETECTED_NS3_VERSIONS 0 NETSIMULYZER_DETECTED_NS3_VERSION)

    set(NETSIMULYZER_NS3_VERSION "${NETSIMULYZER_DETECTED_NS3_VERSION}" CACHE STRING "Detected ns-3 version")
elseif (NETSIMULYZER_GREP)
    # Last resort version detection
    # Huge hack for tar ball ns-3 installs
    # If they don't have `grep` either,
    # then I'm out of options
    #
    # grep 'deprecated.h' for the highest number we can get
    execute_process(COMMAND "grep" "--only-matching" "--extended-regexp" "NS_DEPRECATED_3_[0-9]+" "src/core/model/deprecated.h"
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            OUTPUT_VARIABLE NETSIMULYZER_DEPRECATED_GREP)

    # Strip the 'NS_DEPRECATED_3_' off of the macros
    # so we should have something like: "45\n,44\n43..."
    string(REPLACE "NS_DEPRECATED_3_" "" NETSIMULYZER_DEPRECATED_GREP "${NETSIMULYZER_DEPRECATED_GREP}")
    # Convert to CMake list & sort
    # so the highest number is first
    # Use `NATURAL` sorting to handle double digit numbers and decimals
    string(REPLACE "\n" ";" NETSIMULYZER_DEPRECATED_GREP "${NETSIMULYZER_DEPRECATED_GREP}")
    list(SORT NETSIMULYZER_DEPRECATED_GREP COMPARE NATURAL ORDER DESCENDING)
    list(GET NETSIMULYZER_DEPRECATED_GREP 0 NETSIMULYZER_DETECTED_NS3_VERSION)

    set(NETSIMULYZER_NS3_VERSION "${NETSIMULYZER_DETECTED_NS3_VERSION}" CACHE STRING "Detected ns-3 version")
endif ()


# Check for minor releases, e.g. 46.1
# We bin them in with the major release, but set an additional
# variable, just in case
if (NETSIMULYZER_NS3_VERSION MATCHES "[0-9]+\.[0-9]+") # Matches [number].[number]
    string(REPLACE "." ";" NETSIMULYZER_NS3_VERSION_SPLIT ${NETSIMULYZER_NS3_VERSION})

    list(GET NETSIMULYZER_NS3_VERSION_SPLIT 0 NETSIMULYZER_NS3_VERSION)
    list(GET NETSIMULYZER_NS3_VERSION_SPLIT 1 NETSIMULYZER_NS3_VERSION_MINOR)
endif ()



if (NOT NETSIMULYZER_NS3_VERSION AND NOT NETSIMULYZER_NS3_VERSION MATCHES "^[0-9]+$")
    message(FATAL_ERROR "NetSimulyzer: Failed to detect ns-3 version, please specify the `NETSIMULYZER_NS3_VERSION` environment variable in the form: 'ns-3.xx' e.g 'ns-3.45'")
endif ()

add_compile_definitions(NETSIMULYZER_NS3_VERSION=${NETSIMULYZER_NS3_VERSION})

if (DEFINED NETSIMULYZER_NS3_VERSION_MINOR)
    add_compile_definitions(NETSIMULYZER_NS3_VERSION_MINOR=${NETSIMULYZER_NS3_VERSION_MINOR})
endif ()

if (${NETSIMULYZER_PRE_NS3_41_ENUM_VALUE} OR ${NETSIMULYZER_NS3_VERSION} LESS 41)
    add_compile_definitions(NETSIMULYZER_PRE_NS3_41_ENUM_VALUE)
endif ()

# ----- Set up model source files -----

build_lib(
  LIBNAME netsimulyzer
  SOURCE_FILES
    helper/area-helper.cc
    helper/building-configuration-container.cc
    helper/building-configuration-helper.cc
    helper/logical-link-helper.cc
    helper/node-configuration-container.cc
    helper/node-configuration-helper.cc
    helper/throughput-sink-helper.cc
    model/node-configuration.cc
    model/building-configuration.cc
    model/category-axis.cc
    model/category-value-series.cc
    model/color.cc
    model/color-palette.cc
    model/decoration.cc
    model/ecdf-sink.cc
    model/log-stream.cc
    model/logical-link.cc
    model/netsimulyzer-version.cc
    model/orchestrator.cc
    model/rectangular-area.cc
    model/series-collection.cc
    model/state-transition-sink.cc
    model/value-axis.cc
    model/xy-series.cc
    model/throughput-sink.cc
  HEADER_FILES
    helper/area-helper.h
    helper/building-configuration-container.h
    helper/building-configuration-helper.h
    helper/logical-link-helper.h
    helper/node-configuration-container.h
    helper/node-configuration-helper.h
    helper/throughput-sink-helper.h
    library/json.hpp
    model/event-message.h
    model/log-stream.h
    model/logical-link.h
    model/netsimulyzer-3D-models.h
    model/netsimulyzer-ns3-compatibility.h
    model/netsimulyzer-version.h
    model/node-configuration.h
    model/optional.h
    model/optional-types.h
    model/building-configuration.h
    model/category-axis.h
    model/category-value-series.h
    model/color.h
    model/color-palette.h
    model/decoration.h
    model/ecdf-sink.h
    model/orchestrator.h
    model/rectangular-area.h
    model/series-collection.h
    model/state-transition-sink.h
    model/value-axis.h
    model/xy-series.h
    model/throughput-sink.h
  LIBRARIES_TO_LINK
    ${libbuildings}
    ${libcore}
    ${libnetwork}
    ${libmobility}
    ${libpoint-to-point}
    ${libapplications}
  TEST_SOURCES
        test/test-buildings.cc
        test/netsimulyzer-test-utils.h
        test/test-node-events.cc
        test/test-orchestrator-outputs.cc
)

# ----- Compiler Defines -----

target_compile_definitions(${libnetsimulyzer} PUBLIC HAS_NETSIMULYZER)

if (NETSIMULYZER_CRASH_HANDLER)
    add_compile_definitions(NETSIMULYZER_CRASH_HANDLER) # TODO: target_compile_definitions _should_ work here, but it doesn't
endif ()

# ----- CMake Define for model detection -----
# This seems to only work with core modules if it's a cached value
set(HAS_NETSIMULYZER TRUE CACHE BOOL "NetSimulyzer ns-3 module")
