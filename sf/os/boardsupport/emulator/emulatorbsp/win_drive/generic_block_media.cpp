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
// Implementaion of the media abstraction classs.
// 
//

/**
 @file
*/

#include "win_media.h"
#include "win_media_device.h"


//#########################################################################################################################
//##        CMediaBase abstract base class implementation
//#########################################################################################################################


CMediaBase::CMediaBase(TInt aEpocDrvNumber)
           :iEpocDrvNumber((TDriveNumber)aEpocDrvNumber), iReadOnly(0), ipDevice(NULL)
{
    ASSERT(iEpocDrvNumber >= EDriveA && iEpocDrvNumber <= EDriveZ);
    ipSettingsManager = NULL;
}

CMediaBase::~CMediaBase()
{
    Disconnect();    
    delete ipDevice;
    delete ipSettingsManager;
}

/**
    This method is called in order to disconnect from the media (on dismount)
*/
void CMediaBase::Disconnect()
{
    if(ipDevice)
        ipDevice->Disconnect();
}

//-----------------------------------------------------------------------------
/**
    Get EPOC-style drive capabilities
    @param aCaps - some fields of this structure will be filled with the information from the WinMediaDevice
    @return standard EPOC code
*/
TInt CMediaBase::GetEpocCaps(TLocalDriveCaps& aCaps)
{
    //-- get real drive geometry from the windows device we are using and 
    //-- override the native media driver's data
    TDriveGeometry dg;
    ipDevice->GetDriveGeometry(dg);
    aCaps.iSize = dg.TotalSizeInBytes();

    //-- here we can also override other information, like drive and media attributes etc..

    //-- media type
    TUint32 val = ipSettingsManager->TMediaType_Override();
    if(val)
        aCaps.iType = (TMediaType)val;
         
    //-- media attributes
    TUint32 andMask, orMask;
    ipSettingsManager->MediaAtt_OverrideMasks(andMask, orMask);
    aCaps.iMediaAtt &= andMask;
    aCaps.iMediaAtt |= orMask;

    //-- drive attributes
    ipSettingsManager->DriveAtt_OverrideMasks(andMask, orMask);
    aCaps.iDriveAtt &= andMask;
    aCaps.iDriveAtt |= orMask;

    return KErrNone;
}

//#########################################################################################################################
//##        CGenericBlockMedia class implementation
//#########################################################################################################################



CGenericBlockMedia::CGenericBlockMedia(TInt aEpocDrvNumber)
                   :CMediaBase(aEpocDrvNumber)
{
}

CGenericBlockMedia::~CGenericBlockMedia()
{
}

//-----------------------------------------------------------------------------

/**
    Connect to the media, i.e. do mounting job. 
    Read settings from the ini file, instantiate "windows device" class..
*/    
TInt CGenericBlockMedia::Connect()
{
    __LOG1(_L("#-- CGenericBlockMedia::Connect(), epocDrv:%d"), iEpocDrvNumber);

    if(ipDevice)
    {
        ASSERT(0);
        return KErrInUse;
    }

    //-- initialise an object to work with settings from ini file
    if(!ipSettingsManager)
    {
        ipSettingsManager = TSettingsManager::Create(iEpocDrvNumber);
        if(!ipSettingsManager)
        {
            __LOG(_L("#-- Can not create TSettingsManager!"));
            return KErrGeneral;
        }
    }


    //-- 1. obtain and process windows device name
    char szDevName[KMaxFileName];
    Settings().WinDeviceName(szDevName, sizeof(szDevName));
    
    if(strlen(szDevName) < 4)
    {
        __LOG(_L("#-- CGenericBlockMedia::Connect() Bad device name!"));
        __LOGF(szDevName);
        return KErrBadName;
    }

    //-- 1.1 find out if it is the physical volume or image file
    TBool bImageFile = ETrue;
    const char szDevPrefix[] = "\\\\.\\";
    if( memcmp(szDevName, szDevPrefix, strlen(szDevPrefix)) == 0)
    {//-- this is a physical device
        bImageFile = EFalse;    
    }

    
    //-- 2. instantinate an interface to the Windows media device
    TInt nRes;

    if(bImageFile)
    {//-- device name corresponds to the image file
        ipDevice = new CWinImgFileDevice;
    }
    else
    {//-- device name corresponds to the physical volume file
        ipDevice = new CWinVolumeDevice;
    }
    
    ASSERT(ipDevice);


    //-- 3. other parameters
    iReadOnly = Settings().IsReadOnly();

    //-- set and validate parameters
    TMediaDeviceParams connectParams;
    
    const TUint32 bps = Settings().MediaSectorSize();
    if(bps > 0) 
    {//-- some value is provided in the ini file.
        if(!IsPowerOf2(bps) ||  bps < KDefaultSectorSz || bps > 4096)
        {
        __LOG1(_L("#-- 'BytesPerSector' value is incorrect! :%d"), bps);
        return KErrArgument;
        }
    }

    connectParams.iDrvGeometry.iBytesPerSector = bps;
    
    //-- !!! chech size here!!
    connectParams.iDrvGeometry.iSizeInSectors  = Settings().MediaSizeInSectors();
    connectParams.ipDevName = szDevName;
    connectParams.iReadOnly = iReadOnly;
    
    //-- connect to the device driver
    nRes = ipDevice->Connect(connectParams);
    

    return nRes;
    
}

