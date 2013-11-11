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
// Implementation of this File System extension
// 
//

/**
 @file
*/


#include "win_drive_ext.h"


//#############################################################################
//# Proxy drive implementation
CWinDrvProxyDrive* CWinDrvProxyDrive::NewL(CProxyDrive* aProxyDrive, CMountCB* aMount)
{
    CWinDrvProxyDrive* pSelf = new(ELeave) CWinDrvProxyDrive(aProxyDrive, aMount);
    return pSelf;
}


CWinDrvProxyDrive::CWinDrvProxyDrive(CProxyDrive* aProxyDrive, CMountCB* aMount)
                  :CBaseExtProxyDrive(aProxyDrive, aMount)  
{
    ASSERT(aProxyDrive && aMount);

    iMount = aMount;
    iProxyDrive = aProxyDrive;
}

CWinDrvProxyDrive::~CWinDrvProxyDrive()
{
    Dismounted();
}


//-----------------------------------------------------------------------------

/** 
    Initialise the proxy drive (this extension)
    @return standard epoc error code.
*/
TInt CWinDrvProxyDrive::Initialise()
{
    __PRINT(_L("#-- CWinDrvProxyDriveFactory::Initialise()"));

    TInt nRes;
    ASSERT(!ipMedia);

    //-- create a scratch buffer
    nRes = iScratchBuf.CreateMax(65536);
    if(nRes != KErrNone)
        return nRes;

    //-- create media driver object, at present only generic block media supported
    if(!ipMedia)
    {
        const TInt nEpocDrvNum = iMount->DriveNumber();
        ipMedia = new CGenericBlockMedia(nEpocDrvNum);
        if(!ipMedia)
            return KErrNoMemory;
    }

    //-- connect to the media device
    nRes = ipMedia->Connect();
    
    return  nRes;
}


/**
    Dismount this extension.
    @return standard epoc error code.
*/
TInt CWinDrvProxyDrive::Dismounted()
{
    __PRINT(_L("#-- CWinDrvProxyDriveFactory::Dismounted()"));
    
    //-- disconnect from the media driver and delete it
    if(ipMedia)
    {
        ipMedia->Disconnect();
        delete ipMedia;
        ipMedia = NULL;
    }

    iScratchBuf.Close();

    return KErrNone;
}

//-----------------------------------------------------------------------------
TInt CWinDrvProxyDrive::GetInterface(TInt aInterfaceId,TAny*& aInterface,TAny* aInput)
{
	switch(aInterfaceId)
		{
		case ELocalBufferSupport:
			return KErrNone;

		default:
			return CBaseExtProxyDrive::GetInterface(aInterfaceId,aInterface,aInput);
		}
}

//-----------------------------------------------------------------------------

/**
    Read data from the media.
    @param  aPos    media position to start write with.
    @param  aLength number of bytes to read
    @param  destination data descriptor

    @return standard epoc error code.
*/
TInt CWinDrvProxyDrive::Read(TInt64 aPos, TInt aLength, TDes8& aTrg)
{
    //__PRINT3(_L("#-- CWinDrvProxyDrive::Read()#0 pos:%LU, desLen:%d, desMaxLen:%d"),aPos, aTrg.Length(), aTrg.MaxLength());

    if(aTrg.MaxLength() == 0)
        return KErrNone; //-- nothing to do
    
    aTrg.SetLength(0);
    TInt nRes = ipMedia->Read(aPos, aLength, aTrg);
    return nRes;
}


//-- try to find a message slot containing the required address
#define GetIndex(msg, aAddress, aIndex)			\
	aIndex = msg.Ptr0() == aAddress ? 0 :				\
				msg.Ptr1() == aAddress ? 1 :			\
					msg.Ptr1() == aAddress ? 2 :		\
						msg.Ptr1() == aAddress ? 3 : -1;



