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
// EGetNextDevice state in the PSY Connecting State Machine; should return
// the next device from the BT GPS Configuration API
//



/**
 @file
 @internalTechnology
*/

#ifndef BTGPSGETNEXTDEVICE_H
#define BTGPSGETNEXTDEVICE_H

//Forward Declarations
class MBTGPSConnectManagerExt;

/** The Handler which handles getting the next device from the list
 of BT GPS devices.
 
 @internalTechnology
 */
class CBTGPSGetNextDevice : public CBase
	{
public:
	static CBTGPSGetNextDevice* NewL(MBTGPSConnectManagerExt& aManagerExt);
	~CBTGPSGetNextDevice();
	
private:
	CBTGPSGetNextDevice(MBTGPSConnectManagerExt& aManagerExt);
	void ConstructL();
	
	static TInt HandlerCompleteCallback(TAny* aAny);
	void HandlerCompleteNotify();
	
private:
	//Manager Ext
	MBTGPSConnectManagerExt& iManagerExt;
	
	//Error code
	TInt iError;
	
	//Complete Idle object
	CIdle* iIdle;
	};

#endif	//BTGPSGETNEXTDEVICE_H
