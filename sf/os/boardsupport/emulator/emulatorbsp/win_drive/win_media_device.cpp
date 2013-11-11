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
// Implementation of the classes that work directly with the windows devices - files, drives etc.
// 
//

/**
 @file
*/

#include "win_media_device.h"

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER 0xFFFFFFFF
#endif


const TInt KDiskOpError = 0x134AFF78; ///< internal Disk operation error ID.


static TBool CheckBufFill(const TPtrC8& aBufPtr, TUint8 aFill);

//#########################################################################################################################
//##        CWinMediaDeviceBase abstract base class implementation
//#########################################################################################################################

//-----------------------------------------------------------------------------

CWinMediaDeviceBase::CWinMediaDeviceBase()
{
    iDevHandle = NULL;
    ipScratchBuf = NULL;
}

CWinMediaDeviceBase::~CWinMediaDeviceBase()
{
    Disconnect();
}


/**
    Disconnect from the media device
*/
void CWinMediaDeviceBase::Disconnect()
{
    FlushFileBuffers(iDevHandle);
    CloseHandle(iDevHandle);
    iDevHandle = NULL;
    
}

//-----------------------------------------------------------------------------

/**
    "Erase" a region of the media. Effectively just fills the selected region with the specified pattern.
    
    @param  aPos     media position start 
    @param  aLength  length of the media region to fill
    @param  aFill    filler byte.

    @return EPOC error code.

*/
TInt CWinMediaDeviceBase::Erase(TInt64 aPos, TUint32 aLength, TUint8 aFill)
{
    //-- this method is called to "format" media.
    //-- Because Windows is absolute sux on everythins that concerns formattin the media (IOCTL_DISK_FORMAT_TRACKS seems to be applicable only 
    //-- to floppy disks) we have to perform formatting by just filling media region with a given byte.
    //-- This can be very slow for flash - based removable media (e.g. usb flash drives) so, there is a possibility to check 
    //-- if the given media region is already filled with the pattern and write only if not. See bCheckReadBeforeErase switch.
    
    Mem::Fill(ipScratchBuf, KScratchBufSz, aFill);
    
    TUint32 rem = aLength;
    TInt nRes = KErrNone;

    //-- if True, we firstly will read media region and check if it is already filled with the given byte. 
    //-- this is useful for slow - write media or sparse files on NTFS.
    //TBool bCheckReadBeforeErase = EFalse; 
    TBool bCheckReadBeforeErase = ETrue; 
    
    while(rem)
    {
        const TUint32 bytesToWrite = Min(KScratchBufSz, rem);
        TPtr8 ptrData(ipScratchBuf, bytesToWrite, bytesToWrite);

        if(bCheckReadBeforeErase)
        {//-- try to read data first and check if we need to write anything
            ptrData.SetLength(0);
            nRes = Read(aPos, bytesToWrite, ptrData);
            if(nRes != KErrNone)
                break;

            if(!CheckBufFill(ptrData, aFill))
            {
                Mem::Fill(ipScratchBuf, KScratchBufSz, aFill);

                nRes = Write(aPos, bytesToWrite, ptrData);
                if(nRes != KErrNone)
                    break;
            
            }
        }
        else
        {//-- no need to read first
            nRes = Write(aPos, bytesToWrite, ptrData);
            if(nRes != KErrNone)
                break;
            
        }
        

        rem-=bytesToWrite;
        aPos+=bytesToWrite;
           
    }

    
    return nRes;
}


//#########################################################################################################################
//##        CWinVolumeDevice  class implementation
//#########################################################################################################################


CWinVolumeDevice::CWinVolumeDevice()
                 :CWinMediaDeviceBase()
{
    //-- create scratch buffer
    ipScratchBuf = ::new TUint8[KScratchBufSz];
    ASSERT(ipScratchBuf);
}

CWinVolumeDevice::~CWinVolumeDevice()
{
    delete ipScratchBuf;
}

//-----------------------------------------------------------------------------

