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
// MMFDevSoundCIDeMuxUtility.cpp
// 
//

#ifndef MMFDEVSOUNDCIDEMUXUTILITY_H
#define MMFDEVSOUNDCIDEMUXUTILITY_H

#include <f32file.h>
#include <e32math.h>
#include <s32mem.h> 
#include "mmfdevsoundserverstart.h"
#include "mmfaudioclientserver.h"
#include <mmf/server/mmfdevsoundcustominterface.h>


/**
* @internalTechnology
* @file
*/

class CMMFDevSoundCIDeMuxUtility;
NONSHARABLE_CLASS( CMMFDevSoundCIDeMuxUtility ) :  public CBase,
												   public MMMFDevSoundCustomInterfaceDeMuxUtility
	{
public:
	static CMMFDevSoundCIDeMuxUtility* NewL(MMMFDevSoundCustomInterfaceDeMuxInterface* aInterface);
	virtual ~CMMFDevSoundCIDeMuxUtility();
	void ConstructL();

	// create a custom interface Mux implementation
	MMMFDevSoundCustomInterfaceDeMuxPlugin* CreateCustomInterfaceDeMuxL(TUid aInterfaceId);
	
	// check and process custom commands
	TInt ProcessCustomInterfaceCommandL(const RMmfIpcMessage& aMessage);
	
	// utility functions from MMMFDevSoundCustomInterfaceDeMuxUtility
	// get all settings at once into client supplied package
	virtual void GetSyncMessageDataL(const RMmfIpcMessage& aMessage, TMMFDevSoundCIMessageData& aData);
	virtual void GetAsyncMessageDataL(const RMmfIpcMessage& aMessage, TMMFDevSoundCIMessageData& aData);
	
	// read and write to input and output descriptors
	virtual TInt InputDesLength(const RMmfIpcMessage& aMessage);
	virtual void ReadFromInputDesL(const RMmfIpcMessage& aMessage, TDes8* aBufToFill);
	virtual void WriteToOutputDesL(const RMmfIpcMessage& aMessage, TDesC8& aBufToWrite);
	
	// message related
	virtual void CompleteMessage(const RMmfIpcMessage& aMessage, TInt aError);
private:
	CMMFDevSoundCIDeMuxUtility(MMMFDevSoundCustomInterfaceDeMuxInterface* aInterface);
	MMMFDevSoundCustomInterfaceDeMuxInterface* iInterface;
	};





#endif 
