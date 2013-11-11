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
// BT Synchronous link active object helpers
// 
//

#ifndef BTSYNCLINKHELPERS_H
#define BTSYNCLINKHELPERS_H

#include <bluetooth/btscoparams.h>

NONSHARABLE_CLASS(CBTSynchronousLinkBaseband) : public CActive
	{
public:
	static CBTSynchronousLinkBaseband* NewL(CBluetoothSynchronousLink& aParent);
	~CBTSynchronousLinkBaseband();
	
	void CatchEvents();
	void PreventPark();
	void StopAll();
	
private:
	CBTSynchronousLinkBaseband(CBluetoothSynchronousLink& aParent);
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
	CBluetoothSynchronousLink&	iParent;
	TBTBasebandEvent			iEvent;
	};


NONSHARABLE_CLASS(CBTSynchronousLinkAttacher) : public CActive
	{
public:
	static CBTSynchronousLinkAttacher* NewL(CBluetoothSynchronousLink& aParent, TSCOType aSCOType);
	~CBTSynchronousLinkAttacher();

	void AttachSCOLink(TBTSockAddr& aSockAddr);


private:
	CBTSynchronousLinkAttacher(CBluetoothSynchronousLink& aParent, TSCOType aSCOTypes);
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	CBluetoothSynchronousLink& iParent;
	
	TSCOType iSCOType;
	};


NONSHARABLE_CLASS(CBTSynchronousLinkDetacher) : public CActive
	{
public:
	static CBTSynchronousLinkDetacher* NewL(CBluetoothSynchronousLink& aParent, TSCOType aSCOTypes);
	~CBTSynchronousLinkDetacher();

	void DetachSCOLink(const TDesC8& aDesOut, TDes8& aDesIn);


private:
	CBTSynchronousLinkDetacher(CBluetoothSynchronousLink& aParent, TSCOType aSCOTypes);
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	CBluetoothSynchronousLink& iParent;
	TBuf8<1> iDummySCOShutdownDescriptor;
	
	TSCOType iSCOType;
	};


NONSHARABLE_CLASS(CBTSynchronousLinkAccepter) : public CActive
	{
public:
	static CBTSynchronousLinkAccepter* NewL(CBluetoothSynchronousLink& aParent, TSCOType aSCOType);
	~CBTSynchronousLinkAccepter();

	void Accept();


private:
	CBTSynchronousLinkAccepter(CBluetoothSynchronousLink& aParent, TSCOType aSCOType);
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	CBluetoothSynchronousLink& iParent;
	
	TSCOType iSCOType;
	};


NONSHARABLE_CLASS(CBTSynchronousLinkSender) : public CActive
	{
public:
	static CBTSynchronousLinkSender* NewL(CBluetoothSynchronousLink& aParent, TSCOType aSCOType);
	~CBTSynchronousLinkSender();

	void SendSCOData(const TDesC8& aSCOData);


private:
	CBTSynchronousLinkSender(CBluetoothSynchronousLink& aParent, TSCOType aSCOType);
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	CBluetoothSynchronousLink& iParent;
	
	TSCOType iSCOType;
	};


NONSHARABLE_CLASS(CBTSynchronousLinkReceiver) : public CActive
	{
public:
	static CBTSynchronousLinkReceiver* NewL(CBluetoothSynchronousLink& aParent, TSCOType aSCOType);
	~CBTSynchronousLinkReceiver();

	void ReadSCOData(TDes8& aDesc);


private:
	CBTSynchronousLinkReceiver(CBluetoothSynchronousLink& aParent, TSCOType aSCOTypes);
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	CBluetoothSynchronousLink& iParent;
	
	TSCOType iSCOType;
	};

#endif
