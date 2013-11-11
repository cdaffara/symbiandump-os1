// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#if !defined(__C32EXE_H__)
#define __C32EXE_H__

/**
@file

@internalComponent
@released
*/

#include <rsstd.h>
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
#include <comms-infras/commsbufpondop.h>
#else
#include <es_mbman.h>
#endif // SYMBIAN_ZERO_COPY_NETWORKING
#include <mb_thread.h>
#include "c32policy.h"

namespace RootServer
{

class CC32ProviderModule : public CCommsProviderModule
/** C32 process specific rootserver extensions.
@internalComponent 
@released
*/
    {
public:
 
    static CCommsProviderModule* NewL(CRootServer* aRootServer, const TRSStartModuleParams& aParams, HBufC8* aIniData);
 
private:
    CC32ProviderModule() :
        CCommsProviderModule(iRCFThread)
        {
        }
    virtual TInt CreateModuleThread(const TDesC& aName, TThreadFunction aOtherThreadFunction,
						TInt aStackSize, RHeap* aHeap, TAny* aModuleArgs);
private:
        /** Thread handle of running module context. 
	 */
	CommsFW::RCFThread iRCFThread;			
    };

class CC32RootServer : public CRootServer
/** C32 process specific rootserver extensions.
@internalComponent 
@released
*/
    {
public:
    ~CC32RootServer();
    
    static CRootServer* NewL(const TRootServerStartupInfo& aRootServerStartupInfo);
 
    virtual void LazyLoadL();
    virtual TInt AdjustMBufPool(const TRSSetMBufMnr &aMBufParams);
	virtual TInt AdjustMBufSizeAllocInfo(const TRSAddMBufAllocInfo &aMBufParamsPckg);
	virtual TBool IsCallerConfigurator(const CRootServerSession* aSession, const RMessage2& aMessage);
	virtual void DisconnectSession(const CRootServerSession* aSession);

	virtual CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
#ifdef _DEBUG
    virtual TInt RunError( TInt aError );
#endif //_DEBUG

private:
    CC32RootServer(TInt aPriority):
        CRootServer(aPriority,CC32RootServerPolicy)
        {
        }

private:
	RArray<TRSAddMBufAllocInfoContainer> iMBufSizeAllocInfo;
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
    RCommsBufPondOp iCommsBufPond;
#else
	TUint iInitMBufPoolSize;
	TUint iMaxMBufPoolSize;

	CMBufManager* iMBufManager;
	MMBufSizeAllocator *iMBufSizeAllocator;
#endif // SYMBIAN_ZERO_COPY_NETWORKING    
    const CRootServerSession* iTheConfiguratorSession;
    };

class CC32RootServerSession : public CRootServerSession
/**  C32 process specific RootServer Session extensions.
@internalComponent 
@released
*/
    {
public:
	CC32RootServerSession(const CRootServer* aRootServer);
	virtual void ServiceL(const RMessage2& aMessage);
	~CC32RootServerSession();

private:
	TInt SetMBufPoolSize(const RMessage2& aMessage);
	virtual TInt IsCallerConfigurator(const RMessage2& aMessage);    
	TInt AddMBufAllocInfo(const RMessage2& aMessage);
	
    };
}
#endif // __C32ROOT_H__
