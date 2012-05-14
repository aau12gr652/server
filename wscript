#! /usr/bin/env python
# encoding: utf-8

import os

# Necessary since we override different Contexts
import waflib.extras.wurftools as wt

APPNAME = 'kodo_encode_decode_simple'
VERSION = '0.1'


wt.add_dependency('kodo', 'git://github.com/steinwurf/kodo.git', '1.0.3')

wt.add_dependency('sak', 'git://github.com/steinwurf/sak.git', '1.0.0')

wt.add_dependency('fifi', 'git://github.com/steinwurf/fifi.git', '1.0.1')

wt.add_dependency('boost', 'git://github.com/steinwurf/external-boost.git',
                  '1.1.0-boost_1_48_0')

wt.add_dependency('gtest', 'git://github.com/steinwurf/external-gtest.git',
                  '1.0.1-gtest_1_6_0')

def load_helper(ctx, name):
    if ctx.is_system_dependency(name):
        ctx.fatal('Load a tool to find %s as system dependency' % name)
    else:
        ctx.load_dependency(name)

def options(opt):
    opt.load('wurftools')

    load_helper(opt, 'sak')
    load_helper(opt, 'fifi')
    load_helper(opt, 'boost')
    load_helper(opt, 'kodo')
    load_helper(opt, 'gtest')

def configure(conf):

    conf.load('wurftools')

    load_helper(conf, 'sak')
    load_helper(conf, 'fifi')
    load_helper(conf, 'boost')
    load_helper(conf, 'kodo')
    load_helper(conf, 'gtest')


def build(bld):

    load_helper(bld, 'sak')
    load_helper(bld, 'fifi')
    load_helper(bld, 'boost')
    load_helper(bld, 'kodo')
    load_helper(bld, 'gtest')

    cxxflags = []

    if bld.env.TOOLCHAIN == 'linux':
        cxxflags += ['-O2', '-g', '-ftree-vectorize',
                     '-Wextra', '-Wall']

    if bld.env.TOOLCHAIN == 'darwin':
        cxxflags += ['-O2', '-g', '-ftree-vectorize']

    if bld.env.TOOLCHAIN == 'win32':
        cxxflags += ['/O2', '/Ob2', '/W3', '/EHsc']

    bld.program(features = 'cxx',
                source   = ['uep_test.cpp','../postoffice/Postoffice.cpp','../node/kodo_decoder.cpp', "kodo_encoder.cpp"],
                target   = 'uep_test',
                includes  = ["..",'bundle_dependencies/boost-1.1.0-boost_1_48_0/boost'],
                cxxflags = cxxflags,
                stlib = ['boost_thread-mt','boost_system-mt'],
                use      = ['kodo_includes',
                            'fifi_includes', 'sak_includes'])

#     bld.program(features = 'cxx',
#                 source   = ['server.cpp','../postoffice/Postoffice.cpp', "kodo_encoder.cpp"],
#                 target   = 'server',
#                 includes  = "..",
#                 cxxflags = cxxflags,
#                 use      = ['kodo_includes', 'boost_includes',
#                             'fifi_includes', 'sak_includes'])
