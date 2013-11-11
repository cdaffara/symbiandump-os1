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

#ifndef A3FCISTUBEXTN_H
#define A3FCISTUBEXTN_H

#include <e32base.h>
#include <a3f/mmfdevsoundcustominterfaceextensions.h>

#include <a3f/maudiocontext.h>
#include <a3f/maudiocontextobserver.h>

#include "testsetvol.h"

class CSetVol;

enum TSetVolCommands
    {
    ETestSetVolIfSetVolCommand,
    ETestSetVolIfVolCommand,
    };

/**
*
* CA3fClientCiStubExtn
*
* stub version of MDevSoundCIClientExtension 
* Always fails to open CIs, but simulates presence of object
*/

class CA3fClientCiStubExtn : public CBase,
			   		   		 public MDevSoundCIClientExtension,
			   		   		 public MTestSetVolIf
			      	 					
	{
public:
	static MDevSoundCIClientExtension* NewL();
	~CA3fClientCiStubExtn();

protected:	
	// from MDevSoundCIClientExtension
	TInt Setup(MCustomCommand& aCustomCommand);
	TInt CustomInterfaceExtension(TUid aUid, TAny*& aInterface);
	void Release();
	void PassDestructorKey(TUid aDestructorKey);
	
	// from MTestSetVolIf
	TInt SetVol(TInt aVolume);
	TInt Vol(TInt aMaxVolume);
	
protected:
	TUid iKey;
	MCustomCommand* iCommand; // not owned
	HBufC* iDummyBuffer;
	};


/**
*
* CA3fServerCiStubExtn
*
* stub version of MDevSoundCIServerExtension 
* Always fails to open CIs, but simulates presence of object
*/

class CA3fServerCiStubExtn : public CBase,
			   		   		 public MDevSoundCIServerExtension
			      	 					
	{
public:
	static MDevSoundCIServerExtension* NewL();
	~CA3fServerCiStubExtn();

protected:	
	// from MDevSoundCIServerExtension
	TInt Setup(MCustomInterface& aInterface);
	TInt HandleMessageL(const RMmfIpcMessage& aMessage);
	void Release();
	void PassDestructorKey(TUid aDestructorKey);
	
protected:
	TUid iKey;
	MCustomInterface* iInterface; // not owned
	HBufC* iDummyBuffer;
	CSetVol* iSetVol;
	};
	
// CAVEAT - for testing only
class CSetVol : public CBase,
				public MAudioContextObserver
	{
public:
	static CSetVol* NewL(MCustomInterface& aCustomInterface);
	~CSetVol();

	TInt HandleMessageL(const RMmfIpcMessage& aMessage);
	
	// from MAudioContextObserver
	void ContextEvent(TUid aEvent, TInt aError);
	
private:
	CSetVol(MCustomInterface& aCustomInterface);
	void ConstructL();
	
	void UpdateA3fPointers();
	void SetVolumeL(TInt aVol);
	TInt VolumeL(TInt aMaxVolume);
	
	MCustomInterface* iInterface; // not owned
	MAudioContext* iContext;
	MAudioStream* iStream;
	MAudioProcessingUnit* iGain;
	
	CActiveSchedulerWait* iWait;
	TInt iError;
	};


#endif 