/**
    Open the device and do some initalisation work.
    
    @param  aParams device parameters
    @return Epoc error code, KErrNone if everything is OK
*/
TInt CWinVolumeDevice::Connect(const TMediaDeviceParams& aParams)
{
    
    __PRINT(_L("#-- CWinVolumeDevice::Connect()"));    
    
    if(!aParams.ipDevName)
    {
        __LOG(_L("#-- CWinVolumeDevice::Connect() device name is not set!"));    
        return KErrBadName;
    }

    __PRINTF(aParams.ipDevName);
    
    ASSERT(!HandleValid() && ipScratchBuf);

    //-- open the device
    DWORD dwAccess = GENERIC_READ;
    
    if(!aParams.iReadOnly)
        dwAccess |= GENERIC_WRITE;  
    
    iDevHandle = CreateFileA(aParams.ipDevName,
                             dwAccess, 
                             FILE_SHARE_READ,
                             (LPSECURITY_ATTRIBUTES)NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);

    if(!HandleValid())
    {
        __LOG1(_L("#-- CWinVolumeDevice::Connect() Error creating device handle! WinErr:%d"), GetLastError());
        return KErrGeneral;
    }     
    
    //-- find out device geometry
    iMediaType = Unknown;
    iDrvGeometry.iBytesPerSector = KDefaultSectorSz;

    DWORD junk; 

    //-- 1. try to query disk geometry, but it can produce wrong results for partitioned media
    BOOL bResult = DeviceIoControl(Handle(),
                                   IOCTL_DISK_GET_DRIVE_GEOMETRY,
                                   NULL, 0,
                                   ipScratchBuf, KScratchBufSz,
                                   &junk, (LPOVERLAPPED)NULL);

    if(bResult)
    {
        const DISK_GEOMETRY& dg = (const DISK_GEOMETRY&)*ipScratchBuf;
        
        iDrvGeometry.iBytesPerSector = dg.BytesPerSector;
        iMediaType = dg.MediaType;

        __PRINT3(_L("#-- dev geometry: Cyl:%d Heads:%d Sectors:%d"), dg.Cylinders.LowPart, dg.TracksPerCylinder, dg.SectorsPerTrack);    
        __PRINT2(_L("#-- dev geometry: MediaType:%d, bps:%d"), dg.MediaType, dg.BytesPerSector);    

    }
    else
    {
        iMediaType = Unknown;
        iDrvGeometry.iBytesPerSector = KDefaultSectorSz;

        __LOG1(_L("#-- CWinVolumeDevice::Connect() IOCTL_DISK_GET_DRIVE_GEOMETRY WinError:%d !"), GetLastError());
    }

    //-- 1.1 check "bytes per sector" value and how it corresponds to the request from parameters
    if(aParams.iDrvGeometry.iBytesPerSector == 0)
    {//-- do nothing, this parameter is not set in config file, use media's
    } 
    else if(aParams.iDrvGeometry.iBytesPerSector != iDrvGeometry.iBytesPerSector)
    {//-- we can't set "SectorSize" value for the physical media
        __LOG1(_L("#-- CWinVolumeDevice::Connect() can not use 'Sec. Size' value from config:%d !"), aParams.iDrvGeometry.iBytesPerSector);
        Disconnect();
        return KErrArgument;
    }


    ASSERT(IsPowerOf2(BytesPerSector()) && BytesPerSector() >= KDefaultSectorSz && BytesPerSector() < 4096);

    //-- find out partition information in order to determine volume size. 
    bResult = DeviceIoControl(Handle(),
                              IOCTL_DISK_GET_PARTITION_INFO,
                              NULL, 0,
                              ipScratchBuf, KScratchBufSz,
                              &junk, (LPOVERLAPPED)NULL);

    if(!bResult)
    {//-- this is a fatal error
        __LOG1(_L("#-- CWinVolumeDevice::Connect() IOCTL_DISK_GET_PARTITION_INFO WinError:%d !"), GetLastError());
        Disconnect();
        return KErrBadHandle;    
    }

    //-- get partition informaton
    const PARTITION_INFORMATION& pi = (const PARTITION_INFORMATION&)*ipScratchBuf;
    TInt64 volSz = MAKE_TINT64(pi.PartitionLength.HighPart, pi.PartitionLength.LowPart);
    iDrvGeometry.iSizeInSectors = (TUint32)(volSz / iDrvGeometry.iBytesPerSector);
            
    __LOG3(_L("#-- partition size, bytes:%LU (%uMB), sectors:%u"), volSz, (TUint32)(volSz>>20), iDrvGeometry.iSizeInSectors);
   
    //-- check if the media size is set in coonfig and if we can use this setting.
    if(aParams.iDrvGeometry.iSizeInSectors == 0)
    {//-- do nothing, the media size is not set in the ini file, use existing media parameters
    }
    else if(aParams.iDrvGeometry.iSizeInSectors > iDrvGeometry.iSizeInSectors)
    {//-- requested media size in ini file is bigger than physical media, error.
     //-- we can't increase physical media size
    __LOG2(_L("#-- CWinVolumeDevice::Connect() 'MediaSizeSectors' value from config:%d > than physical:%d !"), aParams.iDrvGeometry.iSizeInSectors, iDrvGeometry.iSizeInSectors);
    Disconnect();
    return KErrArgument;
    }
    else if(aParams.iDrvGeometry.iSizeInSectors < iDrvGeometry.iSizeInSectors)
    {//-- settings specify smaller media than physical one, adjust the size
    __PRINT1(_L("#-- reducing media size to %d sectors"), aParams.iDrvGeometry.iSizeInSectors);
    iDrvGeometry.iSizeInSectors = aParams.iDrvGeometry.iSizeInSectors;
    }


    ASSERT(iDrvGeometry.iSizeInSectors > KMinMediaSizeInSectors);
    return KErrNone;
}

