project('42sh', 'c')
all_sources = []
subdir('src')
executable('42sh', all_sources, include_directories: 'src', c_args : ['-std=c99',  '-pedantic',  '-Werror', '-Wall', '-Wextra', '-Wvla'])
