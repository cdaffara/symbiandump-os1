// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <dscitem.h>
#include "startsafe.h"

#include "amastart.h"

/**
 * Used to create an instance of CAmaStart class
 * @return An instance of CAmaStart
 */
EXPORT_C CAmaStart* CAmaStart::NewL()
	{
	RDebug::Print(_L(">CAmaStart::NewL"));
	
	CAmaStart* ama = new(ELeave) CAmaStart();
	CleanupStack::PushL(ama);
	ama->ConstructL();
	CleanupStack::Pop(ama);
	
	RDebug::Print(_L("CAmaStart::NewL>"));
	return ama;
	}



/**
 * Used to launch AMAs (after market application) from a DSC. 
 * Returns when processing of the DSC is complete. 
 * (Synchronous)
 * @param aDscId Id of the DSC containing AMAs to be started.
 */
EXPORT_C void CAmaStart::StartL(const TUid aDscId)
	{
	RDebug::Print(_L(">CAmaStart::StartL"));
	
	iDscStore.EnumOpenLC(aDscId) ; // leaves if opening fails

	CDscItem* item = iDscStore.EnumReadNextL();
	while (item)
		{
		//if loading of one AMA from the DSC fails we should move to next AMA - so we trap here 
		TRAP_IGNORE(StartDscItemL(*item));
		
		delete item;
		item = iDscStore.EnumReadNextL();
		}

	//EnumClose()
	//Pop and destroy cleanup rollback operation left on cleanstack by EnumOpenLC()
	CleanupStack::PopAndDestroy();
	
	RDebug::Print(_L("CAmaStart::StartL>"));
	}

/**
 * Destructor for CAmaStart class
 */
EXPORT_C CAmaStart::~CAmaStart()
	{
	RDebug::Print(_L( ">CAmaStart::~CAmaStart"));
	delete iStartSafe;
	if (&(this)->iDscStore)
		{
		iDscStore.Close();
		}
	if (&(this)->iSysMon)
		{
		iSysMon.Close();
		}
	RDebug::Print(_L( "CAmaStart::~CAmaStart>"));
	}



void CAmaStart::StartDscItemL(const CDscItem& aDscItem)
	{
	RDebug::Print(_L( ">CAmaStart::StartDscItemL"));
	//A process to be created inside iStartSafe->StartL();
	RProcess process;
	CleanupClosePushL(process);

	//Number of retries made for starting the process.
	TInt tried=0;
	
	// start the process
	const CStartupProperties& properties = aDscItem.StartupProperties();
	iStartSafe->StartL(properties, process, tried);
		
#ifdef _DEBUG
	_LIT(KNumRetried, "%d times has been retried to start the process successfully");
	RDebug::Print(KNumRetried, tried);
#endif

	TInt error=KErrNone;
	//monitor the process if indicated
	if(aDscItem.Monitored())
		{
		// first time monitoring, so connect with the SysMon server
		if (iSysMon.Handle() == KNullHandle)
			{
			TRAP(error, iSysMon.OpenL());
			}
		// monitor the process	
		if (KErrNone==error) 
			{
			TRAP(error, iSysMon.MonitorL(properties, process));
			}
		//Connect to SysMon fail or register to monitor the process fail
		//Kill the started process because Start and Monitor should be one atomic function.
		if (KErrNone !=error)
			{
			process.Kill(error);
			process.Close();
			}
		User::LeaveIfError(error);	
		}
	CleanupStack::PopAndDestroy(&process);
	RDebug::Print(_L( "CAmaStart::StartDscItemL>"));
	}


	
CAmaStart::CAmaStart()
	{
	RDebug::Print(_L(">CAmaStart::CAmaStart> (Empty constructor)"));
	}



void CAmaStart::ConstructL()
	{
	RDebug::Print(_L(">CAmaStart::ConstructL"));
	iStartSafe = CStartSafe::NewL();
	iDscStore.OpenL();
	RDebug::Print(_L("CAmaStart::ConstructL>"));
	}