//-----------------------------------------------------------------------------

/**
    Read a portion of data from the device. 
    Note: at present it _APPENDS_ data to the aDataDes, so the caller must take care of setting its length

    @param  aPos     media position in bytes
    @param  aLength  how many bytes to read
    @param  aDataDes data descriptor

    @return KErrNone on success, standard Epoc error code otherwise

*/
TInt CWinVolumeDevice::Read(TInt64 aPos, TInt aLength, TDes8& aDataDes)
{
    //__PRINT2(_L("#-- CWinVolumeDevice::Read, pos:%LU, len:%u"), aPos, aLength);
 
    ASSERT(HandleValid());
    ASSERT(aLength <= aDataDes.MaxLength());

    //-- check position on the volume
    const TInt64 maxPos = iDrvGeometry.TotalSizeInBytes();
    if(aPos < 0 || aPos > maxPos)
        return KErrArgument;

    const TInt64 lastPos = aPos+aLength;

    if(lastPos > maxPos)
        return KErrArgument;
    //--


    TUint32 dataLen = aLength;

    if(dataLen == 0)
        return KErrNone;

    DWORD dwRes;
    DWORD dwBytesRead = 0;

    //aDataDes.SetLength(0);

    const TUint32 KSectorSize = BytesPerSector();
    
    try
    {
        LONG    mediaPosHi = I64HIGH(aPos);
        const TUint32 mediaPosLo = I64LOW(aPos);
        const TUint32 startPosOffset = mediaPosLo & (KSectorSize-1);
        
        //-- 1. position to the media with sector size granularity and read 1st sector
        dwRes = SetFilePointer(iDevHandle, mediaPosLo-startPosOffset, &mediaPosHi, FILE_BEGIN);
        if(dwRes == INVALID_SET_FILE_POINTER)
            throw KDiskOpError;

        //-- 1.1 read 1st sector
        if(!ReadFile(iDevHandle, ipScratchBuf, KSectorSize, &dwBytesRead, NULL))
            throw KDiskOpError;
        
        const TUint32 firstChunkLen = Min(dataLen, KSectorSize - startPosOffset);
        aDataDes.Append(ipScratchBuf+startPosOffset, firstChunkLen);
        dataLen-=firstChunkLen;

        if(dataLen == 0)
            return KErrNone; //-- no more data to read
    
        //-- 2. read whole number of sectors from the meida
        const TUint32 KBytesTail = dataLen & (KSectorSize-1); //-- number of bytes in the incomplete last sector
    
        ASSERT((KScratchBufSz % KSectorSize) == 0);

        TUint32 rem = dataLen - KBytesTail;
        while(rem)
        {
            const TUint32 bytesToRead = Min(KScratchBufSz, rem);
    
            if(!ReadFile(iDevHandle, ipScratchBuf, bytesToRead, &dwBytesRead, NULL))
                throw KDiskOpError;        
 
            aDataDes.Append(ipScratchBuf, bytesToRead);
            rem-=bytesToRead;
        }

        //-- 3. read the rest of the bytes in the incomplete last sector
        if(KBytesTail)
        {
            if(!ReadFile(iDevHandle, ipScratchBuf, KSectorSize, &dwBytesRead, NULL))
                throw KDiskOpError;    

            aDataDes.Append(ipScratchBuf, KBytesTail);
        }

    }//try
    catch(TInt nErrId)
    {//-- some disk operation finished with the error
        (void)nErrId;
        ASSERT(nErrId == KDiskOpError);
        const DWORD dwWinErr = GetLastError();
        const TInt  epocErr = MapWinError(dwWinErr);
        
        __PRINT2(_L("#-- CWinVolumeDevice::Read() error! WinErr:%d, EpocErr:%d"), dwWinErr, epocErr);
        ASSERT(epocErr != KErrNone);

        return epocErr;
    }

    return KErrNone;
}