//-----------------------------------------------------------------------------
/**
    Disconnect from the media, dismounting.
*/
void CGenericBlockMedia::Disconnect()
{
    __PRINT1(_L("#-- CGenericBlockMedia::Disconnect(), epocDrv:%d"), iEpocDrvNumber);
    ASSERT(ipDevice);
    CMediaBase::Disconnect();
}

//-----------------------------------------------------------------------------
/**
    Read from the media. See corresponding CWinDrvProxyDrive:: method
*/
TInt CGenericBlockMedia::Read(TInt64 aPos, TInt aLength, TDes8& aDataDes)
{
    //__PRINT3(_L("#-- CGenericBlockMedia::Read(), epocDrv:%d, pos:%LU, len:%u"), iEpocDrvNumber, aPos, aLength);
    ASSERT(ipDevice);
    return ipDevice->Read(aPos, aLength, aDataDes);
}

//-----------------------------------------------------------------------------

/**
    Write to the media. See corresponding CWinDrvProxyDrive:: method
*/
TInt CGenericBlockMedia::Write(TInt64 aPos,TInt aLength, const TDesC8& aDataDes)
{
    //__PRINT3(_L("#-- CGenericBlockMedia::Write(), epocDrv:%d, pos:%LU, len:%u"), iEpocDrvNumber, aPos, aLength);

    ASSERT(ipDevice);

    if(iReadOnly)
        return KErrAccessDenied;

    return ipDevice->Write(aPos, aLength, aDataDes);
}


//-----------------------------------------------------------------------------

/**
    Format the media. 
    For "formatting" called CWinMediaDeviceBase::Erase(...) method that fills required media region with zeroes (see its implementation).
    As soon as the number of erased sectors becomes >= max. number of sectors on this media, KErrEof is reurned indicationg the last
    formatting step done.
*/
TInt CGenericBlockMedia::Format(TFormatInfo& anInfo)
{
    if(iReadOnly)
        return KErrAccessDenied;

    TInt nRes;

    const TUint32 KMaxFmtSteps = 100; //-- max. number of formatting steps.

    TDriveGeometry dg;
    ipDevice->GetDriveGeometry(dg);
    const TUint32 KFmtSectors = (dg.iSizeInSectors + (KMaxFmtSteps-1))/ KMaxFmtSteps; //-- how many sectors format during one step

    if(anInfo.iFormatIsCurrent == 0)
    {//-- this is a first entry, prepare format data
        iSectorsFormatted = 0;
        anInfo.iFormatIsCurrent = 1;
    }
    
    //-- erase block of the media 
    TUint32 sectorsToErase = KFmtSectors;
    if(iSectorsFormatted+KFmtSectors > dg.iSizeInSectors)
    {
        sectorsToErase -= (iSectorsFormatted+KFmtSectors-dg.iSizeInSectors);
    }

    nRes = ipDevice->Erase(iSectorsFormatted*dg.iBytesPerSector, sectorsToErase*dg.iBytesPerSector, 0);
    
    if(nRes != KErrNone)
    {
        ASSERT(0);
        return nRes;
    }

    iSectorsFormatted+=KFmtSectors;
    anInfo.i512ByteSectorsFormatted = iSectorsFormatted;

    if(iSectorsFormatted >= dg.iSizeInSectors)
        return KErrEof; //-- formatted has finished

    return KErrNone;

}


//-----------------------------------------------------------------------------

/**
    "format" the specified region on the media. See corresponding CWinDrvProxyDrive:: method.
*/
TInt CGenericBlockMedia::Format(TInt64 aPos, TInt aLength)
{
    TInt nRes = ipDevice->Erase(aPos, aLength, 0);
    
    return nRes; //-- shall it be KErrEof ?
}
























