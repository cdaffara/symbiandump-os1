/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


Description:
-----------------------------------

The "win_drive" file system extension for the emulator allows you to attach Windows device
(that supports CreateFile/ReadFile/WriteFile operations) to the Symbian OS emulator as the emulated drive.
For example, windows HDD partition, removable drive or a file can be treated by the emulator as "media" mapped
on some emulator's drive letter.

How it works:
-----------------------------------

This extension is installed as a primary one and hooks all emulators media driver's operations redirecting them to 
appropriate windows file IO API.

Why:
-----------------------------------

Existing Symbian OS windows emulator has a quite limited support for its drives:

- emulator's drives suport can be implemented within media driver stacks (like mmc) which is not
  very easy to configure properly. Moreover, using whole driver stacks is not always required and 
  slows down emulator filesystems.

- it is not always easy to attach filesystem image file to the emulator and it's impossible make it 
  to use physical drive (USB drive for example)

- emulated MMC drives can't be bigger than 1G or something, which doesn't make work with FAT32 easy.


How to use:
-----------------------------------
1. make sure that the extension file "win_drive.fxt" is present in the emulator's "Z:\" drive directory:
"\epoc32\release\wins\udeb\" or "\epoc32\release\winscw\udeb\" or "...\urel\", depending on what platform
and release you are using.

2. copy "win_drive.ini" configuration file to: "\epoc32\data\"
3. edit this file to set up emulated drive size, location etc. See inside.

4. Tell the emulator to install "win_drive.fxt" extension to the emulator's drive you are going to use.
   to do this edit appropritae "estart.txt" file, which can be, for example, here: \epoc32\release\wins\udeb\z\sys\DATA\estart.txt
   If there is no such a file, copy the default one from  "\emulator\wins\estart\estart.txt"

   Example:

    [....]
    #X:  1   EFAT32 FAT  0          FS_FORMAT_CORRUPT #<--- previous commented out line
    X:   1   EFAT32 FAT  win_drive  FS_FORMAT_CORRUPT #<--- drive "X:" now has "win_drive" primary extension
    [....]

5. run the emulator, its drive "X:" will be redirected to the device you have set up in the "win_drive.ini" file.



Configuration examples:
-----------------------------------

1. Windows drive F: is a USB cardreader with the SD card you want to use as the emulator's "X:" FAT drive.
Minimal settings in "win_drive.ini" for this case:

=========================
[Drive_X]
DeviceName=\\.\F:
BytesPerSector=0 ;or just comment it out
MediaSizeInSectors=0 ;or just comment it out
=========================

2. You have a spare partition that has "Z:" windows drive letter and you want to use a part of it (to say 1G) as the emulator's "X:" FAT drive.
Minimal settings in "win_drive.ini" for this case:

=========================
[Drive_X]
DeviceName=\\.\Z:
MediaSizeInSectors=2097152
=========================


3. You want to use 8G file as the emulator's "X:" FAT drive. The file doesn't exist.

Minimal settings in "win_drive.ini" for this case:
=========================
[Drive_X]
DeviceName=c:\MyDir\MyImageFile8G.img
MediaSizeInSectors=16777216
=========================


4. You have a FAT volume image file and want to attach it as as the emulator's "X:" FAT drive. You also want to have it read-only.

Minimal settings in "win_drive.ini" for this case: 
=========================
[Drive_X]
DeviceName=c:\MyDir\MyImageFile.img
MediaSizeInSectors=0 ;or just comment it out
ReadOnly = 1;
=========================


Troubleshooting:
-----------------------------------
See "epocwind.out" file if something is going wrong. 



Drawbacks:
-----------------------------------

- you can't use this extension if the emulator's drive alredy uses primary extension (see estart.txt).
- some dodgy tests can fail, for example, those, that dismount the file system and forget to store and mount all its extensions
  include primary.
- Some methods from media driver are not overriden yet and just go directly to the original media driver; it can be confusing.