//-----------------------------------------------------------------------------
/**
    Write some data to the device.

    @param  aPos     media position in bytes
    @param  aLength  how many bytes to read
    @param  aDataDes data descriptor

    @return KErrNone on success, standard Epoc error code otherwise
*/
TInt CWinVolumeDevice::Write(TInt64 aPos, TInt aLength, const TDesC8& aDataDes)
{
    //__PRINT2(_L("#-- CWinVolumeDevice::Write, pos:%LU, len:%u"), aPos, aLength);

    ASSERT(HandleValid());
    
    if(aLength == 0 || aDataDes.Length() == 0)
        return KErrNone;

    if(aLength > aDataDes.Length())
    {
        ASSERT(0);
        return KErrArgument;
    }

    //-- check position on the volume
    const TInt64 maxPos = iDrvGeometry.TotalSizeInBytes();
    if(aPos < 0 || aPos > maxPos)
        return KErrArgument;

    const TInt64 lastPos = aPos+aLength;
    if(lastPos > maxPos)
        return KErrArgument;

    TUint32 dataLen = aLength;

    DWORD dwRes;
    DWORD dwBytes = 0;
    
    const TUint32 KSectorSize = BytesPerSector();
    const TUint8 *pData = aDataDes.Ptr();
    
    try
    {
        LONG    mediaPosHi = I64HIGH(aPos);
        const TUint32 mediaPosLo = I64LOW(aPos);
        const TUint32 startPosOffset = mediaPosLo & (KSectorSize-1);
        const TUint32 sectorPos = mediaPosLo-startPosOffset;

        //-- 1. position to the media with sector size granularity 
        dwRes = SetFilePointer(iDevHandle, sectorPos, &mediaPosHi, FILE_BEGIN);
        if(dwRes == INVALID_SET_FILE_POINTER)
        {    
            throw KDiskOpError;
        }

        if(startPosOffset || dataLen <= KSectorSize)
        {//-- need a read-modify-write here.
            //-- 1.1 read first sector
            if(!ReadFile(iDevHandle, ipScratchBuf, KSectorSize, &dwBytes, NULL))
                throw KDiskOpError;

            dwRes = SetFilePointer(iDevHandle, sectorPos, &mediaPosHi, FILE_BEGIN);
            if(dwRes == INVALID_SET_FILE_POINTER)
            {    
                throw KDiskOpError;
            }


            if(dwRes == INVALID_SET_FILE_POINTER)
                throw KDiskOpError;

            //-- 1.2 copy chunk of data there
            const TUint32 firstChunkLen = Min(dataLen, KSectorSize - startPosOffset);
            Mem::Copy(ipScratchBuf+startPosOffset, pData, firstChunkLen);
            
            //-- 1.3 write sector
            if(!WriteFile(iDevHandle, ipScratchBuf, KSectorSize, &dwBytes, NULL))
                throw KDiskOpError;


            dataLen-=firstChunkLen;
            pData+=firstChunkLen;

            if(dataLen == 0)
                return KErrNone; //-- no more data to write
        }

        //-- 2. write whole number of sectors to the media
        const TUint32 KBytesTail = dataLen & (KSectorSize-1); //-- number of bytes in the incomplete last sector
        TUint32 KMainChunkBytes = dataLen - KBytesTail;

        ASSERT((KMainChunkBytes % KSectorSize) == 0);

        //-- the pointer to the data shall be 2-bytes aligned, otherwise WriteFile will fail
        if(!((DWORD)pData & 0x01))
        {//-- data pointer aligned, ok
            if(!WriteFile(iDevHandle, pData, KMainChunkBytes, &dwBytes, NULL))
                throw KDiskOpError;
        
            pData+=KMainChunkBytes;
            dataLen-=KMainChunkBytes;

        }
        else
        {//-- data pointer is odd, we need to copy data to the aligned buffer
            TUint32 rem = KMainChunkBytes;
            while(rem)
            {
                const TUint32 nBytesToWrite = Min(KScratchBufSz, rem);
                Mem::Copy(ipScratchBuf, pData, nBytesToWrite);
            
                if(!WriteFile(iDevHandle, ipScratchBuf, nBytesToWrite, &dwBytes, NULL))
                    throw KDiskOpError;
        
                rem-=nBytesToWrite;
                pData+=nBytesToWrite;
                dataLen-=nBytesToWrite;
            }

        }


        //-- 3. write the rest of the bytes into the incomplete last sector
        if(KBytesTail)
        {
            //-- 3.1 read last sector
            if(!ReadFile(iDevHandle, ipScratchBuf, KSectorSize, &dwBytes, NULL))
                throw KDiskOpError;    

            LARGE_INTEGER liRelOffset;
            liRelOffset.QuadPart = -(LONG)KSectorSize;

            //dwRes = SetFilePointer(iDevHandle, -(LONG)KSectorSize, NULL, FILE_CURRENT);
            
            dwRes = SetFilePointer(iDevHandle, liRelOffset.LowPart, &liRelOffset.HighPart, FILE_CURRENT);
            if(dwRes == INVALID_SET_FILE_POINTER)
                throw KDiskOpError;

            //-- 1.2 copy chunk of data there
            Mem::Copy(ipScratchBuf, pData, KBytesTail);

            //-- 1.3 write sector
            if(!WriteFile(iDevHandle, ipScratchBuf, KSectorSize, &dwBytes, NULL))
                throw KDiskOpError;
            
        }


    }//try
    catch(TInt nErrId)
    {//-- some disk operation finished with the error
        (void)nErrId;
        ASSERT(nErrId == KDiskOpError);
        const DWORD dwWinErr = GetLastError();
        const TInt  epocErr = MapWinError(dwWinErr);
        
        __PRINT2(_L("#-- CWinVolumeDevice::Write() error! WinErr:%d, EpocErr:%d"), dwWinErr, epocErr);
        ASSERT(epocErr != KErrNone);
        return epocErr;
    }
 
    return KErrNone;
}

