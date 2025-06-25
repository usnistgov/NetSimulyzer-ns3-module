# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass


def configure(conf):
    conf.define("HAS_NETSIMULYZER", 1, True, 'Has netsimulyzer module')
    # `waf` was only ever used pre ns-3.41,
    # so we always flag for compatability here
    conf.define("NETSIMULYZER_PRE_NS3_41_ENUM_VALUE", 1, True, 'Use pre ns-3.41 EnumValue compatability')

    # Waf is not used post ns-3.36
    # and we only have compatibility checks for < ns-3.40
    conf.define("NETSIMULYZER_NS3_VERSION", 36, True, 'Version of ns-3 the NetSimulyzer is build for')
    conf.env['HAS_NETSIMULYZER'] = True
    conf.env.append_value('CXXFLAGS', '-std=c++17')


def build(bld):
    module = bld.create_ns3_module('netsimulyzer', ['buildings', 'core', 'network', 'mobility', 'point-to-point'])

    module.source = [
        'helper/area-helper.cc',
        'helper/building-configuration-container.cc',
        'helper/building-configuration-helper.cc',
        'helper/logical-link-helper.cc',
        'helper/node-configuration-container.cc',
        'helper/node-configuration-helper.cc',
        'model/node-configuration.cc',
        'model/building-configuration.cc',
        'model/category-axis.cc',
        'model/category-value-series.cc',
        'model/color.cc',
        'model/color-palette.cc',
        'model/decoration.cc',
        'model/ecdf-sink.cc',
        'model/log-stream.cc',
        'model/logical-link.cc',
        'model/netsimulyzer-version.cc',
        'model/orchestrator.cc',
        'model/rectangular-area.cc',
        'model/series-collection.cc',
        'model/state-transition-sink.cc',
        'model/value-axis.cc',
        'model/xy-series.cc',
        'model/throughput-sink.cc'
        ]

    module_test = bld.create_ns3_module_test_library('netsimulyzer')
    module_test.source = [
        ]

    headers = bld(features='ns3header')
    headers.module = 'netsimulyzer'
    headers.source = [
        'helper/area-helper.h',
        'helper/building-configuration-container.h',
        'helper/building-configuration-helper.h',
        'helper/logical-link-helper.h',
        'helper/node-configuration-container.h',
        'helper/node-configuration-helper.h',
        'library/json.hpp',
        'model/event-message.h',
        'model/log-stream.h',
        'model/logical-link.h',
        'model/netsimulyzer-3D-models.h',
        'model/netsimulyzer-ns3-compatibility.h',
        'model/netsimulyzer-version.h',
        'model/node-configuration.h',
        'model/optional.h',
        'model/optional-types.h',
        'model/building-configuration.h',
        'model/category-axis.h',
        'model/category-value-series.h',
        'model/color.h',
        'model/color-palette.h',
        'model/decoration.h',
        'model/ecdf-sink.h',
        'model/orchestrator.h',
        'model/rectangular-area.h',
        'model/series-collection.h',
        'model/state-transition-sink.h',
        'model/value-axis.h',
        'model/xy-series.h',
        'model/throughput-sink.h'
        ]

    # Examples are not enabled for waf versions of
    # ns-3, since the API has changed so much
    # If you want to try them, uncomment the lines below
    # if bld.env.ENABLE_EXAMPLES:
    #     bld.recurse('examples')
