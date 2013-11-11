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
/**
  *
 * Header for TE_RootServerSuite class. This is the container
 * class for all the RConnection multihoming test steps
 *
 */
#if (!defined __TE_RootServerSUITE_H__)
#define __TE_RootServerSUITE_H__

#include <test/TestExecuteServerBase.h>
#include "c32root.h"

class CTestStepRootServer;

class CTE_RootServerServer : public CTestServer
	{
public:
	static CTE_RootServerServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
	
	TInt StartRootServer();
	TBool ShutdownRootServer();

	RRootServ& RootSess()
		{
		return iRootServer;
		}
	TInt NextServerNumber()
		{
		return iNextServerNumber++;
		}
	// this should be a pure virtual so every test ddl
	// has to provide a version but for now defaults to ?.?
	virtual TPtrC GetVersion( void );
	
private:
	RRootServ iRootServer;
	TInt iNextServerNumber;
	
	};

// CSelfPopScheduler - thin extension of CActiveScheduler to give useful installation & removal cleanup-stack behaviour
class CSelfPopScheduler : public CActiveScheduler
	{
public:
	static CSelfPopScheduler* CreateLC();
	operator TCleanupItem();
private:
	static void Cleanup(TAny* aItem);
	};
	
#endif
