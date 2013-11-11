// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\specific\mmcv.cpp
// WINS variant
// 
//

#include <mmccd_ifc.h>
#include "mmci.h"
#include "variantmediadef.h"

//
// Factory class for creation of platform specifics
//
class TMMCardControllerInterfaceWins : public TMMCardControllerInterface
	{
	// Factory functions
	virtual DMMCSocket* NewSocket(TInt aSocketNum, TMMCPasswordStore* aPasswordStore);
	virtual DMMCStack* NewStack(TInt aStackNum, DMMCSocket* aSocket);
	virtual DMMCMediaChange* NewMediaChange(TInt aMcId);
	virtual DMMCPsu* NewVcc(TInt aVccNum, TInt aMcId);
	// Stack initialisation
	virtual TInt Init();
	// Machine configuration
	virtual TBool IsMMCSocket(TInt aSocket,SMediaDeviceInfo& aMediaDeviceInfo);
	virtual TInt  MediaChangeID(TInt aSocket);
	virtual TInt  VccID(TInt aSocket);
    };


TInt TMMCardControllerInterfaceWins::Init()
//
// Initialise the Controller h/w
// 
	{
	return KErrNone;
	}

TInt TMMCardControllerInterfaceWins::MediaChangeID(TInt aSocket)
//
// Return the socket's media change index
//
	{
	return aSocket;
	}

TInt TMMCardControllerInterfaceWins::VccID(TInt aSocket)
//
// Return the socket's PSU index
//
	{
	return aSocket;
	}

static const TInt DriveNumbers[MMC0_DRIVECOUNT]={MMC0_DRIVELIST};
_LIT(KLitMmcSocketName,MMC0_DRIVENAME);
TBool TMMCardControllerInterfaceWins::IsMMCSocket(TInt aSocket,SMediaDeviceInfo& aMediaDeviceInfo)
//
// Confirm whether an MMC stack is supported on the specified
// socket and if it is, the Media Info. for that socket.
//
	{
	
	if (aSocket==0)
		{
		aMediaDeviceInfo.iDevice=MEDIA_DEVICE_MMC;
		aMediaDeviceInfo.iDriveCount=MMC0_DRIVECOUNT;
		aMediaDeviceInfo.iDriveList=&DriveNumbers[0];
		aMediaDeviceInfo.iNumMedia=MMC0_NUMMEDIA;
		aMediaDeviceInfo.iDeviceName=&KLitMmcSocketName;
		return(ETrue);
		}
	else
		return(EFalse);	
	}

DMMCSocket* TMMCardControllerInterfaceWins::NewSocket(TInt aSocketNum, TMMCPasswordStore* aPasswordStore)
//
// Create a new platform-specific socket
//
	{
	return new DMMCSocket(aSocketNum, aPasswordStore);
	}

DMMCStack* TMMCardControllerInterfaceWins::NewStack(TInt aStackNum, DMMCSocket* aSocket)
//
// Create a new platform-specific stack
//
	{
	return new DWinsMMCStack(aStackNum, aSocket);
	}

DMMCMediaChange* TMMCardControllerInterfaceWins::NewMediaChange(TInt aMcId)
//
// Create a new platform-specific media change
//
	{
	return new DWinsMMCMediaChange(aMcId);
	}

DMMCPsu* TMMCardControllerInterfaceWins::NewVcc(TInt aVccNum, TInt aMcId)
//
// Create a new platform-specific PSU
//
	{
	return new DWinsMMCPsu(aVccNum, aMcId);
	}

DECLARE_STANDARD_EXTENSION()
//
// Extension Entry Point
//
	{
	__KTRACE_OPT(KPBUS1,Kern::Printf("Starting MMC interface"));

	TInt r=KErrNoMemory;
	TMMCardControllerInterfaceWins* pI=new TMMCardControllerInterfaceWins;
	if (pI)
		r=pI->Create();

	__KTRACE_OPT(KPBUS1,Kern::Printf("Returns %d",r));
	return r;
	}

