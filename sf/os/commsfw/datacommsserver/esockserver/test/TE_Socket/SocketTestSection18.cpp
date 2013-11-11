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
// SocketTestSection14.cpp
// This contains ESock Test cases from section 14
// 
//

// EPOC includes
#include <e32base.h>
#include <test/es_dummy.h>

// Test system includes
#include "SocketTestSection18.h"


// Test step 18.1
const TDesC& CSocketTest18_1::GetTestName()
	{
	_LIT(ret,"Test18.1");
	return ret;
	}

enum TVerdict CSocketTest18_1::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Testing cancellation of Accept"));
	RSocketServ ss;
	TInt ret=OptimalConnect(ss);
	CleanupClosePushL(ss);
    TESTL(KErrNone == ret);	
    
	Logger().WriteFormat(_L("Open by name"));             
	RSocket sock, sock2;                                                    
	ret=sock.Open(ss,_L("Dummy Protocol 2")); 
	TESTL(KErrNone == ret);	
	
	ret=sock2.Open(ss); // null socket to accept onto                         
	TESTL(KErrNone == ret);	
	
	TSockAddr addr;
	ret=sock.Bind(addr);
	TESTL(KErrNone == ret);	
	
	Logger().WriteFormat(_L("Listen"));             
	ret=sock.Listen(5);
	TESTL(KErrNone == ret);	
	
	Logger().WriteFormat(_L("Accepting"));             
	TRequestStatus rstat;
	sock.Accept(sock2, rstat);
	TESTL(rstat.Int() == KRequestPending);	

	Logger().WriteFormat(_L("Cancelling Accept"));	
	sock.CancelAccept();
	User::WaitForRequest(rstat);
	TESTL(rstat == KErrCancel);	
	
    Logger().WriteFormat(_L("Attempting to complete the accept"));             
	//emulate en connect complete event
	ret=sock.SetOpt(KDummyOptionSetConnectComplete,0,0);
	User::After(100000); //Just a wait to allow something to possibly go wrong
	TESTL(rstat.Int() == KErrCancel);	
	
	CleanupStack::Pop(&ss);
    ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}
	
// Test step 18.2
const TDesC& CSocketTest18_2::GetTestName()
	{
	_LIT(ret,"Test18.2");
	return ret;
	}

enum TVerdict CSocketTest18_2::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Testing cancellation of Connect"));
	RSocketServ ss;
	TInt ret=OptimalConnect(ss);
	CleanupClosePushL(ss);
    TESTL(KErrNone == ret);	
    
	Logger().WriteFormat(_L("Open by name"));             
	RSocket sock;                                                    
	ret=sock.Open(ss,_L("Dummy Protocol 2"));                      
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Setiing option to block connect"));             
	ret = sock.SetOpt(KDummyOptionSetBlockConnect,25,TRUE);
	TESTL(KErrNone == ret);	
	
	TSockAddr addr;
	TRequestStatus stat1;
	Logger().WriteFormat(_L("Performing Connect"));             
	sock.Connect(addr,stat1);
	User::After(100000);
	TESTL(stat1 == KRequestPending);
	
	Logger().WriteFormat(_L("Setiing option to unblock complete connect"));             
	ret = sock.SetOpt(KDummyOptionSetBlockConnect,25,FALSE);
	TESTL(KErrNone == ret);	
	User::WaitForRequest(stat1);
	TESTL(stat1 == KErrNone);
	
	Logger().WriteFormat(_L("Closing Socket"));             
	sock.Close();
	
	Logger().WriteFormat(_L("Open by name"));             
	ret=sock.Open(ss,_L("Dummy Protocol 2"));                      
	TESTL(KErrNone == ret);
	
	Logger().WriteFormat(_L("Setiing option to block connect"));             
	ret = sock.SetOpt(KDummyOptionSetBlockConnect,25,TRUE);
	TESTL(KErrNone == ret);	
	
	TRequestStatus stat;
	Logger().WriteFormat(_L("Performing Connect"));             
	sock.Connect(addr,stat);
	User::After(100000);
	TESTL(stat == KRequestPending);
	
	Logger().WriteFormat(_L("Cancelling Connect"));             
	sock.CancelConnect();
	User::WaitForRequest(stat);
	TESTL(stat.Int() == KErrCancel);	

	Logger().WriteFormat(_L("Setiing option to unblock complete connect"));             
	ret = sock.SetOpt(KDummyOptionSetBlockConnect,25,FALSE);
	TESTL(KErrNone == ret);	
	User::After(100000);
	TESTL(stat == KErrCancel);
    	
	CleanupStack::Pop(&ss);
    ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}
	
	// Test step 18.3
const TDesC& CSocketTest18_3::GetTestName()
	{
	_LIT(ret,"Test18.3");
	return ret;
	}

enum TVerdict CSocketTest18_3::InternalDoTestStepL( void )
	{
	TVerdict verdict = EPass;
	
	Logger().WriteFormat(_L("Testing cancellation of Ioctl"));
	RSocketServ ss;
	TInt ret=OptimalConnect(ss);
	CleanupClosePushL(ss);
    TESTL(KErrNone == ret);	
    
	Logger().WriteFormat(_L("Open by name"));             
	RSocket sock;                                                    
	ret=sock.Open(ss,_L("Dummy Protocol 2"));                      
	TESTL(KErrNone == ret);
	
	TRequestStatus stat;
	Logger().WriteFormat(_L("Sending Ioclt that does not complete"));             
	sock.Ioctl(KDummyIocltNonCompleting,stat);
	TESTL(KErrNone == ret);
	User::After(100000);
	TESTL(stat == KRequestPending);
	
	Logger().WriteFormat(_L("Setiing option to complete Ioclt"));             
	ret = sock.SetOpt(KDummyOptionSetIocltComplete,25,0);
	TESTL(KErrNone == ret);	
	User::WaitForRequest(stat);
	TESTL(stat == KErrNone);
		
	Logger().WriteFormat(_L("Sending Ioclt that does not complete"));             
	sock.Ioctl(KDummyIocltNonCompleting,stat);
	TESTL(KErrNone == ret);
	User::After(100000);
	TESTL(stat == KRequestPending);
	
	Logger().WriteFormat(_L("Cancelling Ioclt"));             
	sock.CancelIoctl();
	User::WaitForRequest(stat);
	TESTL(stat.Int() == KErrCancel);	

	Logger().WriteFormat(_L("Setiing option to complete Ioclt"));             
	ret = sock.SetOpt(KDummyOptionSetIocltComplete,25,0);
	TESTL(KErrNone == ret);	
	User::After(100000);
	TESTL(stat == KErrCancel);
    	
	CleanupStack::Pop(&ss);
    ss.Close();
	
	SetTestStepResult(verdict);
	return verdict;
	}



