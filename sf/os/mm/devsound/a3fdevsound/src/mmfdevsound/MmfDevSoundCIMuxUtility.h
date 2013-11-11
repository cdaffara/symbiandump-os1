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

#ifndef MMFDEVSOUNDCIMUXUTILITY_H
#define MMFDEVSOUNDCIMUXUTILITY_H

#include <f32file.h>
#include <e32math.h>
#include <s32mem.h> 
#include "mmfaudioclientserver.h"
#include <mmf/server/mmfdevsoundcustominterface.h>


/**
* @internalTechnology
* @file
*/

class CMMFDevSoundCIMuxUtility;
class TA3FCustomInterfaceCommand;
NONSHARABLE_CLASS( CMMFDevSoundCIMuxUtility ) : public CBase, 
												public MMMFDevSoundCustomInterfaceMuxUtility
	{
public:
	enum TMMFDevSoundCustomCommand
		{
		EMMFDevSoundCustomCommandCIOpenSlave,
		EMMFDevSoundCustomCommandCICloseSlave,
		EMMFDevSoundCustomCommandCISendSlaveSyncCommand,
		EMMFDevSoundCustomCommandCISendSlaveSyncCommandResult,
		EMMFDevSoundCustomCommandCISendSlaveAsyncCommand,
		EMMFDevSoundCustomCommandCISendSlaveAsyncCommandResult
		};
		
		/*
	* internal class used for Asynchronous Custom Command message passing from CI Mux to Demux.
	*/

	NONSHARABLE_CLASS(CAsyncCommandCleanup): public CActive
		{

	public:
			
		static CAsyncCommandCleanup* NewL(CMMFDevSoundCIMuxUtility* aMuxUtility,MMMFDevSoundCustomInterfaceChannel* aCustomChannel);
		~CAsyncCommandCleanup();
		void AsyncCustomCommand(CMMFDevSoundCIMuxUtility::TMMFDevSoundCustomCommand aType,TMMFDevSoundCustomInterfaceCommandPackage& aComPackage, TRequestStatus& aStatus, const TDesC8& aPackageBuf, TDes8* aResultBuf);

	private:
		void ConstructL();
		void RunL();
		void DoCancel();
		CAsyncCommandCleanup(CMMFDevSoundCIMuxUtility* aMuxUtility,MMMFDevSoundCustomInterfaceChannel* aCustomChannel);

	public:
		TPckgBuf<TA3FCustomInterfaceCommand>* iCommandBuffer;

	private:

		CMMFDevSoundCIMuxUtility* iMuxUtility;
		MMMFDevSoundCustomInterfaceChannel* iCustomChannel;		
		TRequestStatus* iClientRequestStatus;
		};

public:
	static CMMFDevSoundCIMuxUtility* NewL(MMMFDevSoundCustomInterfaceChannel* aCustomChannel);
	virtual ~CMMFDevSoundCIMuxUtility();
	void ConstructL();

	// create a custom interface Mux implementation.
	MMMFDevSoundCustomInterfaceMuxPlugin* CreateCustomInterfaceMuxL(TUid aInterfaceId);
	
	// from MMMFDevSoundCustomInterfaceMux interface.
	virtual TInt OpenSlave(TUid, const TDesC8& aPackageBuf);
	virtual void CloseSlave(TInt aHandle);
	virtual TInt SendSlaveSyncCommand(TInt aHandle, TInt aCommand, const TDesC8& aPackageBuf);
	virtual TInt SendSlaveSyncCommandResult(TInt aHandle, TInt aCommand, const TDesC8& aPackageBuf, TDes8& aResultBuf); 
	virtual void SendSlaveAsyncCommand(TMMFDevSoundCustomInterfaceCommandPackage& aComPackage, TRequestStatus& aStatus, const TDesC8& aPackageBuf);
	virtual void SendSlaveAsyncCommandResult(TMMFDevSoundCustomInterfaceCommandPackage& aComPackage, TRequestStatus& aStatus, const TDesC8& aPackageBuf, TDes8& aResultBuf);	
	
	void RemoveAsyncCommand(CAsyncCommandCleanup* aAsyncCustomCommandCleanup);
private:
	CMMFDevSoundCIMuxUtility(MMMFDevSoundCustomInterfaceChannel* aCustomChannel);
	// Not owned.
	MMMFDevSoundCustomInterfaceChannel* iCustomChannel;
	RPointerArray <CAsyncCommandCleanup> iAsyncCustomCommandCleanup;
	};


/*
* internal class used to pass custom command & custom command type from CI Mux to DeMux.
*/
class TA3FCustomInterfaceCommand
	{
public:
	// Custom Interface command (meaning depends on CI in use)
	TInt iCommand;
	// Internal slave command type.
	CMMFDevSoundCIMuxUtility::TMMFDevSoundCustomCommand iType;
	// CI handle (equates to an offset into array of open CIs for most command types)
	TInt iHandle;
	};
	

#endif 
