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
// Implementation of the classes responsible for the settings handling in the .ini file
// 
//

/**
 @file
*/

#include "settings_manager.h"
#include "win_media_device.h"

//-----------------------------------------------------------------------------
TSettingsManager::TSettingsManager(const char* apFileName, TInt aEpocDrvNumber)
{
    //-- store ini file name
    ASSERT(lstrlenA(apFileName) < KMaxFileName);
    lstrcpyA(iIniFileName, apFileName);

    //-- make the drive section name e.g. [Drive_D]
    ASSERT(aEpocDrvNumber >= EDriveA && aEpocDrvNumber <= EDriveZ);
    wsprintfA(iDrvSectionName, "Drive_%C", 'A'+aEpocDrvNumber);
}


//-----------------------------------------------------------------------------
/** 
    Factory function
*/
TSettingsManager* TSettingsManager::Create(TInt aEpocDrvNumber)
{
    TSettingsManager* pThis = NULL;

    const char* KDefaultEpocRootName = "EPOCROOT"; 

    //-- try to find the configuration file. It must be: "\\epoc32\\data\\win_drive.ini"
    char buf[KMaxFileName];

    TInt nRes = GetEnvironmentVariableA(KDefaultEpocRootName, buf, sizeof(buf));
    if(!nRes)
    {
     __LOG(_L("#-- TSettingsManager::Create() env. var. 'EPOCROOT' is not set!"));   
     return NULL;
    }

    lstrcatA(buf, "epoc32\\data\\win_drive.ini");

    //-- try to locate ini file
    WIN32_FIND_DATAA wfd;
    HANDLE hFile = FindFirstFileA(buf, &wfd);
    const TBool bIniFilePresent = (hFile != INVALID_HANDLE_VALUE);
    FindClose(hFile);

    if(!bIniFilePresent)
    {
     __LOG(_L("#-- TSettingsManager::Create() ini file is not present, aborting!"));   
     __LOGF(buf); 
     return NULL;
    }

    pThis = new TSettingsManager(buf, aEpocDrvNumber);
    
    return pThis;
}

//-----------------------------------------------------------------------------
/**
    Read a string from the ini file.
 
    @param  aAppName    pointer to the name of the section
    @param  aKeyName    pointer to the name of the key
    @param  aStrDefault pointer to the string with the default value
    @param  aStrDest    pointer to the destination buffer
    @param  aSize       size of the buffer, where the string to bee copied to.
 
    @return length of the read string or length of the aStrDefault if the value hasn't been found
*/
DWORD TSettingsManager::ReadString (const char* aAppName, const char* aKeyName, const char* aStrDefault, char* aStrDest, DWORD aSize) const
{
    return GetPrivateProfileStringA(aAppName, aKeyName, aStrDefault, aStrDest, aSize, IniFileName());
}


/**
    Read integer value from the ini file.
 
    @param aAppName         pointer to the name of the section
    @param aKeyName         pointer to the name of the key
    @param aDefaultIntVal   default value, the result if section, key or value is not found
 
    @return integer value from appropriate section and key, or aDefaultIntVal
*/
int TSettingsManager::ReadInt(const char* aAppName, const char* aKeyName, int aDefaultIntVal) const
{
    return GetPrivateProfileIntA(aAppName, aKeyName, aDefaultIntVal, IniFileName());
}

/**
    Read a boolean value from the ini file.
 
    @param aAppName        pointer to the name of the section
    @param aKeyName        pointer to the name of the key
    @param aDefaultBoolVal default value, the result if section, key of value is not found
    @return     the boolean value or aDefaultBoolVal if section, key or value is not found
*/
TBool TSettingsManager::ReadBool(const char* aAppName, const char* aKeyName, int  aDefaultBoolVal) const
{
    return (ReadInt(aAppName, aKeyName, aDefaultBoolVal ? 1 : 0) != 0);
}


/**
    read a hexadecimal value from the ini file.
 
    @param aAppName pointer to the name of the section
    @param aKeyName pointer to the name of the key
    @param aVal     default value input, if there is a valid value in th eini file, it will be returned here
    @return         ETrue if the valid value from ini file is read; EFalse otherwise.

*/
TBool TSettingsManager::ReadUintFromHex(const char* aAppName, const char* aKeyName, TUint32& aVal) const
{
    unsigned char buff[80];
    
    DWORD dwLen = ReadString(aAppName, aKeyName, "", (char*)buff, sizeof(buff));
    if(!dwLen)
        return EFalse;  //-- the value is not set
        
    //-- try to convert the string from hexadecimal representation to a number.
    TLex8 lex(buff);
    lex.SkipSpace();
    
    //-- skip '0x' prefix if it is present by chance
    TPtrC8 rem = lex.Remainder();
    TInt offset = rem.FindF(_L8("x"));
    if(offset >= 0)
        lex.Inc(offset+1);

    TUint32 val32;
    if(lex.Val(val32, EHex) == KErrNone)
    {
        aVal = val32;
        return ETrue;
    }        

    return EFalse;
}

