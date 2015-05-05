#!/usr/bin/env python

import os
from xml.etree import ElementTree as ET

mypath = "."

def is_xml(filename):
    filename = filename.strip()
    return "xml" == filename[-3:]

(total, oks, kos) = (0, 0, 0)
for (dirpath, dirnames, filenames) in os.walk(mypath):
    for filename in filenames:
        total += 1
        if not is_xml(filename):
            continue
        fpath = os.path.join(dirpath, filename)

        try:
            ET.parse(fpath)
            oks = oks + 1
#            print fpath + ": OK"

#        except ET.ParseError, e:
        except Exception, e:
            kos = kos + 1
            print fpath + ": " + str(e)
print "Total: %d  Correct: %d With Errors: %d" % (total, oks, kos)