//-----------------------------------------------------------------------------

/**
    Check if the buffer is filled with aFill character.
    @param  aBufPtr buffer descriptor
    @param  aFill filling character
    @return ETrue if the buffer is filled with aFill byte.
*/
static TBool CheckBufFill(const TPtrC8& aBufPtr, TUint8 aFill)
{
    const TUint32 bufSz = (TUint32)aBufPtr.Size();
    
    //-- optimised by using DWORD granularity
    if(bufSz % sizeof(TUint32) == 0)
    {
        TUint32 wordPattern = aFill;
        wordPattern <<= 8; wordPattern |= aFill;
        wordPattern <<= 8; wordPattern |= aFill;
        wordPattern <<= 8; wordPattern |= aFill;

        const TUint nWords = bufSz / sizeof(TUint32);
        const TUint32* pWords = (const TUint32*) aBufPtr.Ptr();

        for(TUint32 i=0; i<nWords; ++i)
        {
            if(pWords[i] != wordPattern)
                return EFalse;
        }

        return ETrue;
    }
    
    //-- dumb implementation
    for(TUint32 i=0; i<bufSz; ++i)
    {
        if(aBufPtr[i] != aFill)
            return EFalse;
    }

    return ETrue;
}


//#########################################################################################################################
//##        CWinImgFileDevice  class implementation
//#########################################################################################################################


//-----------------------------------------------------------------------------

CWinImgFileDevice::CWinImgFileDevice()
                  :CWinMediaDeviceBase() 
{

    ihFileMapping = NULL;
    ipImageFile  = NULL;

    //-- create scratch buffer
    ipScratchBuf = ::new TUint8[KScratchBufSz];
    ASSERT(ipScratchBuf);

}

CWinImgFileDevice::~CWinImgFileDevice()
{
    delete ipScratchBuf;
}

//-----------------------------------------------------------------------------
void CWinImgFileDevice::Disconnect()
{
    CloseHandle(ihFileMapping);
    ihFileMapping = NULL;
    ipImageFile  = NULL;

    CWinMediaDeviceBase::Disconnect();
}

