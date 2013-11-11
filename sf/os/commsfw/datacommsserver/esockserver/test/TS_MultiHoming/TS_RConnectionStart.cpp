// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains TS_MultiHoming RConnection Start primitive.
// RConnection starts opens and starts a connection, and 
// stores this connection in C
// 
//

/**
 @file
*/


#include "TS_RConnectionStart.h"
#include <nifman.h>
/* 
 * Test 1.1
 * Open a default / named connection
 */
CTS_RConnectionStart::CTS_RConnectionStart()
	{
	iTestStepName = KRConnectionStart;
	}

CTS_RConnectionStart::~CTS_RConnectionStart()
	{	
	}

enum TVerdict CTS_RConnectionStart::doTestStepPreambleL(void)
/**
 * Implements OOM testing in each test
 */
	{
	if (!(GetBoolFromConfig(KConnectionInfo,KOomTest,iIsOOMTest)))
		iIsOOMTest=EFalse;
	return EPass;
	}


enum TVerdict CTS_RConnectionStart::doTestStepL(void)
/**
 * Creates and starts one or more Explicit Connection(s)
 * and appends it/them to the array of
 * connections in the MultiHoming Suite 
 * @return The test step outcome.
 * 
 */
	{
	TPtrC connPtr(KNameDefault);
	TInt err = KErrNone;
	TInt numOfConns = 0;
	TBool expectSuccess = ETrue;

	err = GetIntFromConfig(KConnectionInfo, KNumOfConns, numOfConns);
	
	
	if (err != 1)		// We have no config available, so end test
		{	
			return EFail;
		}
	
	
	// Get Config Values
	
	TBuf<10> keyName;

	// Create Connection object(s)
	for (TInt i=0; i < numOfConns; i++)	
		{
		TConnDetails *myConn = iOwnerSuite->GetNewTConnection();
		keyName = KConnName;		//< Descriptor len is ok up to 99 connections...
		
		keyName.AppendNum(i);
		err = GetStringFromConfig(KConnectionInfo, keyName, connPtr);
		if(err != 1)
			{
			LogExtra((TText8*)__FILE__, __LINE__, ESevrErr, KEConfigFile);	// We must have the naming section, for >1 connection 
			return EFail;
		}

		myConn->iName.Copy(connPtr.Ptr(),connPtr.Length());

		err = GetConnectionType(myConn, myConn->iName);
		if(err != 0)
			{
			LogExtra((TText8*)__FILE__, __LINE__, ESevrErr, KEConfigFile);	// We must have the naming section, for >1 connection 
			return EFail;
			}
		

		if(myConn->iConnectionType != TConnDetails::implicitConn)
			{
			err = myConn->iSocketServ.Connect();
			TESTE(err==KErrNone,err);	
		
			err = myConn->iConnection.Open(myConn->iSocketServ, KAfInet);
			TESTE(err==KErrNone,err);	
			}
		
		// Find out if we're to use the notification API
		// Ignore errors - dont care if this is missing
		// No check necessary, as will stay false if fails.
		err = SetConnPrefs(myConn->iConnPrefs, connPtr);

		expectSuccess = -1;
		if(err != 0)
			{
				TPtrC errorText = EpocErrorToText(err);
				LogExtra((TText8*)__FILE__, __LINE__, ESevrInfo, KELogger, &errorText);	
				return EFail;
			}

		GetBoolFromConfig(connPtr, KExpectSuccess, expectSuccess);

		
		if(myConn->iConnectionType == TConnDetails::explicitConn)
			{
				err = myConn->iConnection.Start(myConn->iConnPrefs);
			}
		else if(myConn->iConnectionType == TConnDetails::defaultConn)
			{
				err = myConn->iConnection.Start();
			}
		else
			{
			TUint protocol = KProtocolInetUdp;
	
			err = myConn->iSocketServ.Connect();
			TESTE(err==KErrNone,err);	

			// Get the protocol field and open the socke
			err = myConn->iSocket.Open(myConn->iSocketServ, KAfInet, KSockDatagram, protocol);
			// Bind the socket, as if this is a server to open the connection
			TInetAddr localAddr;
			localAddr.SetAddress(KInetAddrAny);
			localAddr.SetPort(6666 + i);            // source port = 666 + conn num

			myConn->iSocket.SetOpt(KSoReuseAddr, KSolInetIp, 1);

			err = myConn->iSocket.Bind(localAddr);

			TInetAddr destAddr;
           
			destAddr.SetAddress(INET_ADDR(192, 168, 1, 255));
			destAddr.SetPort(500);            // destination port....

			TRequestStatus status;

			TBuf8<32> buffer;

			// build some data to send on the socket
			buffer.SetMax();
			buffer.FillZ();
			myConn->iSocket.SendTo(buffer, destAddr, 0, status);
			User::WaitForRequest(status);
			err = status.Int();
			}

		TESTE(err == KErrNone &&  expectSuccess
			  || err == KErrNone && !expectSuccess, err);
			
				
		if (err==KErrNone)	// If we got this far without errors, we have an open connection
			{
			// Insert into array
			if(myConn->iConnectionType != TConnDetails::implicitConn)
				{
//				myConn->iConnection.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
				}
			else
				{
				myConn->iSocket.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
				}
			LogExtra((TText8*)__FILE__, __LINE__, ESevrInfo, KConnXInsert, &(myConn->iName));
			}
		else
			{
			
			TPtrC errorText = EpocErrorToText(err);
			LogExtra((TText8*)__FILE__, __LINE__, ESevrInfo, KConnXFail, &(myConn->iName), &errorText); 
			myConn->iName.Zero();
			//	CLog::EpocErrorToText(err));
			}

		}		// for (<numOfConns>)
	return iTestStepResult;
	}