/**
    Read data from the media.
    @param  aPos            media position to start write with.
    @param  aLength         number of bytes to be written
    @param  aTrg            pointer to the destination data descriptor
    @param  aThreadHandle   can specify remote thread where the data will be copied to.
    @param  aOffset         an offset within the data descriptor to the real data to be written (see aTrg).
    
    @return standard epoc error code.
*/
TInt CWinDrvProxyDrive::Read(TInt64 aPos, TInt aLength, const TAny* aTrg, TInt aThreadHandle, TInt aOffset)
{
    //__PRINT4(_L("#-- CWinDrvProxyDrive::Read()#1 pos:%LU, offset:%d, desLen:%d, desMaxLen:%d"),aPos, aOffset, desData.Length(), desData.MaxLength());
    ASSERT(aTrg);

    //-- N.B. aTrg is a pointer to the data descriptor TDes8 !
    TDes8& orgDes = *((TDes8*)aTrg);

    if(aLength == 0)
        return KErrNone; //-- nothing to do


    if(aOffset < 0 || aOffset+aLength > orgDes.MaxLength())
        return KErrArgument; 

    
    //-- check if we have local or remote message from a different process
    
    TBool localMessage = (aThreadHandle == KLocalMessageHandle);
	if(!localMessage)
	{
		RMessage2 msg(*(RMessagePtr2 *) &aThreadHandle);
		localMessage = (msg.Handle() == KLocalMessageHandle);
	}

    if(localMessage)
    {//-- we can just copy data to the descriptor; it is the same process
        TPtr8  desData((TUint8*)(orgDes.Ptr()+aOffset), orgDes.Length()-aOffset);
        return ipMedia->Read(aPos, aLength, desData);
    }
    else
    {//-- we have to read data to the local buffer first and then write to another process 
        TInt  msgSlotIdx = 0;
        RMessage2 msg(*(RMessagePtr2 *) &aThreadHandle);

		GetIndex(msg, aTrg, msgSlotIdx); //-- find out message slot index containing the desctiptor data
		if (msgSlotIdx < 0)
			return KErrArgument;
   
        TInt  rem = aLength;
        TInt  localOffset = 0;
        
        while(rem)
        {
            const TInt bytesToRead = Min(rem, iScratchBuf.Size());
            TPtr8 ptrBuf((TUint8*)iScratchBuf.Ptr(), bytesToRead);

            //-- read chunk of data from the media to the local buffer
            TInt nRes = ipMedia->Read(aPos + localOffset, bytesToRead, ptrBuf);
            if(nRes != KErrNone)
                return nRes; //-- media read error

            //-- write data from the local buffer to another process
            nRes = msg.Write(msgSlotIdx, ptrBuf, aOffset+localOffset);
            if(nRes != KErrNone)
                return nRes; 

            rem -= bytesToRead;
            localOffset += bytesToRead;
        }
    
        return KErrNone;
    }
    
    
}

/** the same as above */
TInt CWinDrvProxyDrive::Read(TInt64 aPos,TInt aLength, const TAny* aTrg,TInt aThreadHandle, TInt aOffset,TInt /*aFlags*/)
{
    //__PRINT4(_L("#-- CWinDrvProxyDrive::Read()#2 pos:%LU, offset:%d, desLen:%d, desMaxLen:%d"),aPos, aOffset, desData.Length(), desData.MaxLength());
    return Read(aPos, aLength, aTrg, aThreadHandle, aOffset);
}


//-----------------------------------------------------------------------------

/**
    Write data to the media.
    @param  aPos    media position to start write with.
    @param  aSrc    data descriptor

    @return standard epoc error code.
*/
TInt CWinDrvProxyDrive::Write(TInt64 aPos, const TDesC8& aSrc)
{
    //__PRINT2(_L("#-- CWinDrvProxyDrive::Write()#0 pos:%LU, desLen:%d"),aPos, aSrc.Length());

    if(aSrc.Length() == 0 )
        return KErrNone; //-- nothing to do

    TInt nRes = ipMedia->Write(aPos, aSrc.Size(), aSrc);

    return nRes;
}

