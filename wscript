# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass


def configure(conf):
    conf.define("HAS_VISUALIZER3D", 1, True, 'Has visualizer3d module')
    conf.env['HAS_VISUALIZER3D'] = True


def build(bld):
    module = bld.create_ns3_module('visualizer3d', ['buildings', 'core', 'mobility'])

    module.source = [
        'helper/building-configuration-container.cc',
        'helper/building-configuration-helper.cc',
        'helper/node-configuration-container.cc',
        'helper/node-configuration-helper.cc',
        'model/node-configuration.cc',
        'model/building-configuration.cc',
        'model/category-axis.cc',
        'model/category-value-series.cc',
        'model/color.cc',
        'model/decoration.cc',
        'model/log-stream.cc',
        'model/orchestrator.cc',
        'model/rectangular-area.cc',
        'model/series-collection.cc',
        'model/state-transition-sink.cc',
        'model/value-axis.cc',
        'model/xy-series.cc',
        'model/throughput-sink.cc',
        ]

    module_test = bld.create_ns3_module_test_library('visualizer3d')
    module_test.source = [
        ]

    headers = bld(features='ns3header')
    headers.module = 'visualizer3d'
    headers.source = [
        'helper/building-configuration-container.h',
        'helper/building-configuration-helper.h',
        'helper/node-configuration-container.h',
        'helper/node-configuration-helper.h',
        'library/json.hpp',
        'model/event-message.h',
        'model/log-stream.h',
        'model/node-configuration.h',
        'model/building-configuration.h',
        'model/category-axis.h',
        'model/category-value-series.h',
        'model/color.h',
        'model/decoration.h',
        'model/orchestrator.h',
        'model/rectangular-area.h',
        'model/series-collection.h',
        'model/state-transition-sink.h',
        'model/value-axis.h',
        'model/xy-series.h',
        'model/throughput-sink.h',
        'model/visualizer3d-variables.h'
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')
