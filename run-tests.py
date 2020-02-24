# Axel '0vercl0k' Souchet - February 23 2020
import urllib.request
import os
import sys
import zipfile
import subprocess
import itertools

sln = r'src\kdmp-parser.sln'
testdatas_url = ''

def msbuild(sln, platform, configuration):
    cmd = (
        'msbuild',
        '/t:Build',
        '/p:Platform={0};Configuration={1}'.format(platform, configuration),
        sln
    )

    return subprocess.call(cmd)

def test(platform, configuration, dmp_path):
    dir_path = os.path.join(platform, configuration)
    if platform == 'x86':
        dir_path = configuration

    cmd = (
        os.path.join('src', dir_path, 'test.exe'),
        dmp_path
    )

    print('Launching "{0}"..'.format(' '.join(cmd)))
    return subprocess.call(cmd)

def main():
    matrix = (
        ('x64', 'Release'),
        ('x64', 'Debug'),
        ('x86', 'Release'),
        ('x86', 'Debug')
    )

    for platform, configuration in matrix:
        if msbuild(sln, platform, configuration) != 0:
            print('{0}/{1} build failed, bailing.'.format(platform, configuration))
            return 1

    # Download the test datas off github.
    archive_path, _ = urllib.request.urlretrieve(testdatas_url)
    print('Successfully downloaded the test datas in {0}, extracting..'.format(archive_path))
    archive_dir, _ = os.path.split(archive_path)
    zipfile.ZipFile(archive_path).extractall(archive_dir)
    full = os.path.join(archive_dir, 'full.dmp')
    bmp = os.path.join(archive_dir, 'bmp.dmp')
    dmp_paths = (full, bmp)
    for dmp_path in dmp_paths:
        for platform, configuration in matrix:
            if test(platform, configuration, dmp_path) != 0:
                print('{0}/{1}/{2} test failed, bailing.'.format(platform, configuration, dmp_path))
                return 1

        os.remove(dmp_path)

    print('All good!')
    return 0

if __name__ == '__main__':
    sys.exit(main())