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

option(NETSIMULYZER_PRE_NS3_41_ENUM_VALUE "Force compatibility with ns-3 versions before ns-3.41" OFF)

# ns-3.41 is tagged at e5039092b3bb5d7942d973cc1016eaa60bdce51c
#
# Note: This doesn't handle squash merges, or some other
# way out there out of tree stuff
# See: `NETSIMULYZER_PRE_NS3_41_ENUM_VALUE` option for that
execute_process(COMMAND "git" "merge-base" "--is-ancestor" "e5039092b3bb5d7942d973cc1016eaa60bdce51c" "HEAD"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE NETSIMULYZER_USE_ENUM_COMPAT
)

# Handling for if `git` is not installed
# Hope this string is well defined...
#
# `128` is the `git` return code for a non-git repo
if (${NETSIMULYZER_USE_ENUM_COMPAT} STREQUAL "no such file or directory" OR ${NETSIMULYZER_USE_ENUM_COMPAT} EQUAL 128)
    # Huge hack for tar ball ns-3 installs
    # If they don't have `grep` either,
    # then I'm out of options
    execute_process(COMMAND "grep" "-w" "EnumValue(int value)" "src/core/model/enum.h"
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            RESULT_VARIABLE NETSIMULYZER_USE_ENUM_COMPAT_GREP)
    # `grep` returns `0` on match
    if (${NETSIMULYZER_USE_ENUM_COMPAT_GREP} EQUAL 0)
        set(NETSIMULYZER_USE_ENUM_COMPAT 1)
    endif ()
endif ()

if (${NETSIMULYZER_PRE_NS3_41_ENUM_VALUE} OR ${NETSIMULYZER_USE_ENUM_COMPAT} EQUAL 1)
    add_compile_definitions(NETSIMULYZER_PRE_NS3_41_ENUM_VALUE)
endif ()


build_lib(
  LIBNAME netsimulyzer
  SOURCE_FILES
    helper/area-helper.cc
    helper/building-configuration-container.cc
    helper/building-configuration-helper.cc
    helper/logical-link-helper.cc
    helper/node-configuration-container.cc
    helper/node-configuration-helper.cc
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
    library/json.hpp
    model/event-message.h
    model/log-stream.h
    model/logical-link.h
    model/netsimulyzer-3D-models.h
    model/netsimulyzer-version.h
    model/node-configuration.h
    model/optional.h
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
  TEST_SOURCES
        test/test-buildings.cc
        test/netsimulyzer-test-utils.h
        test/test-node-events.cc
        test/test-orchestrator-outputs.cc
)

target_compile_definitions(libnetsimulyzer PUBLIC HAS_NETSIMULYZER)

option(NETSIMULYZER_CRASH_HANDLER "Adds a crash handler that writes NetSimulyzer output on an irregular exit of the program" ON)
if (NETSIMULYZER_CRASH_HANDLER)
    add_compile_definitions(NETSIMULYZER_CRASH_HANDLER) # TODO: target_compile_definitions _should_ work here, but it doesn't
endif ()

# This seems to only work with core modules if it's a cached value
set(HAS_NETSIMULYZER TRUE CACHE BOOL "NetSimulyzer ns-3 module")
