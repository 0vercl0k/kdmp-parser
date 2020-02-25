# Axel '0vercl0k' Souchet - February 23 2020
import urllib.request
import os
import sys
import zipfile
import subprocess
import itertools

sln = r'src\kdmp-parser.sln'
testdatas_url = 'https://github.com/0vercl0k/kdmp-parser/releases/download/v0.1/testdatas.zip'

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

    appveyor = os.getenv('APPVEYOR') is not None
    if not appveyor:
        # Build the matrix if not running from AppVeyor.
        for platform, configuration in matrix:
            if msbuild(sln, platform, configuration) != 0:
                print('{0}/{1} build failed, bailing.'.format(platform, configuration))
                return 1

    # Download the test datas off github.
    print('Downloading {0}..'.format(testdatas_url))
    archive_path, _ = urllib.request.urlretrieve(testdatas_url)
    print('Successfully downloaded the test datas in {0}, extracting..'.format(archive_path))

    # Unzip its content in the same temp directory.
    archive_dir, _ = os.path.split(archive_path)
    zipfile.ZipFile(archive_path).extractall(archive_dir)

    # Once we have extracted the archive content, we can delete it.
    os.remove(archive_path)

    # Build full path for both the full / bitmap dumps.
    full = os.path.join(archive_dir, 'full.dmp')
    bmp = os.path.join(archive_dir, 'bmp.dmp')
    dmp_paths = (full, bmp)

    # If we run from AppVeyor we only redefine the matrix as only one flavor.
    if appveyor:
        platform = os.getenv('platform')
        configuration = os.getenv('configuration')
        matrix = (
            (platform, configuration)
        )

    # Now iterate through all the configurations and run every flavor of test.exe against
    # both dumps.
    for dmp_path in dmp_paths:
        for platform, configuration in matrix:
            if test(platform, configuration, dmp_path) != 0:
                print('{0}/{1}/{2} test failed, bailing.'.format(platform, configuration, dmp_path))
                return 1

        # We've run this dump against all the flavor of the test application so we can delete it now.
        os.remove(dmp_path)

    print('All good!')
    return 0

if __name__ == '__main__':
    sys.exit(main())