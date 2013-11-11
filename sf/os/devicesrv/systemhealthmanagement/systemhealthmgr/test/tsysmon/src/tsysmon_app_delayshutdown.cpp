// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This process will rendezvous then keep running until terminated by an external intervention. 
// Count is kept of the number of times the process has been run (based on the name of the 
// executable the process is launched from.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code   
*/

#include <savenotf.h>
	
#include "sysmontesthelper.h"

class CShutdownDelayer : public CBase, public MSaveObserver
	{
public:
	static CShutdownDelayer* NewL();
	CShutdownDelayer();
	~CShutdownDelayer();
	void ConstructL();
	//from MSaveObserver
	void SaveL(TSaveType aSaveType);
private:
	CSaveNotifier *iNotifier;
	};

CShutdownDelayer* CShutdownDelayer::NewL()
	{
	CShutdownDelayer * self = new (ELeave) CShutdownDelayer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CShutdownDelayer::CShutdownDelayer()	
	{
	}	
	
void CShutdownDelayer::ConstructL()	
	{
	iNotifier = CSaveNotifier::NewL(*this);
	}	
	
CShutdownDelayer::~CShutdownDelayer()	
	{
	delete iNotifier;
	}	

void CShutdownDelayer::SaveL(TSaveType /*aSaveType*/)
	{ 
	iNotifier->DelayRequeue(); 
	User::After(5000000); // 5 seconds
	iNotifier->Queue();
	CActiveScheduler::Stop();
	}	

void MainL()
    {
    RProcess::Rendezvous(KErrNone);
	
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CShutdownDelayer* delayer = CShutdownDelayer::NewL();
	CleanupStack::PushL(delayer);
	
	CActiveScheduler::Start();
		
	CleanupStack::PopAndDestroy(2, scheduler);
    }

//  Global Functions

TInt E32Main()
	{
	// Create cleanup stack
	RDebug::Print(_L("slaveprocess: E32Main"));
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	// Run application code inside TRAP harness
	TRAPD(testError, MainL());
	if (testError)
		{
		User::Panic(_L("Test failure"), testError);
		}

	delete cleanup;
	__UHEAP_MARKEND;
	RDebug::Print(_L("slaveprocess: E32Main end"));
	return KErrNone;
	} //lint -e714 Suppress 'not referenced'