/**
    Write data to the media.
    @param  aPos            media position to start write with.
    @param  aLength         number of bytes to be written
    @param  aSrc            pointer to the data descriptor
    @param  aThreadHandle   can specify remote thread where the data will be got from.
    @param  aOffset         an offset within the data descriptor to the real data to be written (see aSrc).
    
    @return standard epoc error code.
*/
TInt CWinDrvProxyDrive::Write(TInt64 aPos, TInt aLength, const TAny* aSrc, TInt aThreadHandle, TInt aOffset)
{
    //__PRINT3(_L("#-- CWinDrvProxyDrive::Write()#1 pos:%LU, offset:%d, desLen:%d"),aPos, aOffset, orgDes.Length());
    
    ASSERT(aSrc);
        
    TDesC8& orgDes = *((TDesC8*)aSrc);

    if(orgDes.Length() == 0 || aLength == 0)
        return KErrNone; //-- nothing to do

    if(aOffset < 0 || aOffset+aLength > orgDes.Length())
        return KErrArgument; 
    
   
    //-- check if we have local or remote message from a different process
    
    TBool localMessage = (aThreadHandle == KLocalMessageHandle);
	if(!localMessage)
	{
		RMessage2 msg(*(RMessagePtr2 *) &aThreadHandle);
		localMessage = (msg.Handle() == KLocalMessageHandle);
	}

    if(localMessage)
    {//-- we can just copy data from the descriptor; it is the same process
    TPtrC8  dataDes(orgDes.Ptr()+aOffset, orgDes.Length()-aOffset);
    return ipMedia->Write(aPos, aLength, dataDes);
}
    else
    {//-- we have to copy data to the local buffer first and then write to the media.
        TInt  msgSlotIdx = 0;
        
        RMessage2 msg(*(RMessagePtr2 *) &aThreadHandle);
		GetIndex(msg, aSrc, msgSlotIdx);

		if (msgSlotIdx < 0)
			return KErrArgument;

        TInt  rem = aLength;
        TInt  localOffset = 0;

        while(rem)
        {
            const TInt bytesToWrite = Min(rem, iScratchBuf.Size());
            
            TInt nRes = msg.Read(msgSlotIdx, iScratchBuf, aOffset+localOffset);
            if(nRes != KErrNone)
                return nRes; 

            nRes = ipMedia->Write(aPos+localOffset, bytesToWrite, iScratchBuf);
            if(nRes != KErrNone)
                return nRes; 


            rem -= bytesToWrite;
            localOffset += bytesToWrite;
        }
    
        return KErrNone;
    }
   
}



/** the same as above */
TInt CWinDrvProxyDrive::Write(TInt64 aPos,TInt aLength,const TAny* aSrc,TInt aThreadHandle, TInt aOffset,TInt /*aFlags*/)
{
    //__PRINT3(_L("#-- CWinDrvProxyDrive::Write()#2 pos:%LU, offset:%d, desLen:%d"),aPos, aOffset, orgDes.Length());
    return Write(aPos, aLength, aSrc, aThreadHandle, aOffset);
}


//-----------------------------------------------------------------------------

/** 
    get drive capabilities 
    @param anInfo packed descriptor of TLocalDriveCaps
    @return standard Epoc error code
*/
TInt CWinDrvProxyDrive::Caps(TDes8& anInfo)
{
    TInt nRes;
    
    //-- get capabilities of the EPOC drive we are overriding
    nRes = CBaseExtProxyDrive::Caps(anInfo);
    if(nRes != KErrNone)
        return nRes;

    TLocalDriveCaps& orgCaps = ((TLocalDriveCapsBuf&)anInfo)();

    //-- override capabilities with the data from our device.
    ipMedia->GetEpocCaps(orgCaps);
    
    return KErrNone;
}

//-----------------------------------------------------------------------------

/** 
    Format whole media.
    This method can be called many times, each call represents the formatting step.

    @param  anInfo specifies the formatting parameters.    
    @return KErrNone if the formatting step has  gone OK.
            KErrEof  if the formatting has finished
            standard epoc error code otherwise.

*/
TInt CWinDrvProxyDrive::Format(TFormatInfo& anInfo)
{
    return ipMedia->Format(anInfo);
}

/** 
    format part of the media 
    @param  aPos    starting media position
    @param  aLength length of the media space to be formatted.

    @return standard epoc error code
*/
TInt CWinDrvProxyDrive::Format(TInt64 aPos, TInt aLength)
{
    return ipMedia->Format(aPos, aLength);
}


//-----------------------------------------------------------------------------


//#############################################################################
//# Extension factory implementation

CWinDrvProxyDriveFactory::CWinDrvProxyDriveFactory()
{
}

TInt CWinDrvProxyDriveFactory::Install()
{
    _LIT(KExtensionName,"Win_Drive"); //-- this extensoin name.

    __PRINT(_L("#-- CWinDrvProxyDriveFactory::Install()"));
    return(SetName(&KExtensionName));
}

CProxyDrive* CWinDrvProxyDriveFactory::NewProxyDriveL(CProxyDrive* aProxy, CMountCB* aMount)
{
    __PRINT(_L("#-- CWinDrvProxyDriveFactory::NewProxyDriveL()"));
    return (CWinDrvProxyDrive::NewL(aProxy, aMount));
}


/** DLL entrypoint */
extern "C" 
{
    EXPORT_C CProxyDriveFactory* CreateFileSystem()
    {
        __PRINT(_L("#-- Win_Drive extension CreateFileSystem()"));
        return new CWinDrvProxyDriveFactory();
    }
}




















