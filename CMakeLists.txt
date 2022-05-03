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

build_lib(
  LIBNAME netsimulyzer
  SOURCE_FILES
    helper/building-configuration-container.cc
    helper/building-configuration-helper.cc
    helper/node-configuration-container.cc
    helper/node-configuration-helper.cc
    model/node-configuration.cc
    model/building-configuration.cc
    model/category-axis.cc
    model/category-value-series.cc
    model/color.cc
    model/decoration.cc
    model/ecdf-sink.cc
    model/log-stream.cc
    model/netsimulyzer-version.cc
    model/orchestrator.cc
    model/rectangular-area.cc
    model/series-collection.cc
    model/state-transition-sink.cc
    model/value-axis.cc
    model/xy-series.cc
    model/throughput-sink.cc
  HEADER_FILES
    helper/building-configuration-container.h
    helper/building-configuration-helper.h
    helper/node-configuration-container.h
    helper/node-configuration-helper.h
    library/json.hpp
    model/event-message.h
    model/log-stream.h
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
)

target_compile_definitions(libnetsimulyzer PUBLIC HAS_NETSIMULYZER)
