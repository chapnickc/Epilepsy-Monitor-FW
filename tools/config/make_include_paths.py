import os
import re

def add_pardirs(dirs, root_dir):
    """
    Recursively adds parent directories to a set
    """
    out_dirs = set(dirs)

    for path in dirs:
        p = path
        par_dir = os.path.dirname(p)

        # path might be equal to root_path, so check
        at_root = par_dir in out_dirs or p == root_dir

        while not at_root:
            p = par_dir
            at_root = p in out_dirs or p == root_dir

            out_dirs.add(p)
            par_dir = os.path.dirname(p)

    return list(out_dirs)


def find_sources(dir_path, file_ext = '.h'):
    """
    Finds all directories of a library which contains header
    files and returns the full path of each directory
    """
    # use a set to prevent duplicates
    dir_path = os.path.abspath(dir_path)

    header_dirs = set()

    # make sure to include the parent dir
    header_dirs.add(dir_path)

    for path, dirs, files in list(os.walk(dir_path)):
        path = os.path.abspath(path)
        h_matches = [f.endswith(file_ext) for f in files]
        if any(h_matches):
            header_dirs.add(path)

    return header_dirs


def compute_path(src_var, lib_path, h_path):
    """
    Computes the path(s) that will be added
    as arguments to include_dirs in CMakeLists.txt.
    Creates paths to parent directories, as necessary and
    appends relevant h_path suffixes
    """
    _, suffix = re.split(lib_path, h_path)
    out_path = r'${' + src_var + r'}' + suffix
    return out_path

if __name__ == '__main__':

    from make_include_paths import *

    # Note: Use the absolute path (from system root dir) rather than '~' for home
    cwd = os.getcwd()
    lib_paths = {
            'MBED_SRC_PATH': os.path.abspath(cwd  + '/../../lib/mbed'),
            'BLE_API_SRC_PATH' :os.path.abspath(cwd  + '/../../lib/BLE_API'),
            'NRF51822_SRC_PATH2' : os.path.abspath(cwd  + '/../../lib/nRF51822'),
            'NRF51822_SRC_PATH' : os.path.abspath(cwd + '/../../lib/nRF51822/nrf51-sdk/source/nordic_sdk/components')
            }


    all_paths = []
    for src_var, lib_path in lib_paths.items():
        header_paths = find_sources(lib_path)


        header_paths = add_pardirs(header_paths, lib_path)
        header_paths.sort(key = len)

        for h_path in header_paths:
            out_path = compute_path(src_var, lib_path, h_path)
            all_paths.append(out_path)
            print (out_path)


    with open('include_paths.txt', 'w') as f:
        for p in all_paths:
            f.write(p + '\n')
