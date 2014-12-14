#!/usr/bin/env python
import fnmatch
import os
import re
from termcolor import colored

forbidden_boost_headers = [ "foreach", "bind", "lambda/lambda", "shared_ptr", "regex", "tuple/tuple" ]

def check_for_string( line, str ):
    if re.search( str, line ):
        suffix = colored( ' [ignored]', 'yellow', attrs = ['bold'] ) if line.endswith( "//:QA_IGN\n" ) else ""
        print( "{0} File {1} contains {2}{3}".format( colored( '[E]', 'red', attrs = ['bold'] ), colored( name, 'green' ), colored( str, 'blue' ), suffix ) )
        return True
    return False

def check_file( name ):
    found = False
    with open( name, "r" ) as f:
        for line in f:
            for header in forbidden_boost_headers:
                found = found or check_for_string( line, "boost/%s.hpp" % header )
            found = found or check_for_string( line, "shared_ptr\(.*new" )
            found = found or check_for_string( line, "::ptr\(.*new" )
            found = found or check_for_string( line, "typedef" )
    return not found

count = 0; total = 0
for _dir in [".", "addons/cirkit-addon-formal", "addons/cirkit-addon-mini", "addons/cirkit-addon-revlib", "addons/cirkit-addon-experimental", "addons/cirkit-addon-verific", "addons/cirkit-addon-yosys"]:
    for top in ["src", "programs", "test"]:
        for root, dirnames, filenames in os.walk( "{0}/{1}".format( _dir, top ) ):
            for filename in fnmatch.filter( filenames, "*.?pp" ):
                name = os.path.join( root, filename )
                if not check_file( name ):
                    count += 1
                total += 1

print( "{0} {1} from {2} files have some code issues.".format( colored( '[I]', 'white', attrs = ['bold'] ), count, total ) )