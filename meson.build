project('vagabond', 'cpp', 'c')

links = meson.get_cross_property('links', [])
compile_args = meson.get_cross_property('compile_args', [])
glm = dependency('glm', required: false)
gl = dependency('GL', method : 'system', required: false)
curl = dependency('libcurl')
sdl2 = dependency('SDL2', required: false)
sdl2image = dependency('SDL2_image', required: false)

fftw_static = true
if (links == [])
  fftw_static = false
endif

fftw = dependency('fftw3f', static: fftw_static)
deps = [glm, fftw]
name = 'index.html'

if (links == [])
  deps += [gl, sdl2, sdl2image, curl]
  name = 'vagabond.gui'
endif

data_dir = get_option('datadir') / 'vagabond'
shaders = data_dir / 'assets' / 'shaders'
images = data_dir / 'assets' / 'images'
geometry = data_dir / 'assets' / 'geometry'
examples = data_dir / 'assets' / 'examples'
dir_base = get_option('prefix')
combined = join_paths(dir_base, data_dir)

install_data(
install_dir : examples)

install_data(
'assets/geometry/standard_geometry.cif',
'assets/geometry/D-ASP.cif',
'assets/geometry/ACY.cif',
'assets/geometry/ATP.cif',
'assets/geometry/ASP.cif',
'assets/geometry/CHX.cif',
'assets/geometry/PO4.cif',
'assets/geometry/CMO.cif',
'assets/geometry/H2S.cif',
'assets/geometry/HEX.cif',
'assets/geometry/IPA.cif',
'assets/geometry/GLY.cif',
'assets/geometry/L2P.cif',
'assets/geometry/LSD.cif',
'assets/geometry/TCI.cif',
'assets/geometry/TYR.cif',
'assets/geometry/NH3.cif',
install_dir : geometry)

install_data(
'assets/shaders_450/axes.vsh',
'assets/shaders_450/axes.fsh',
'assets/shaders_450/color_only.fsh',
'assets/shaders_450/quad.vsh',
'assets/shaders_450/quad.fsh',
'assets/shaders_450/box.vsh',
'assets/shaders_450/box.fsh',
'assets/shaders_450/shade.fsh',
'assets/shaders_450/floating_box.vsh',
'assets/shaders_450/with_matrix.vsh',
'assets/shaders_450/density.vsh',
'assets/shaders_450/density.fsh',
'assets/shaders_450/multiply.fsh',
'assets/shaders_450/point.vsh',
'assets/shaders_450/point.fsh',
'assets/shaders_450/lighting.fsh',
'assets/shaders_450/reflections.fsh',
install_dir : shaders)

install_data(
'assets/images/arrow.png',
'assets/images/axis.png',
'assets/images/blank.png',
'assets/images/box.png',
'assets/images/cartoon.png',
'assets/images/colour_scheme.png',
'assets/images/cross.png',
'assets/images/data_model.png',
'assets/images/density.png',
'assets/images/dot.png',
'assets/images/gel.png',
'assets/images/gel_top.png',
'assets/images/files.png',
'assets/images/happy_face.png',
'assets/images/lemon.png',
'assets/images/line.png',
'assets/images/metadata.png',
'assets/images/misc_data.png',
'assets/images/model.png',
'assets/images/ok_face.png',
'assets/images/orange.png',
'assets/images/palette.png',
'assets/images/paper.jpg',
'assets/images/peak.png',
'assets/images/points.png',
'assets/images/protein.png',
'assets/images/protractor.png',
'assets/images/reflections.png',
'assets/images/ruler.png',
'assets/images/sad_face.png',
'assets/images/some_atoms.png',
'assets/images/tick.png',
'assets/images/tools.png',
'assets/images/torsion.png',
'assets/images/tube_1.5ml.png',
'assets/images/unit_cell.png',
install_dir : images)

subdir('ccp4')
deps += dep_ccp4

hash_script = find_program('get_hash.sh')

out_path = 'commit.h'
gen_src = custom_target('commit-output',
                        input : ['vagabond/commit.h.pre'],
                        output : [out_path],
                        command : [hash_script, '@INPUT@', '@OUTPUT@', 
						combined],
                        build_by_default: true,
                        build_always_stale: true)

subdir('vagabond/utils')
subdir('vagabond/c4x')
subdir('vagabond/core')

subdir('vagabond/gui/elements')
subdir('vagabond/gui')

extrafiles = []

if (links != [])
extrafiles += 'vagabond/gui/print_file.cpp'
endif

if (links == []) # not emscripten
  executable(name, 'vagabond/gui/main.cpp',
  link_with : [gui, core],
  cpp_args: ['-std=c++11'],
  link_args: [links],
  dependencies : deps, 
  install: true)
else # emscripten
  start_cpp = 'vagabond/gui/main.cpp'
  executable(name, extrafiles, start_cpp,
  link_with: [core, gui],
  cpp_args: ['-I/usr/local/include', '-std=c++11', compile_args],
  link_args: [links],
  dependencies : deps, install: true)
endif

if (links == [])
  subdir('vagabond/c4x/tests')
  subdir('vagabond/utils/tests')
  subdir('vagabond/core/tests')
  subdir('vagabond/gui/tests')
endif

