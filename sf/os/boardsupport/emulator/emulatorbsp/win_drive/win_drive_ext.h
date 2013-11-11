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
// Definitions for this file system extension
// 
//

/**
 @file
*/

#ifndef WIN_DRIVE_EXTENSION_H
#define WIN_DRIVE_EXTENSION_H

#include "win_media.h"

//-----------------------------------------------------------------------------

/**
    This class represents the File System extension (.fxt module) that intercepts media driver operations and
    deals with them on windows filesystem level.
*/
class CWinDrvProxyDrive : public CBaseExtProxyDrive
{
public:
    static CWinDrvProxyDrive* NewL(CProxyDrive* aProxyDrive, CMountCB* aMount);

protected:
    CWinDrvProxyDrive(CProxyDrive* aProxyDrive, CMountCB* aMount);
    virtual ~CWinDrvProxyDrive();

	//-- overrides from the base class
    
    virtual TInt Initialise();
	virtual TInt Dismounted();

	virtual TInt Read(TInt64 aPos,TInt aLength,const TAny* aTrg,TInt aThreadHandle,TInt aOffset,TInt aFlags);
	virtual TInt Read(TInt64 aPos,TInt aLength,const TAny* aTrg,TInt aThreadHandle,TInt aOffset);
	virtual TInt Read(TInt64 aPos,TInt aLength,TDes8& aTrg);
	
	virtual TInt Write(TInt64 aPos,TInt aLength,const TAny* aSrc,TInt aThreadHandle,TInt aOffset,TInt aFlags);
	virtual TInt Write(TInt64 aPos,TInt aLength,const TAny* aSrc,TInt aThreadHandle,TInt aOffset);
	virtual TInt Write(TInt64 aPos,const TDesC8& aSrc);

    virtual TInt Caps(TDes8& anInfo);

    virtual TInt Format(TFormatInfo& anInfo);
    virtual TInt Format(TInt64 aPos,TInt aLength);

    /* -------- virtual base class API --------------
    virtual TInt Enlarge(TInt aLength);
	virtual TInt ReduceSize(TInt aPos, TInt aLength);
	
    # virtual TInt Read(TInt64 aPos,TInt aLength,const TAny* aTrg,TInt aThreadHandle,TInt aOffset,TInt aFlags);
	# virtual TInt Read(TInt64 aPos,TInt aLength,const TAny* aTrg,TInt aThreadHandle,TInt anOffset);
	# virtual TInt Read(TInt64 aPos,TInt aLength,TDes8& aTrg);

	# virtual TInt Write(TInt64 aPos,TInt aLength,const TAny* aSrc,TInt aThreadHandle,TInt aOffset,TInt aFlags);
	# virtual TInt Write(TInt64 aPos,TInt aLength,const TAny* aSrc,TInt aThreadHandle,TInt anOffset);
	# virtual TInt Write(TInt64 aPos,const TDesC8& aSrc);
	
    # virtual TInt Caps(TDes8& anInfo);
	#virtual TInt Format(TFormatInfo& anInfo);
	#virtual TInt Format(TInt64 aPos,TInt aLength);
	virtual TInt SetMountInfo(const TDesC8* aMountInfo,TInt aMountInfoThreadHandle=KCurrentThreadHandle);
	virtual TInt ForceRemount(TUint aFlags=0);
	virtual TInt Unlock(TMediaPassword &aPassword, TBool aStorePassword);
	virtual TInt Lock(TMediaPassword &aOldPassword, TMediaPassword &aNewPassword, TBool aStorePassword);
	virtual TInt Clear(TMediaPassword &aPassword);
	virtual TInt ControlIO(const RMessagePtr2& aMessage,TInt aCommand,TAny* aParam1,TAny* aParam2);
	virtual TInt ErasePassword();
	virtual TInt GetLastErrorInfo(TDes8& aErrorInfo);
    virtual TInt DeleteNotify(TInt64 aPos, TInt aLength);
    */
protected:
	virtual TInt GetInterface(TInt aInterfaceId,TAny*& aInterface,TAny* aInput);	


protected:
    CMountCB*    iMount;        ///< pointer to the owning mount
    CProxyDrive* iProxyDrive;   ///< pointer to the chain of the proxies to the native media driver
    CMediaBase*  ipMedia;       ///< pointer to the base "media" object that implements this extension't IO operations

    RBuf8        iScratchBuf;   ///< scratch buffer for non-local read/write operations
};

//-----------------------------------------------------------------------------

/**
    This Proxy drive factory class
*/
class CWinDrvProxyDriveFactory : public CProxyDriveFactory
{
 public:
    CWinDrvProxyDriveFactory();
    virtual TInt Install();         
    virtual CProxyDrive* NewProxyDriveL(CProxyDrive* aProxy,CMountCB* aMount);
};


#endif //WIN_DRIVE_EXTENSION_H




