//-----------------------------------------------------------------------------
/** 
    Get media sector size in bytes. 
    Usually it is 512 bytes, other values are not recommended.
    
    '0' means "auto", i.e. if the media allows, it will be queried for the sector size,
    otherwise 512 bytes will be used.

    @return media sector size in bytes. 
*/
TUint32 TSettingsManager::MediaSectorSize() const
{
    
    const char KeyName[]="BytesPerSector";
    const TInt KDefaultSPC = 0; //-- default value, means "use media settings or 512 bytes by default"
    
    return ReadInt(DriveSection(), KeyName, KDefaultSPC);
}

//-----------------------------------------------------------------------------
/**
    Get media size in sectors. See also MediaSectorSize().
    '0' means "auto". i.e. size will be taken from windows media device (partition or image file)
    
    @return media size in sectors.  
*/
TUint32 TSettingsManager::MediaSizeInSectors() const
{
   
    const char KeyName[]="MediaSizeInSectors";
    const TInt KDefaultSzSec = 0; 

    return ReadInt(DriveSection(), KeyName, KDefaultSzSec);
}

//-----------------------------------------------------------------------------
/**
    Get windows device name that will be used for media emulation.
    like "\\\\.\\Z:" for a physical volume or "K:\\epoc32\\data\\media\\mmccrd0a.bin" for a image file.

    @param  apszName pointer to the buffer, where the name will be copied to.
    @param  aBufLen  length of the input buffer
    @return ETrue on success
*/
TBool TSettingsManager::WinDeviceName(char* apszName, TUint aBufLen) const
{
    const char KeyName[]="DeviceName";
    const char KDefaultValue[]="";

    TInt nRes = ReadString(DriveSection(), KeyName, KDefaultValue, apszName, aBufLen);
    
    if(nRes == 0)
    {
    __LOG(_L("#-- TSettingsManager: 'DeviceName' key value isn't set !"));   
    }

    return ETrue;
}


//-----------------------------------------------------------------------------
/**
    @return ETrue if the device is Read-Only
*/
TBool TSettingsManager::IsReadOnly() const
{
    const char KeyName[]="ReadOnly";
    const TBool KDefaultValue = EFalse; 

    return ReadBool(DriveSection(), KeyName, KDefaultValue);
}

//-----------------------------------------------------------------------------
/**
    @return Media type override value from the ini file or 0 if it is not set there
*/
TUint32 TSettingsManager::TMediaType_Override() const
{
    const char KeyName[]="MediaTypeOverride";
    const TInt KDefaultVal = 0; 

    return ReadInt(DriveSection(), KeyName, KDefaultVal);
}

//-----------------------------------------------------------------------------

/**
    Get Media Attributes override "AND" and "OR" bitmasks. (the need to be in hex in the ini file).
    
    @param  aAndMask    On return contains "And" mask; default value == 0xFFFFFFFF
    @param  aOrMask     On return contains "OR" mask;  default value == 0x00
*/
void TSettingsManager::MediaAtt_OverrideMasks(TUint32 &aAndMask, TUint32 &aOrMask) const
{
    TUint32 defVal;

    //-- 1. read "AND" mask
    defVal = KMaxTUint;
    const char KeyName1[]="MediaAttOverride_AND";
    TBool bRes = ReadUintFromHex(DriveSection(), KeyName1, defVal);
    aAndMask = defVal;

    //-- 2. read "OR" mask
    const char KeyName2[]="MediaAttOverride_OR";
    defVal = 0;
    bRes = ReadUintFromHex(DriveSection(), KeyName2, defVal);
    aOrMask = defVal;
}

//-----------------------------------------------------------------------------
/**
    Get Drive Attributes override "AND" and "OR" bitmasks. (the need to be in hex in the ini file).
    
    @param  aAndMask    On return contains "And" mask; default value == 0xFFFFFFFF
    @param  aOrMask     On return contains "OR" mask;  default value == 0x00
*/
void TSettingsManager::DriveAtt_OverrideMasks(TUint32 &aAndMask, TUint32 &aOrMask) const
{
    TUint32 defVal;

    //-- 1. read "AND" mask
    defVal = KMaxTUint;
    const char KeyName1[]="DriveAttOverride_AND";
    TBool bRes = ReadUintFromHex(DriveSection(), KeyName1, defVal);
    aAndMask = defVal;

    //-- 2. read "OR" mask
    const char KeyName2[]="DriveAttOverride_OR";
    defVal = 0;
    bRes = ReadUintFromHex(DriveSection(), KeyName2, defVal);
    aOrMask = defVal;
}



























