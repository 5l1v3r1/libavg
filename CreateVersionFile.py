#!/usr/bin/env python
# -*- coding: utf-8 -*-
# libavg - Media Playback Engine.
# Copyright (C) 2003-2012 Ulrich von Zadow
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Current versions can be found at www.libavg.de
#

# The script generates a header file that contains versioning data for
# the current build.

import os
import sys
import subprocess
import errno
import re
import socket
import getpass
import platform
import datetime
import pickle

CHECK_FIELDS = ('releaseVersion', 'revision', 'branchurl')
OUTPUT_TEMPLATE = '''// version.h
// This file is automatically generated by CreateVersionFile.py

#define AVG_VERSION_RELEASE     "%(releaseVersion)s"
#define AVG_VERSION_FULL        "%(fullVersion)s"
#define AVG_VERSION_BRANCH_URL  "%(branchurl)s"
#define AVG_VERSION_BUILDER     "%(builder)s"
#define AVG_VERSION_BUILDTIME   "%(buildtime)s"
#define AVG_VERSION_REVISION    %(revision)s
#define AVG_VERSION_MAJOR       %(major)d
#define AVG_VERSION_MINOR       %(minor)d
#define AVG_VERSION_MICRO       %(micro)d
'''
TOPDIR = os.path.dirname(os.path.abspath(__file__))
INPUT_FILE = os.path.join(TOPDIR, 'm4', 'avg_version.m4')
CACHE_FILE = os.path.join(TOPDIR, 'versioninfo.cache')
OUTPUT_FILENAME = 'version.h'

def err(text):
    print >>sys.stderr, text

def getSvnRevision():
    revision = 0
    
    try:
        process = subprocess.Popen(['svnversion'],
                cwd=TOPDIR,
                stdout=subprocess.PIPE)

        output, discard = process.communicate()
    except OSError, e:
        print e, dir(e)
        if e.errno == errno.ENOENT:
            err('Cannot query current revision number via svninfo: '
                    '"svnversion" executable cannot be found.')
    else:
        match = re.search(r'^(\d+)(?::(\d+))?', output)
        if match:
            if match.group(2):
                revision = match.group(2)
            else:
                revision = match.group(1)
            
    return int(revision)

def getSvnBranch():
    url = ''
    branch = 'exported'
    
    try:
        env = os.environ.copy()
        env['LC_ALL'] = 'C'
        process = subprocess.Popen(['svn', 'info', '.'],
            cwd=TOPDIR,
            stdout=subprocess.PIPE,
            env=env)

        output, discard = process.communicate()
    except OSError, e:
        if e.errno == errno.ENOENT:
            err('Cannot query current branch via svn: '
                    '"svn" executable cannot be found.')
            branch = 'unknown'
    else:
        match = re.search(r'^URL: (.+)$', output, re.M)
        if match:
            url = match.group(1)
            
            if 'svn/trunk' in url:
                branch = 'trunk'
            elif 'svn/branches/' in url:
                match = re.search(r'svn\/branches\/([^\/]+)', url)
                if match:
                    branch = match.group(1)

    return (url, branch)

def getBuilder():
    user = getpass.getuser()
    hostname = socket.gethostname()
    
    return '%s@%s %s' % (user, hostname, platform.platform())

def extractComponentFromM4(text, component):
    match = re.search(r'%s\s*\].*\[\s*(\d+)\s*\]' % component, text, re.M)
    if match:
        return int(match.group(1))
    else:
        err('Cannot identify %s version component in %s' % (component, INPUT_FILE))
        sys.exit(1)
    
def getVersionComponents():
    f = open(INPUT_FILE)
    contents = f.read()
    f.close()
    
    major = extractComponentFromM4(contents, 'VERSION_MAJOR')
    minor = extractComponentFromM4(contents, 'VERSION_MINOR')
    micro = extractComponentFromM4(contents, 'VERSION_MICRO')
    
    return (major, minor, micro)
    
def assembleVersionInfo(major, minor, micro):
    releaseVersion = '%d.%d.%d' % (major, minor, micro)
    revision = getSvnRevision()
    branchurl, branch = getSvnBranch()
    builder = getBuilder()
    buildtime = datetime.datetime.now().isoformat()
    
    if revision and branch:
        fullVersion = '%s-%s/r%s' % (releaseVersion, branch, revision)
    elif revision:
        fullVersion = '%s-r%s' % (releaseVersion, revision)
    else:
        fullVersion = releaseVersion

    return locals()

def dumpVersionInfo(versionInfo):
    for k, v in versionInfo.iteritems():
        print '  %s: %s' % (k, v)

def hasChanged(versionInfo):
    try:
        cachef = open(CACHE_FILE)
    except IOError:
        return True
    
    try:
        cachedVersionInfo = pickle.load(cachef)
    except Exception, e:
        err('Corrupted %s file, forcing rewrite (%s)' % (CACHE_FILE, str(e)))
        cachef.close()
        return True
    
    cachef.close()

    for field in CHECK_FIELDS:
        if versionInfo.get(field) != cachedVersionInfo.get(field):
            return True
    
    return False
    
def writeVersionHeader(versionInfo, originalFile):
    outf = open(originalFile, 'w')
    outf.write(OUTPUT_TEMPLATE % versionInfo)
    outf.close()
    
    try:
        cachef = open(CACHE_FILE, 'w')
    except IOError:
        pass
    else:
        pickle.dump(versionInfo, cachef)
        cachef.close()

def main(topBuildDir=TOPDIR):
    versionInfo = assembleVersionInfo(*getVersionComponents())
    outputFile = os.path.join(topBuildDir, OUTPUT_FILENAME)
    
    # Avoid to write again if the content hasn't significantly changed
    if not os.path.exists(outputFile) or hasChanged(versionInfo):
        dumpVersionInfo(versionInfo)
        writeVersionHeader(versionInfo, outputFile)


if __name__ == '__main__':
    if len(sys.argv) == 2:
        main(sys.argv[1])
    elif len(sys.argv) == 1:
        main()
    else:
        err('%s [top build dir]' % sys.argv[0])
        sys.exit(1)
