// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "esocktestutils.h"
#include <es_sock.h>
#include <es_enum.h>
#include <f32file.h>






EXPORT_C TInt ESockTestUtils::StopAllInterfaces(TDes& aErrDesc)
/**
Close all interfaces that are currently open by attaching to each in turn and then using the Stop() method.

@param None
*/
	{
	// this code is based on the implementation of the same method in TS_RConnection
	// there are no methods used here which are allowed to leave and so the cleanup stack is not used
	TInt err = KErrNone;
	
	// all operations here require a socket server
	RSocketServ ss;	
	err = ss.Connect();

	TDes16IgnoreOverflow overflow;
	aErrDesc.Zero();
	
	// how many open interfaces are there?
	RConnection enumerator;
	if(err == KErrNone)
		{
		err = enumerator.Open(ss);
		}
	TUint noOfConnectionsBefore = 0;
	TUint noOfConnectionsAfter = 0;	
	if(err == KErrNone)
		{
		err = enumerator.EnumerateConnections(noOfConnectionsBefore);
		}	
	if (KErrNone != err)
		{
		aErrDesc.AppendFormat(_L("Up to initial EnumerateConnections failed"), &overflow);
		return err;
		}
	aErrDesc.AppendFormat(_L("Started with %d connections\n"), &overflow, noOfConnectionsBefore);
	// for each interface attach to it and then stop it.
	TPckgBuf<TConnectionInfo> connectionInfo;
	while (noOfConnectionsBefore && err == KErrNone)
		{
		err = enumerator.GetConnectionInfo(1, connectionInfo);
		if (KErrNone != err)
			{
			aErrDesc.AppendFormat(_L("GetConnectionInfo(1) failed"), &overflow);
			break;
			}
		aErrDesc.AppendFormat(_L("Conn#1 IAP=%d, NID=%d\n"), &overflow, connectionInfo().iIapId, connectionInfo().iNetId);
		
		RConnection real;
		err = real.Open(ss);
		if (KErrNone != err)
			{
			aErrDesc.AppendFormat(_L("RConnection::Open() failed"), &overflow);
			break;
			}
		
		//Attach can fail, and it may be acceptible for it to fail
		//if the connection is no longer there.
		err = real.Attach(connectionInfo, RConnection::EAttachTypeNormal);
		if (KErrNone == err)
			{
    		err = real.Stop();
			if(KErrNone != err)
				{
				aErrDesc.Format(_L("RConnection::Stop failed"), &overflow);
				}
			}
		aErrDesc.Format(_L("RConnection::Attach failed with %d - continuing"), &overflow, err);
  		real.Close();
  		
  		//Here we check the ultimate objective of the iteration, i.e.:
  		//at least one connection less. 
		TInt errEnum = enumerator.EnumerateConnections(noOfConnectionsAfter);
		if(errEnum != KErrNone)
			{
			aErrDesc.Format(_L("RConnection::Enumerate failed with %d - continuing"), &overflow, errEnum);
			}
		if (noOfConnectionsAfter < noOfConnectionsBefore)
    		{
    		//Iteration successful. If error attaching, clear it.
    		err = KErrNone;
    		noOfConnectionsBefore = noOfConnectionsAfter;
    		}
    	else if (err == KErrNone)
        	{
            //very strange - no error recorded, though no connection stopped.
            err = KErrGeneral;
        	}
		}
	
	// clean up
	enumerator.Close();
	ss.Close();
	return err;
	}


EXPORT_C void ESockTestUtils::DefaultToConfig1L()
	{
	// If no PDUMMY1 or PDUMMY2 config files are present then run "te_esock_test_copy_config_1.script". This allows elementary tests such as TE_SOCKET
	// to "work out of the box" on a freshly booted ROM, so reducing support costs.
	// This trivial thing is only implemented as a test step because our test scripting language has no conditional support.
	_LIT(KConfiguratorPrivateDir, "\\private\\101F7988\\");
	_LIT(KPDummyCmiMask, "pdummy*.cmi");
	_LIT(KCodealerCmiMask, "esock_codealer*.cmi");

	RFs fs;
	User::LeaveIfError(fs.Connect());

	TFindFile ff(fs);
	CDir* dummyList = NULL;
	TInt ret = ff.FindWildByDir(KPDummyCmiMask, KConfiguratorPrivateDir, dummyList);
	if(ret != KErrNone)
		{
		delete dummyList;
		ret = ff.FindWildByDir(KCodealerCmiMask, KConfiguratorPrivateDir, dummyList);
		if(ret != KErrNone)
			{
			// Run the config script. Very horribly it seems to have to be like this with ScheduleTest, as running
			// TestExecute from a TestExecute test step produces KErrAlreadyExists followed by a garbled log file for the
			// parent. 
			_LIT(KScheduleTestExe, "ScheduleTest");
			_LIT(KCopyConfig1Script, "z:\\testdata\\scripts\\te_esock_test_copy_config_1.script");
			RProcess testExec;
			ret = testExec.Create(KScheduleTestExe, KCopyConfig1Script);
			if(ret == KErrNone)
				{
				TRequestStatus completedStatus;
				testExec.Logon(completedStatus);
				testExec.Resume();
				User::WaitForRequest(completedStatus);
				}
			testExec.Close();
			}
		}
	delete dummyList;
	}





//from deprecated genconn library
//modified to fit in with TE_ methodology
EXPORT_C TBool ESockTestUtils::IsNetworkActiveL()
{

	TBool result = true;
	TInt err = KErrNone;

	RSocketServ ss;
	err = ss.Connect();
	User::LeaveIfError(err);

	RConnection conn;
	err = conn.Open(ss);
	User::LeaveIfError(err);

	TUint count(0);
	err = conn.EnumerateConnections(count);
	User::LeaveIfError(err);


	conn.Close();
	ss.Close();

	//KErrNone - no connections, need to think about return status if there
	//are connections taking place
	if (count==0)
		result= false;
	else
		result= true;

	return result;
}


//from deprecated genconn library
//modified to fit in with TE_ methodology as per esocktestsection9.cp-waitfor
EXPORT_C TInt ESockTestUtils::Delay(TInt aGuardTimerValue)
{
	//Note this function step would have been called from the iTest scripts
	//to delay for a certain amount of milliseconds before checking to see
	//if the socket/network/connection has changed to the desired state.
	//The delay tries to ensure the socket/network/connection
	//status has change before we go to the next step where we would check 
	//this status

	// wait for either test thread or timer to end
	TInt err = KErrNone;
	RTimer GuardTimer;
	GuardTimer.CreateLocal();			// create for this thread
	TRequestStatus TimerStatus;

	//aGuardTimerValue is in milliseconds
	GuardTimer.After(TimerStatus,aGuardTimerValue * 1000); 


	User::WaitForRequest(TimerStatus);
	if( TimerStatus == KRequestPending )
		{
		// This is lifted from SockBench to solve some apparently
		// strange behaviour with the timer.
		
		// Since we are still waiting for this timer, cancel it and
		// wait for the request to be flagged.  I believe this is
		// needed since the kernel may have setup a semaphore on the
		// 'TimerStatus' Request Status.  The Cancel() will flag the
		// status change (or maybe the timer will expire anyway) and
		// if we don't clear it, we may crash later when we return
		// to the Active Scheduler (since the TRequestStatus and
		// RTimer won't exist).
		//
		GuardTimer.Cancel();
		User::WaitForRequest( TimerStatus );
		}
	GuardTimer.Cancel();
	if (TimerStatus == KErrNone)
		err= KErrNone;
	else
		err= KErrGeneral;

	return err;
}

