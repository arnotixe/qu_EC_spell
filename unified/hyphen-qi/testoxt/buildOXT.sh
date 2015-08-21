#!/bin/bash
# ----------------------------------------------------------------------------------
#
#    Shukllachiska Kichwa del Ecuador, OXT packer script
#    Copyright (C) 2009 Arno Teigseth, Henry David Lara
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.
#
#    You should have received a copy of the GNU Affero General Public License
#    along with this program. See LICENSE.txt. If not, see <http://www.gnu.org/licenses/>.
#
#    The author(s) can be contacted at arno at teigseth dot no
#
# ----------------------------------------------------------------------------------
# 
# This script makes an OXT file, ready for distribution. OXT files (*.oxt) are dictionaries, 
# hyphenation files and thesareus files packed together with information that aids openoffice
# in installing the language. Double-clicking the oxt file should be enough to install/update the dictionary.
#
# Pack oxt file

# TODO

CURVER=`cat CURRENTVERSION.txt`

./generateQUdicfile.sh # In case we forgot to regenerate it

echo Creating description.xml
cat description.xml |sed s/%%VERSION%%/$CURVER/ > built-qu_EC-OXT/description.xml

echo README.txt
cat README.txt |sed s/%%VERSION%%/$CURVER/ > built-qu_EC-OXT/README.txt

# Copy up the current version of the AFF
cp qu_EC.aff built-qu_EC-OXT/qu_EC.aff


cd built-qu_EC-OXT

echo Packing
zip -r qu_EC.oxt *

echo Making a copy for historical reasons
cp qu_EC.oxt qu_EC-$CURVER.oxt

echo Archive OXT
mv *.oxt ../archive_OXT

echo Done.


