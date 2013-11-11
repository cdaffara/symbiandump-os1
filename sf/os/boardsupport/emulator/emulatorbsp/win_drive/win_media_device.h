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
// Definitions for the classes that represent Windows media objects, e.g. a file, physical drive or partition.
// 
//

/**
 @file
*/

#ifndef WIN_MEDIA_DEVICE_H
#define WIN_MEDIA_DEVICE_H

#include "common.h"

#define WIN32_LEAN_AND_MEAN
#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#include <windows.h>
#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#include <winioctl.h> 
#pragma warning( default : 4201 ) // nonstandard extension used : nameless struct/union


//-----------------------------------------------------------------------------

const TUint32 KDefaultSectorSz = 512;       //-- default sector size
const TUint32 KMinMediaSizeInSectors = 128; //-- minimal number of sectors on the media allowed


//-----------------------------------------------------------------------------

/** 
    this structure describes "drive" geometry
    the "drive" consists of a number of sectors.
*/
class TDriveGeometry
{
 public:
    TDriveGeometry() {iBytesPerSector=0; iSizeInSectors=0;}
    
    TInt64 TotalSizeInBytes() const {return iBytesPerSector*(TInt64)iSizeInSectors;}

 public:

    TUint32 iBytesPerSector;  ///< bytes per sectors (usually 512)
    TUint32 iSizeInSectors;   ///< size of the bedia in sectors
};


//-----------------------------------------------------------------------------

/**
    Windows media device creation parameters
*/
class TMediaDeviceParams
{
 public:       
 
    TMediaDeviceParams() {ipDevName = NULL; iReadOnly = EFalse;}

 public:
    TDriveGeometry iDrvGeometry;    ///< drive geometry
    const char*    ipDevName;       ///< windows device name
    TBool          iReadOnly;       ///< if ETrue, the device shall be opened as RO

};

//-----------------------------------------------------------------------------

/**
    Abstract class representing interface to the Windows device (file or volume).
*/
class CWinMediaDeviceBase
{
 public:  
             CWinMediaDeviceBase();
    virtual ~CWinMediaDeviceBase();

    virtual TInt Connect(const TMediaDeviceParams& aParams)=0;
    virtual void Disconnect(); 


    virtual TInt Read(TInt64 aPos,TInt aLength, TDes8& aDataDes)=0;
    virtual TInt Write(TInt64 aPos,TInt aLength, const TDesC8& aDataDes)=0;
    virtual TInt Erase(TInt64 aPos, TUint32 aLength, TUint8 aFill);

    void  GetDriveGeometry(TDriveGeometry& aDG) const {aDG = iDrvGeometry;}

 protected:
    
    CWinMediaDeviceBase(const CWinMediaDeviceBase&);
    CWinMediaDeviceBase& operator=(const CWinMediaDeviceBase&);
    
    inline HANDLE Handle() const {return iDevHandle;}
    inline TBool HandleValid() const;
    inline TBool HandleValid(HANDLE aHandle) const;
    
    TUint32 BytesPerSector() const {return iDrvGeometry.iBytesPerSector;}

    TInt MapWinError(DWORD aWinError) const; 

    enum {KScratchBufSz = 128*1024}; ///< scratch buffer size

 protected:
    
    HANDLE              iDevHandle;     ///< Windows device handle
    TDriveGeometry      iDrvGeometry;   ///< drive geometry.
    TUint8*             ipScratchBuf;   ///< scratch buffer for IO operations
};

//-----------------------------------------------------------------------------

TBool CWinMediaDeviceBase::HandleValid() const 
{
    return HandleValid(iDevHandle);
}

TBool CWinMediaDeviceBase::HandleValid(HANDLE aHandle) const 
{
    return aHandle && (aHandle != INVALID_HANDLE_VALUE);
}


//-----------------------------------------------------------------------------

/**
    This class represents a windows "volume" device, like a physical or logical drive or partition.
    it can also handle files, but not very effectiively.
*/
class CWinVolumeDevice: public CWinMediaDeviceBase
{
 public:  
    CWinVolumeDevice();
   ~CWinVolumeDevice();

    virtual TInt Connect(const TMediaDeviceParams& aParams);
    virtual TInt Read(TInt64 aPos,TInt aLength, TDes8& aDataDes);
    virtual TInt Write(TInt64 aPos,TInt aLength, const TDesC8& aDataDes);
 private:   
    
    MEDIA_TYPE iMediaType;  ///< windows media type.
};



//-----------------------------------------------------------------------------

/**
    This class represents a windows "file" device, i.e. image file on some drive
*/
class CWinImgFileDevice: public CWinMediaDeviceBase
{
 public:  
    CWinImgFileDevice();
   ~CWinImgFileDevice();

    virtual TInt Connect(const TMediaDeviceParams& aParams);
    virtual void Disconnect(); 

    virtual TInt Read(TInt64 aPos,TInt aLength, TDes8& aDataDes);
    virtual TInt Write(TInt64 aPos,TInt aLength, const TDesC8& aDataDes);

 private:
   
 private:   
    HANDLE  ihFileMapping; ///< handle for the image file mapping
    TUint8* ipImageFile;   ///< pointer to the beginning of the image file mapped into memory
};










#endif //WIN_MEDIA_DEVICE_H













