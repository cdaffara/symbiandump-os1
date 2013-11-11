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
// btbbsockethelpers.h
// BT socket interface types
// 
//

#ifndef BTBBSOCKETHELPERS_H
#define BTBBSOCKETHELPERS_H
//=============================================================================
//Active Objects
//

//CBluetoothPhysicalLinks Active Objects...
NONSHARABLE_CLASS(CBTBasebandConnecter) : public CActive
	{
public:
	static CBTBasebandConnecter* NewL(CBluetoothPhysicalLinks& aParent);
	void BasebandConnect(const TBTDevAddr& aBDAddr);

	~CBTBasebandConnecter();


private:
	CBTBasebandConnecter(CBluetoothPhysicalLinks& aParent);
	void ConstructL();
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	CBluetoothPhysicalLinks& iParent;
	};


NONSHARABLE_CLASS(CBTDisconnector) : public CActive
	{
public:
	static CBTDisconnector* NewL(CBluetoothPhysicalLinks& aParent);
	void Disconnect(const TBTDevAddr& aBDAddr);
	void DisconnectAll();
	void DisconnectAllForPowerOff();
	~CBTDisconnector();


private:
	enum TCurrentRequest
		{
		ENone,
		EDisconnect,
		EDisconnectAll,
		};
	CBTDisconnector(CBluetoothPhysicalLinks& aParent);
	void ConstructL();
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	CBluetoothPhysicalLinks& iParent;
	TCurrentRequest iCurrentRequest;
	};

#endif
