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
// MmfDevSoundCIMuxUtility.cpp
// 
//

#ifndef MMFDEVSOUNDCIMUXUTILITY_H
#define MMFDEVSOUNDCIMUXUTILITY_H

#include <f32file.h>
#include <e32math.h>
#include <s32mem.h> 
#include "MmfDevSoundServerStart.h"
#include "MmfAudioClientServer.h"
#include <mmf/server/mmfdevsoundcustominterface.h>


/**
* @internalTechnology
* @file
*/

class CMMFDevSoundCIMuxUtility;
NONSHARABLE_CLASS( CMMFDevSoundCIMuxUtility ) : public CBase, 
												public MMMFDevSoundCustomInterfaceMuxUtility
	{
public:
	static CMMFDevSoundCIMuxUtility* NewL(MMMFDevSoundCustomInterfaceChannel* aCustomChannel);
	virtual ~CMMFDevSoundCIMuxUtility();
	void ConstructL();

	// create a custom interface Mux implementation
	MMMFDevSoundCustomInterfaceMuxPlugin* CreateCustomInterfaceMuxL(TUid aInterfaceId);
	
	// from MMMFDevSoundCustomInterfaceMux interface
	virtual TInt OpenSlave(TUid, const TDesC8& aPackageBuf);
	virtual void CloseSlave(TInt aHandle);
	virtual TInt SendSlaveSyncCommand(TInt aHandle, TInt aCommand, const TDesC8& aPackageBuf);
	virtual TInt SendSlaveSyncCommandResult(TInt aHandle, TInt aCommand, const TDesC8& aPackageBuf, TDes8& aResultBuf); 
	virtual void SendSlaveAsyncCommand(TMMFDevSoundCustomInterfaceCommandPackage& aComPackage, TRequestStatus& aStatus, const TDesC8& aPackageBuf);
	virtual void SendSlaveAsyncCommandResult(TMMFDevSoundCustomInterfaceCommandPackage& aComPackage, TRequestStatus& aStatus, const TDesC8& aPackageBuf, TDes8& aResultBuf);	
private:
	CMMFDevSoundCIMuxUtility(MMMFDevSoundCustomInterfaceChannel* aCustomChannel);

	MMMFDevSoundCustomInterfaceChannel* iCustomChannel;
	};





#endif 
