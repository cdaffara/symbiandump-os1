// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ECheckDevicePairing state in the PSY Connecting State Machine; checks to
// see whether the current device is paired with
//



/**
 @file
 @internalTechnology
*/

#ifndef BTGPSCHECKDEVICEPAIRING_H
#define BTGPSCHECKDEVICEPAIRING_H

//Includes
#include "btgpsdevman.h"

//Forward Declarations
class MBTGPSConnectManagerExt;

/** The Handler which handles checking the pairing status of the current
 BT GPS device
 
 @internalTechnology
 */
class CBTGPSCheckDevicePairing : public CBase, private MBTGPSDevManObserver
	{
public:
	static CBTGPSCheckDevicePairing* NewL(MBTGPSConnectManagerExt& aManagerExt);
	~CBTGPSCheckDevicePairing();
		
private:
	CBTGPSCheckDevicePairing(MBTGPSConnectManagerExt& aManagerExt);
	void ConstructL();
	
	//MBTGPSDevManObserver
	virtual void HandleGetDevicesComplete(TInt aErr, RBTDeviceArray* aDeviceArray);    
	virtual void HandleAddDeviceComplete(TInt aErr);
	virtual void HandleDeleteDevicesComplete(TInt aErr);
	
	static TInt HandlerCompleteCallback(TAny* aAny);
	void HandlerCompleteNotify();
	
private:
	//Manager Ext
	MBTGPSConnectManagerExt& iManagerExt;
	
	//BT Device Manager
	CBTGPSDevMan* iBtDevMan;        
	
	// result array needed when using CBTGPSDevMan
	RBTDeviceArray* iDeviceArray;
	
	//Error code
	TInt iError;
		
	//Complete Idle object
	CIdle* iIdle;
	};

#endif	//BTGPSCHECKDEVICEPAIRING_H
