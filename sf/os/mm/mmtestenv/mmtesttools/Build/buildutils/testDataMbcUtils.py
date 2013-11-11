
# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#
"""Data used by testMbcUtils - give as python strings etc. Separated out to make the test code easier to understand"""

# the following is taken from misc.mbc. Contains all features we seem to use. Specific contents not really important
import1 = r"""
// Misc.mbc
//
// Copyright (c) Symbian Software Ltd 2004 - 2008.  All rights reserved.
//
// non-optional modules for misc components

SECTION_DIRS

..\..\ECam\framework\group
..\..\ECam\plugins\group
..\..\mmcommon\group
..\..\openmax\il\group
..\..\mdf\group
..\..\mobiletv\hai\dvbh\group
..\..\a3f\traces\group
..\..\a3f\refmmrc\group
..\..\a3f\a3fserverstart\group
..\..\a3f\a3fdevsound\group
..\..\a3f\acf\group
..\..\a3f\acl\group
..\..\a3f\refacladaptation\group
..\..\a3f\devsoundadaptationinfo\group
..\..\packetvideo\group
..\..\3gplibrary\group

SECTION_OPTIONALDIRS

..\..\xvidpu\group

SECTION_COMMANDS

// oneoff - TImageViewer
oneoff ..\..\ICL\group abld -k test export
oneoff ..\..\ICL\group abld -k test build arm4 timageviewer
oneoff ..\..\ICL\group abld -k test build winscw timageviewer
"""

result1 = (
None, # will be filled in runtime with the filename
['..\\..\\ECam\\framework\\group',
'..\\..\\ECam\\plugins\\group',
'..\\..\\mmcommon\\group',
'..\\..\\openmax\\il\\group',
'..\\..\\mdf\\group',
'..\\..\\mobiletv\\hai\\dvbh\\group',
'..\\..\\a3f\\traces\\group',
'..\\..\\a3f\\refmmrc\\group',
'..\\..\\a3f\\a3fserverstart\\group',
'..\\..\\a3f\\a3fdevsound\\group',
'..\\..\\a3f\\acf\\group',
'..\\..\\a3f\\acl\\group',
'..\\..\\a3f\\refacladaptation\\group',
'..\\..\\a3f\\devsoundadaptationinfo\\group',
'..\\..\\packetvideo\\group',
'..\\..\\3gplibrary\\group'],
['..\\..\\xvidpu\\group'],
[('..\\..\\ICL\\group', 'abld -k test export'),
('..\\..\\ICL\\group', 'abld -k test build arm4 timageviewer'),
('..\\..\\ICL\\group', 'abld -k test build winscw timageviewer')])

# import2 is a list of three possible files - misc.mbc is actually curtailed. miscopt.mbc is just comment
import2 = [
r"""// Misc.mbc
//
// Copyright (c) Symbian Software Ltd 2004 - 2008.  All rights reserved.
//
// non-optional modules for misc components

SECTION_DIRS

..\..\ECam\framework\group
..\..\ECam\plugins\group
..\..\mmcommon\group
..\..\openmax\il\group""",
r"""// MiscOpt.mbc
//
// Copyright (c) Symbian Software Ltd 2004 - 2007.  All rights reserved.
//
// Optional misc components

SECTION_DIRS

//..\..\mm3plane\mm-tech\mmrc\mmrcfw\group""",
r"""// icl.mbc
//
// Copyright (c) Symbian Software Ltd 2004 - 2007.  All rights reserved.
//
// non-optional ICL modules

SECTION_DIRS

..\..\ICL\group
..\..\ICL\plugins\group
"""]

result2 = [
    (None, ['..\\..\\ECam\\framework\\group',
    '..\\..\\ECam\\plugins\\group',
    '..\\..\\mmcommon\\group',
    '..\\..\\openmax\\il\\group'], [], []),
    (None, [], [], []),
    (None, ['..\\..\\ICL\\group', '..\\..\\ICL\\plugins\\group'], [], [])]

badImport1 = r"""

SECTION_DIRS
this is a bad dir
"""

badImport2 = r"""

SECTION_COMMANDS

// oneoff - TImageViewer
oneoffbad ..\..\ICL\group abld -k test export
"""

# .mbc file used for intTest
intTestMbcFile = r"""
// intTestMbc

SECTION_DIRS

.\x
.\y

SECTION_OPTIONALDIRS

.\z
"""

testFolderList1 = [
    (False, None, "Hello there"),
    (True, None, "Something missing"),
    (True, "foo", "XX"),
    (True, "bar", "XX"),
    ]

testXmlFile1 = """<SystemDefinition name="BLAH" schema="2.0.0">
  <systemModel>
    <layer name="NEW_CUSTOM_LAYER">
      <collection name="Fake Collection">
        <component name="Fake Multimedia">
          <!-- Something missing -->
          <unit bldFile="foo" /><!-- XX -->
          <unit bldFile="bar" /><!-- XX -->
        </component>
      </collection>
    </layer>
  </systemModel>
</SystemDefinition>
"""
