# -*- coding: utf-8 -*-
import os
import re
import fnmatch
import argparse
from textwrap import dedent


parser = argparse.ArgumentParser(description='Add/update copyright on C++ files')
parser.add_argument('root', nargs=1, help='Path to the root of the C++ project')
args = parser.parse_args()

# Descend into the 'root' directory and find all *.cs files
files = []
for root, dirnames, filenames in os.walk(args.root[0]):
#for root, dirnames, filenames in os.walk("/d/FDYNAMICS/Neutrino/trunk/neutrino/Code/Core/Base")
    for filename in fnmatch.filter(filenames, "*.cpp"):
		files.append(os.path.join(root, filename))
	for filename in fnmatch.filter(filenames, "*.h"):
		files.append(os.path.join(root, filename))
print "Found {0} *.cpp *.h files".format(len(files))

for filepath in files:
    with open(filepath) as f:
        contents = f.read()

    # This regex will separate the contents of a *.cs file into two parts.
    # The first part is any text that appears before either 'using' or
    # 'namespace' - perhaps an old copyright. The second part *should* be
    # the actual code beginning with 'using' or 'namespace'.
    match = re.search(r"^.*?((using|namespace|/\*|#).+)$", contents, re.DOTALL)
    if match:
        # Make the file's now contain the user defined copyright (below)
        # followed by a blank line followed by the actual code.
        contents = dedent('''\
			// ****************************************************************************
			// <copyright file="{0}" company="Centroid LAB">
			//		Copyright © Centroid LAB 2020
			// 		All Rights Reserved.
			// 
			// 		All information contained herein is, and remains
			// 		the property of Centroid LAB and its suppliers,
			// 		if any.  The intellectual and technical concepts contained
			// 		herein are proprietary to Centroid LAB
			// 		and its suppliers and may be covered by U.S. and Foreign Patents,
			// 		patents in process, and are protected by trade secret or copyright law.
			// 		Dissemination of this information or reproduction of this material
			// 		is strictly forbidden unless prior written permission is obtained
			// 		from Centroid LAB.
            // </copyright>
            // ****************************************************************************
    
            ''').format(os.path.basename(filepath)) + match.group(1)
        with open(filepath, 'w') as f:
            f.write(contents)
    	print "Wrote new: {0}".format(filepath)