//-----------------------------------------------------------------------------
/**
    Open the device and do some initalisation work.
    
    @param  aParams device parameters
    @return Epoc error code, KErrNone if everything is OK
*/
TInt CWinImgFileDevice::Connect(const TMediaDeviceParams& aParams)
{
    __PRINT(_L("#-- CWinImgFileDevice::Connect()"));    
    
    if(!aParams.ipDevName)
    {
        __LOG(_L("#-- CWinImgFileDevice::Connect() device name is not set!"));    
        return KErrBadName;
    }
    __PRINTF(aParams.ipDevName);
    ASSERT(!HandleValid());

    //-- 1. try to locate an image file by given name.
    WIN32_FIND_DATAA wfd;
    iDevHandle = FindFirstFileA(aParams.ipDevName, &wfd);

    const TBool ImgFileAlreadyExists = HandleValid(iDevHandle);
    
    FindClose(iDevHandle);
    iDevHandle = NULL;
    
    //-- sector size we will use within image file
    const TUint32   sectorSizeToUse = (aParams.iDrvGeometry.iBytesPerSector == 0) ? KDefaultSectorSz : aParams.iDrvGeometry.iBytesPerSector; 
          TUint32   fileSzInSectorsToUse = 0;

    const TUint32   reqSizeSec = aParams.iDrvGeometry.iSizeInSectors; //-- required size in sectors
    const DWORD     dwAccessMode = (aParams.iReadOnly) ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;  

    if(ImgFileAlreadyExists)
    {//-- if the image file already exists, try to open it and optionally adjust its size
        const TInt64    ImgFileSize = MAKE_TINT64(wfd.nFileSizeHigh, wfd.nFileSizeLow);
        const TUint32   ImgFileSectors = (TUint32)(ImgFileSize / sectorSizeToUse);
        const TBool     ImgFileIsRO = wfd.dwFileAttributes & FILE_ATTRIBUTE_READONLY;
        
        DWORD dwFileCreationMode = 0;
        TBool bNeedToAdjustFileSize = EFalse;

        if(reqSizeSec == 0 || reqSizeSec == ImgFileSectors)
        {//-- the required size is either not specified (auto) or the same as the existing file has.
         //-- we can just open this file
         dwFileCreationMode = OPEN_EXISTING;
         fileSzInSectorsToUse = ImgFileSectors;
        }
        else
        {//-- we will have to overwrite the image file
            if(ImgFileIsRO)
            {//-- we won't be able to overwrite existing file.
                __LOG(_L("#-- CWinImgFileDevice::Connect() unable to adjust image file size!"));    
                return KErrAccessDenied;
            }

         fileSzInSectorsToUse = reqSizeSec;
         dwFileCreationMode = CREATE_ALWAYS;
         bNeedToAdjustFileSize = ETrue;
        }

        iDevHandle = CreateFileA(aParams.ipDevName,
                                dwAccessMode, 
                                FILE_SHARE_READ,
                                (LPSECURITY_ATTRIBUTES)NULL,
                                dwFileCreationMode,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

        if(!HandleValid(iDevHandle))
        {
            const DWORD winErr = GetLastError();
            __LOG1(_L("#-- CWinImgFileDevice::Connect() Error opening/creating file! WinErr:%d"), winErr);
            return MapWinError(winErr);
        }     

        //-- adjust file size if we need
        if(bNeedToAdjustFileSize)
        {
            const TInt64 newFileSize = (TInt64)reqSizeSec * sectorSizeToUse;
            ASSERT(newFileSize);

            LONG  newFSzHi = I64HIGH(newFileSize);
            DWORD dwRes = SetFilePointer(iDevHandle, I64LOW(newFileSize), &newFSzHi, FILE_BEGIN);
            if(dwRes == INVALID_SET_FILE_POINTER || !SetEndOfFile(iDevHandle))
            {
                const DWORD winErr = GetLastError();
                Disconnect();
                __LOG1(_L("#-- CWinImgFileDevice::Connect() unable to set file size! WinErr:%d"), winErr);
                return MapWinError(winErr);
            }
        }

    }
    else //if(ImgFileAlreadyExists)
    {//-- if the image file does not exist or its size differs from required. try to create it
       
        if(reqSizeSec == 0)
        {
            __LOG(_L("#-- CWinImgFileDevice::Connect() The image file doesn't exist ant its size isn't specified!"));    
            return KErrArgument;
        }
       
        fileSzInSectorsToUse = reqSizeSec;

        //-- create a new image file
        iDevHandle = CreateFileA(aParams.ipDevName,
                                GENERIC_READ | GENERIC_WRITE, 
                                FILE_SHARE_READ,
                                (LPSECURITY_ATTRIBUTES)NULL,
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

        if(!HandleValid(iDevHandle))
        {
            const DWORD winErr = GetLastError();
            __LOG1(_L("#-- CWinImgFileDevice::Connect() can not create file! WinErr:%d"), winErr);
            return MapWinError(winErr);
        }     

        //-- set its size
        const TInt64 newFileSize = (TInt64)reqSizeSec * sectorSizeToUse;
        ASSERT(newFileSize);

        LONG  newFSzHi = I64HIGH(newFileSize);
        DWORD dwRes = SetFilePointer(iDevHandle, I64LOW(newFileSize), &newFSzHi, FILE_BEGIN);
        if(dwRes == INVALID_SET_FILE_POINTER || !SetEndOfFile(iDevHandle))
        {
            const DWORD winErr = GetLastError();
            Disconnect();
            __LOG1(_L("#-- CWinImgFileDevice::Connect() unable to set file size! WinErr:%d"), winErr);
            return MapWinError(winErr);
        }

        //-- if parametrs require a read-only file, reopen it in RO mode, it doesn't make a lot of sense though...
        if(aParams.iReadOnly)
        {
            CloseHandle(iDevHandle);
            iDevHandle = NULL;

            iDevHandle = CreateFileA(aParams.ipDevName,
                                GENERIC_READ , 
                                FILE_SHARE_READ,
                                (LPSECURITY_ATTRIBUTES)NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

            if(!HandleValid(iDevHandle))
            {
                const DWORD winErr = GetLastError();
                __LOG1(_L("#-- CWinImgFileDevice::Connect() Can't reopen a file in RO mode! WinErr:%d"), winErr);
                return MapWinError(winErr);
            }     
            
        }//if(aParams.iReadOnly)

    }//else if(ImgFileAlreadyExists)
    
    //-- here we must have the image file created/opened and with correct size
    ASSERT(HandleValid());
    ASSERT(sectorSizeToUse);

    if(fileSzInSectorsToUse < KMinMediaSizeInSectors)
    {
        __LOG1(_L("#-- CWinImgFileDevice::Connect() Image file is too small!  sectors:%d"), fileSzInSectorsToUse);
        Disconnect();
        return KErrGeneral;     
    }

    iDrvGeometry.iBytesPerSector = sectorSizeToUse;
    iDrvGeometry.iSizeInSectors  = fileSzInSectorsToUse;
    
    //-- map the image file into memory.
    ASSERT(!HandleValid(ihFileMapping));
    ASSERT(!ipImageFile);
    
    /*
    don't map image file, because it can be > 4G.
    ihFileMapping = CreateFileMapping(Handle(), NULL,
                                      aParams.iReadOnly ? PAGE_READONLY : PAGE_READWRITE,
                                      0, 0, NULL);
    if(HandleValid(ihFileMapping))
    {
    ipImageFile = (TUint8*)MapViewOfFile(ihFileMapping, 
                                         aParams.iReadOnly ? FILE_MAP_READ : FILE_MAP_WRITE,
                                         0,0,0);
    }

    if(!ipImageFile)
    {
        __PRINT1(_L("#-- CWinImgFileDevice::Connect() Error mapping file! WinErr:%d"), GetLastError());
        Disconnect();
        return KErrGeneral;
    }
    */

    return KErrNone;
}


/**
    Read a portion of data from the device.
    Note: at present it _APPENDS_ data to the aDataDes, so the caller must take care of setting its length

    @param  aPos     media position in bytes
    @param  aLength  how many bytes to read
    @param  aDataDes data descriptor

    @return KErrNone on success, standard Epoc error code otherwise

*/
TInt CWinImgFileDevice::Read(TInt64 aPos,TInt aLength, TDes8& aDataDes)
{
    
    //__PRINT3(_L("#-- CWinImgFileDevice::Read, pos:%LU, len:%u, desMaxLen:%u"), aPos, aLength, aDataDes.MaxLength());

    ASSERT(HandleValid());
    ASSERT(aLength <= aDataDes.MaxLength());

    //-- check position on the volume
    const TInt64 maxPos = iDrvGeometry.TotalSizeInBytes();
    if(aPos < 0 || aPos > maxPos)
        return KErrArgument;

    const TInt64 lastPos = aPos+aLength;
    if(lastPos > maxPos)
        return KErrArgument;

    TUint32 dataLen = aLength;

    if(dataLen == 0)
        return KErrNone;

    DWORD dwRes;
    DWORD dwBytesRead = 0;

    //aDataDes.SetLength(0);

    try
    {
        //-- 1. position to the media 
        LONG  mediaPosHi = I64HIGH(aPos);
        const TUint32 mediaPosLo = I64LOW(aPos);

        dwRes = SetFilePointer(iDevHandle, mediaPosLo, &mediaPosHi, FILE_BEGIN);
        if(dwRes == INVALID_SET_FILE_POINTER)
            throw KDiskOpError;


        //-- 2. read data to the scratch buffer and copy it to the descriptor.
        ASSERT(ipScratchBuf);

        TUint32 rem = dataLen;
        
        while(rem)
        {
            const TUint32 bytesToRead = Min(KScratchBufSz, rem);
            if(!ReadFile(iDevHandle, ipScratchBuf, bytesToRead, &dwBytesRead, NULL))
                throw KDiskOpError;

            aDataDes.Append(ipScratchBuf, bytesToRead);
            rem-=bytesToRead;
        }

    }
    catch(TInt nErrId)
    {//-- some disk operation finished with the error
        (void)nErrId;
        ASSERT(nErrId == KDiskOpError);
        const DWORD dwWinErr = GetLastError();
        const TInt  epocErr = MapWinError(dwWinErr);
        
        __PRINT2(_L("#-- CWinImgFileDevice::Read() error! WinErr:%d, EpocErr:%d"), dwWinErr, epocErr);
        ASSERT(epocErr != KErrNone);

        return epocErr;
    }


    return KErrNone;
}

/**
    Write some data to the device.

    @param  aPos     media position in bytes
    @param  aLength  how many bytes to read
    @param  aDataDes data descriptor

    @return KErrNone on success, standard Epoc error code otherwise
*/
TInt CWinImgFileDevice::Write(TInt64 aPos, TInt aLength, const TDesC8& aDataDes)
{
    //__PRINT3(_L("#-- CWinImgFileDevice::Write, pos:%LU, len:%u, desLen:%u" ), aPos, aLength, aDataDes.Length());

    ASSERT(HandleValid());


    if(aLength == 0 || aDataDes.Length() == 0)
        return KErrNone;

    if(aLength > aDataDes.Length())
    {
        ASSERT(0);
        return KErrArgument;
    }

    //-- check position on the volume
    const TInt64 maxPos = iDrvGeometry.TotalSizeInBytes();
    if(aPos < 0 || aPos > maxPos)
        return KErrArgument;

    const TInt64 lastPos = aPos+aLength;

    if(lastPos > maxPos)
        return KErrArgument;

    TUint32 dataLen = aLength;


    DWORD dwRes;
    DWORD dwBytes = 0;
    
    const TUint8 *pData = aDataDes.Ptr();

    try
    {
        //-- 1. position to the media
        LONG  mediaPosHi = I64HIGH(aPos);
        const TUint32 mediaPosLo = I64LOW(aPos);
        dwRes = SetFilePointer(iDevHandle, mediaPosLo, &mediaPosHi, FILE_BEGIN);
        if(dwRes == INVALID_SET_FILE_POINTER)
        {    
            throw KDiskOpError;
        }
    
        //-- 2. write data to the media
        //-- check if the pointer is word-aligned
        const DWORD dwPtrMask = 0x01;
        
        if( (DWORD)pData & dwPtrMask)
        {//-- data pointer isn't aligned, write non-aligned bytes through buffer
            ASSERT(dataLen);

            const int oddCnt = 1;
            ipScratchBuf[0] = *pData;

            ++pData;
            --dataLen;

            if(!WriteFile(iDevHandle, ipScratchBuf, oddCnt, &dwBytes, NULL))
                throw KDiskOpError;
        }
        
        ASSERT(!((DWORD)pData & dwPtrMask));
        if(dataLen > 0)
        {
            if(!WriteFile(iDevHandle, pData, dataLen, &dwBytes, NULL))
                throw KDiskOpError;
        }
    
    }
    catch(TInt nErrId)
    {//-- some disk operation finished with the error
        (void)nErrId;
        ASSERT(nErrId == KDiskOpError);
        const DWORD dwWinErr = GetLastError();
        const TInt  epocErr = MapWinError(dwWinErr);
        
        __PRINT2(_L("#-- CWinImgFileDevice::Write() error! WinErr:%d, EpocErr:%d"), dwWinErr, epocErr);
        ASSERT(epocErr != KErrNone);
        return epocErr;
    }


    
    return KErrNone;
}


//-----------------------------------------------------------------------------

/**
    Make the best effort to map Windows error codes (from GetLastError()) to Epos ones.
    
    @param aWinError MS Windows error code
    @return corresponding EPOC eror code
*/

TInt CWinMediaDeviceBase::MapWinError(DWORD aWinError) const
{
    switch(aWinError)
    {
        case NO_ERROR:
        return KErrNone;
                          
        case ERROR_NOT_READY:
        return KErrNotReady;
        
        case ERROR_WRITE_PROTECT:
        case ERROR_ACCESS_DENIED:
        return KErrAccessDenied;
        
        case ERROR_INVALID_HANDLE:
        return KErrBadHandle;
        
        case ERROR_NOT_ENOUGH_MEMORY:
        return KErrNoMemory;
        
        case ERROR_OUTOFMEMORY:
        return KErrDiskFull;
                                        
        case ERROR_CRC:
        return KErrCorrupt;

        case ERROR_WRITE_FAULT:
        return KErrWrite;

        case ERROR_GEN_FAILURE:
        return KErrGeneral;

        case ERROR_LOCK_VIOLATION:
        return KErrLocked;

        case ERROR_SHARING_VIOLATION:
        return KErrInUse;

        case ERROR_NOT_SUPPORTED:
        return KErrNotSupported;

        default:
        return KErrGeneral;
    
    }
}






























