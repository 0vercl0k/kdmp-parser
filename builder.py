# Axel '0vercl0k' Souchet - February 23 2020
import urllib.request
import os
import sys
import zipfile
import subprocess
import itertools
import platform
import argparse

os_prefix = '' if 'Windows' in platform.platform(terse = 1) else 'lin'
testdatas_url = 'https://github.com/0vercl0k/kdmp-parser/releases/download/v0.1/testdatas.zip'
vsdevprompt = r'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat'

def source_bat(bat_file, arch):
    '''https://dmerej.info/blog/post/cmake-visual-studio-and-the-command-line/'''
    result = {}
    process = subprocess.Popen(
        f'"{bat_file}" {arch} & set',
        stdout = subprocess.PIPE,
        shell = True
    )

    (out, _) = process.communicate()
    for line in out.splitlines():
        line = line.decode()
        if '=' not in line:
            continue
        line = line.strip()
        key, value = line.split('=', 1)
        result[key] = value

    return result

def build(platform, configuration):
    # Grab the environment needed for the appropriate platform on Windows.
    env = source_bat(vsdevprompt, platform) if os_prefix == '' else os.environ
    dir_name = f'{os_prefix}{platform}-{configuration}'
    build_dir = os.path.join('build', dir_name)
    if not os.path.isdir(build_dir):
        os.mkdir(build_dir)

    # We build an absolute path here because otherwise Ninja ends up creating a 'bin' directory
    # in build\<target>\bin.
    output_dir = os.path.abspath(os.path.join('bin', dir_name))
    if not os.path.isdir(output_dir):
        os.mkdir(output_dir)

    ret = subprocess.call((
        'cmake',
        f'-DCMAKE_RUNTIME_OUTPUT_DIRECTORY={output_dir}',
        f'-DCMAKE_BUILD_TYPE={configuration}',
        '-GNinja',
        os.path.join('..', '..')
    ), cwd = build_dir, env = env)

    if ret != 0: return ret

    ret = subprocess.call((
        'cmake',
        '--build',
        '.'
    ), cwd = build_dir, env = env)

    return ret

def test(arch, configuration, dmp_path):
    dir_name = f'{os_prefix}{arch}-{configuration}'
    bin_dir = os.path.join('bin', dir_name)
    cmd = (
        os.path.join(bin_dir, 'testapp'),
        dmp_path
    )

    print('Launching "{0}"..'.format(' '.join(cmd)))
    return subprocess.call(cmd)

def main():
    parser = argparse.ArgumentParser('Build and run test')
    parser.add_argument('--run-tests', action = 'store_true', default = False)
    parser.add_argument('--configuration', action = 'append', choices = ('Debug', 'RelWithDebInfo'))
    parser.add_argument('--arch', action = 'append', choices = ('x64', 'x86'))
    args = parser.parse_args()

    if args.configuration is None:
        args.configuration = ['Debug', 'RelWithDebInfo']

    if args.arch is None:
        args.arch = ['x64', 'x86']

    matrix = tuple(itertools.product(
        args.arch,
        args.configuration
    ))

    for arch, configuration in matrix:
        if build(arch, configuration) != 0:
            print(f'{arch}/{configuration} build failed, bailing.')
            return 1

    if not args.run_tests:
        return 0

    # Download the test datas off github.
    print(f'Downloading {testdatas_url}..')
    archive_path, _ = urllib.request.urlretrieve(testdatas_url)
    print(f'Successfully downloaded the test datas in {archive_path}, extracting..')

    # Unzip its content in the same temp directory.
    archive_dir, _ = os.path.split(archive_path)
    zipfile.ZipFile(archive_path).extractall(archive_dir)

    # Once we have extracted the archive content, we can delete it.
    os.remove(archive_path)

    # Build full path for both the full / bitmap dumps.
    full = os.path.join(archive_dir, 'full.dmp')
    bmp = os.path.join(archive_dir, 'bmp.dmp')
    dmp_paths = (full, bmp)

    # Now iterate through all the configurations and run every flavor of test.exe against
    # both dumps.
    for dmp_path in dmp_paths:
        for arch, configuration in matrix:
            if test(arch, configuration, dmp_path) != 0:
                print(f'{arch}/{configuration}/{dmp_path} test failed, bailing.')
                return 1

        # We've run this dump against all the flavor of the test application so we can delete it now.
        os.remove(dmp_path)

    print('All good!')
    return 0

if __name__ == '__main__':
    sys.exit(main())