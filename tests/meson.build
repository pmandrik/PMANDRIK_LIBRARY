
# sudo apt install meson
# cd build
# meson setup builddir ../tests/
# meson compile

project('tests', 'cpp')
add_project_arguments('-DNO_CERN_ROOT=1', language : 'cpp')

incdir = include_directories('../')

executable('test_trees', '../tests/test_trees.cpp', include_directories : incdir, link_args : '-Wunused-variable', cpp_args : '-Wfatal-errors')
executable('test_drawer', '../tests/test_drawer.cpp', include_directories : incdir, link_args : '-Wunused-variable', cpp_args : '-Wfatal-errors')
