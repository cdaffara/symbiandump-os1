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
//

#ifndef TESTSERVER_H
#define TESTSERVER_H

#include <test/testexecuteserverbase.h>
#include <es_sock.h>
#include <bttypes.h>
#include <bttypes.h>
#include <bt_sock.h>

enum TBluetoothEvent
	{
	EDisconnectAllComplete,
	};

//---------------------------------------------------------------------------------------------------
class CBluetoothUtilsTestServer : public CTestServer, public MBluetoothPhysicalLinksNotifier
	{
public:
	static CBluetoothUtilsTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CBluetoothUtilsTestServer();

public:
	RSocketServ					iSocketServer;
	CBluetoothPhysicalLinks*	iLinks;
	CActiveScheduler*		  	iScheduler;	
	TInt						iLastError;
	TBluetoothEvent				iLastEvent;
private:
	CBluetoothUtilsTestServer();
	void ConstructL(const TDesC& aName);

	void Complete(TBluetoothEvent aEvent, TInt aErr);

	// from MBluetoothPhysicalLinksNotifier
	void HandleCreateConnectionCompleteL(TInt aErr);
	void HandleDisconnectCompleteL(TInt aErr);
	void HandleDisconnectAllCompleteL(TInt aErr);
private: 

	};
	





		
//---------------------------------------------------------------------------------------------------
#endif // TESTSERVER_H
