// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Definitions of the classes responsible for the settings handling in the .ini file
// 
//

/**
 @file
*/

#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include "common.h"

#define WIN32_LEAN_AND_MEAN
#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#include <windows.h>
#pragma warning( default : 4201 ) // nonstandard extension used : nameless struct/union


//-----------------------------------------------------------------------------

/** This class is the interface to the various settings from ini file */
class TSettingsManager
{
 public:
    
    static TSettingsManager* Create(TInt aEpocDrvNumber);

    //-- API for the particular user of this class
    TUint32 MediaSectorSize() const;
    TUint32 MediaSizeInSectors() const;
    TBool   WinDeviceName(char* apszName, TUint aBufLen) const;
    TBool   IsReadOnly() const;

    TUint32 TMediaType_Override() const;
    void    MediaAtt_OverrideMasks(TUint32 &aAndMask, TUint32 &aOrMask) const;
    void    DriveAtt_OverrideMasks(TUint32 &aAndMask, TUint32 &aOrMask) const;

 protected:

    TSettingsManager(const char* apFileName, TInt aEpocDrvNumber);

    TSettingsManager();
    TSettingsManager(const TSettingsManager&);
    TSettingsManager& operator=(const TSettingsManager&);

    //-- low-level API, for internal use only
    DWORD   ReadString (const char* aAppName, const char* aKeyName, const char* aStrDefault, char* aStrDest, DWORD aSize) const;
    int     ReadInt (const char* aAppName, const char* aKeyName, int aDefaultIntVal) const;
    TBool   ReadBool(const char* aAppName, const char* aKeyName, int  aDefaultBoolVal) const;
    TBool   ReadUintFromHex(const char* aAppName, const char* aKeyName, TUint32& aVal) const;

    inline const char* IniFileName() const {return iIniFileName;}
    inline const char* DriveSection() const {return iDrvSectionName;}


 private:
    
    char iIniFileName[KMaxFileName];
    char iDrvSectionName[20];

};


//-----------------------------------------------------------------------------

#endif //SETTINGS_MANAGER_H













