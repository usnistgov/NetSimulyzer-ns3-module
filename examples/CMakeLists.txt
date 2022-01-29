build_lib_example(
  NAME application-state-trace-example-netsimulyzer
  SOURCE_FILES application-state-trace-example.cc
  LIBRARIES_TO_LINK
    ${libnetsimulyzer}
    ${libcore}
    ${libapplications}
)

build_lib_example(
  NAME ecdf-sink-example-netsimulyzer
  SOURCE_FILES ecdf-sink-example-netsimulyzer.cc
  LIBRARIES_TO_LINK
    ${libnetsimulyzer}
    ${libapplications}
    ${libcore}
    ${libinternet}
    ${libmobility}
    ${libnetwork}
    ${libpoint-to-point}
)

build_lib_example(
  NAME mobility-buildings-example-netsimulyzer
  SOURCE_FILES mobility-buildings-example.cc
  LIBRARIES_TO_LINK
    ${libnetsimulyzer}
    ${libcore}
    ${libmobility}
    ${libnetwork}
    ${libbuildings}
)

build_lib_example(
  NAME lena-radio-link-failure-netsimulyzer
  SOURCE_FILES lena-radio-link-failure-netsimulyzer.cc
  LIBRARIES_TO_LINK
    ${libnetsimulyzer}
    ${libcore}
    ${libmobility}
    ${libnetwork}
    ${liblte}
)

build_lib_example(
  NAME outdoor-random-walk-example-netsimulyzer
  SOURCE_FILES outdoor-random-walk-example-netsimulyzer.cc
  LIBRARIES_TO_LINK
    ${libnetsimulyzer}
    ${libcore}
    ${libmobility}
    ${libnetwork}
    ${libbuildings}
)

build_lib_example(
  NAME throughput-sink-example-netsimulyzer
  SOURCE_FILES throughput-sink-example-netsimulyzer.cc
  LIBRARIES_TO_LINK
    ${libnetsimulyzer}
    ${libcore}
    ${libmobility}
    ${libnetwork}
    ${libinternet}
    ${libapplications}
    ${libpoint-to-point}
)

build_lib_example(
  NAME wifi-bianchi-netsimulyzer
  SOURCE_FILES wifi-bianchi-netsimulyzer.cc
  LIBRARIES_TO_LINK
    ${libnetsimulyzer}
    ${libcore}
    ${libmobility}
    ${libnetwork}
    ${libwifi}
)
