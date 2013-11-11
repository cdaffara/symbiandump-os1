// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_DEVSOUND_TESTINTERFACE_H__
#define __TSU_MMF_DEVSOUND_TESTINTERFACE_H__

#include <e32base.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include <mmf/server/mmfdevsoundcustominterface.h>
#include <mmf/server/mmfdevsoundcustomcommands.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


_LIT8(KDevSoundCITestIn, "DON'T");
_LIT8(KDevSoundCITestOut, "PANIC");


enum TMMFDevSoundCITestCommands
	{
	EMMFDevSoundCITestSync = 0,
	EMMFDevSoundCITestSyncResult,
	EMMFDevSoundCITestAsync,
	EMMFDevSoundCITestAsyncResult
	};

// test custom interface
class MMMFDevSoundTestCustomInterface
	{
public:
	virtual TInt TestSyncCommand(const TDesC8& aDes) = 0;
	virtual TInt TestSyncCommandResult(const TDesC8& aDes, TDes8& aRes) = 0;
	virtual void TestAsyncCommandL(const TDesC8& aDes, TRequestStatus& aStatus) = 0;
	virtual void TestAsyncCommandResultL(const TDesC8& aDes, TDes8& aRes, TRequestStatus& aStatus) = 0;
	};


/**
*
* CMMF_TSU_DEVSOUND_TestInterfaceMux
*
* implementation of the test custom interface pair
*/

class CMMF_TSU_DEVSOUND_TestInterfaceMux : public CActive,
			      	 					   public MMMFDevSoundCustomInterfaceMuxPlugin,
			      	 					   public MMMFDevSoundTestCustomInterface
			      	 					
	{
public:
	virtual TInt OpenInterface(TUid aInterfaceId);
	virtual void Release();
	virtual void PassDestructorKey(TUid aDestructorKey);
	virtual void CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility);
	virtual TAny* CustomInterface(TUid aInterfaceId);

	static MMMFDevSoundCustomInterfaceMuxPlugin* NewL();
	~CMMF_TSU_DEVSOUND_TestInterfaceMux();

	// from CActive
	virtual void RunL();
	virtual void DoCancel();
		
protected:
	CMMF_TSU_DEVSOUND_TestInterfaceMux();
	
	// test interface
	virtual TInt TestSyncCommand(const TDesC8& aDes);
	virtual TInt TestSyncCommandResult(const TDesC8& aDes, TDes8& aRes);
	virtual void TestAsyncCommandL(const TDesC8& aDes, TRequestStatus& aStatus);
	virtual void TestAsyncCommandResultL(const TDesC8& aDes, TDes8& aRes, TRequestStatus& aStatus);

protected:
	TUid iKey;
	TInt iRemoteHandle;
	
	TMMFDevSoundCustomInterfaceCommandPackage iComPackage;
	TRequestStatus* iClientStatus;
	
	MMMFDevSoundCustomInterfaceMuxUtility* iUtility;
	};



class CMMF_TSU_DEVSOUND_TestInterfaceDeMux : public CBase,
			      	   						 public MMMFDevSoundCustomInterfaceDeMuxPlugin
	{
public:
	static MMMFDevSoundCustomInterfaceDeMuxPlugin* NewL();
	~CMMF_TSU_DEVSOUND_TestInterfaceDeMux();

	virtual TInt OpenInterface(TUid aInterfaceId);
	virtual void Release();
	virtual void PassDestructorKey(TUid aDestructorKey);
	virtual void SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget);
	virtual void CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aUtility);
	virtual void InterfaceDeleted();
	
	virtual void RefreshL();

	// from MMMFDeMuxInterface
	virtual TInt DoOpenSlaveL(TUid aInterface, const TDesC8& aPackageBuf);
	virtual void DoCloseSlaveL(TInt aHandle);

	// original RMessage is supplied so that remote demux plugin can extract necessary details
	// using DeMux utility
	virtual TInt DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage);
	virtual TInt DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage);
	virtual void DoSendSlaveAsyncCommandL(const RMmfIpcMessage& aMessage);
	virtual void DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& aMessage);

protected:
	CMMF_TSU_DEVSOUND_TestInterfaceDeMux();

protected:
	MMMFDevSoundCustomInterfaceDeMuxUtility* iUtility;
	MMMFDevSoundCustomInterfaceTarget* iTarget;
	TUid iKey;
};

#endif 
