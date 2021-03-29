#!/usr/bin/env python3

import os, subprocess
from os.path import isfile, join
# from chardet import detect

# # get file encoding type
# def get_encoding_type(file):
#     with open(file, 'rb') as f:
#         rawdata = f.read()
#     return detect(rawdata)['encoding']

# def reencode(srcfile):
#     from_codec = get_encoding_type(srcfile)

#     # add try: except block for reliability
#     try:
#         with open(srcfile, 'r', encoding=from_codec) as f, open(trgfile, 'w', encoding='utf-8') as e:
#             text = f.read() # for small files, for big use chunks
#             e.write(text)

#         os.remove(srcfile) # remove old encoding file
#         os.rename(trgfile, srcfile) # rename new encoding
#     except UnicodeDecodeError:
#         print('Decode Error')
#     except UnicodeEncodeError:
#         print('Encode Error')

def run(cmd):
    completed = subprocess.run(["powershell", "-Command", cmd])
    return completed

def reencode(file):
    cmd = 'Get-Content %s | Set-Content -Encoding utf8 %s_utf8' %(file, file)
    run(cmd)
    os.remove(file)
    os.rename('%s_utf8'%file, file) # rename new encoding



def walkDir(path, currentPath = ''):
    print('In %s' % (currentPath + path))
    filesToProcess = []
    for root, dirs, files in os.walk(path):
        for f in files:
            if '.inl' in f:
                filesToProcess.append(os.path.join(root, f))

    for f in filesToProcess:
        print(f)
        reencode(f)
        # for d in dirs:
        #     print('Entering %s' % d)
        #     walkDir(d, currentPath + path + '\\')

def main():

    walkDir('SRC')


main()
