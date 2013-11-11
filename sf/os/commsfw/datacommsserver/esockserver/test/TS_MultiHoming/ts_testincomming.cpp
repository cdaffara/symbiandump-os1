/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include "ts_testincomming.h"



CTS_TestIncomming::CTS_TestIncomming()
	{
	iTestStepName = KTestIncomming;
	}

CTS_TestIncomming::~CTS_TestIncomming()
	{
	}

enum TVerdict CTS_TestIncomming::doTestStepPreambleL(void)
/**
 * Implements OOM testing in each test
 */
	{
	if (!(GetBoolFromConfig(KTestIncomming,KOomTest,iIsOOMTest)))
		iIsOOMTest=EFalse;
	return EPass;
	}


enum TVerdict CTS_TestIncomming::doTestStepL(void)
/**
 * Creates and starts one or more Explicit Connection(s)
 * and appends it/them to the array of
 * connections in the MultiHoming Suite
 * @return The test step outcome.
 *
 */
{
	TInt err = KErrNone;
	TInt numOfSockets = 0;


	err = GetIntFromConfig(KTestIncomming, KNumOfSocks, numOfSockets);


	if ((!err) || (numOfSockets < 1))
		{
		return EInconclusive;
		}

	RArray<CSplitEchoSocket*> sockArray;
	RArray<TBool> expectSocketsToSucceedArray;
	TConnDetails *theConn;

	TInt i;
	for (i = 0; i < numOfSockets; i++)
		{
		// Get some configuration data...
		TInetAddr dest;
		TInetAddr src;

		TInt protocol = KProtocolInetUdp;
		TInt packSize = PACKET_SIZE;
		TInt packCount = NUM_OF_PACKETS;
		TInt aTol = UDP_TOLERANCE;
		TBool expectSuccess = ETrue;
		TBool isListener = EFalse;


		err = GetSockConfig(i,protocol,packSize,packCount, aTol, dest, src,
						expectSuccess, isListener, &theConn);


		expectSocketsToSucceedArray.Append(expectSuccess);

		if (err!=KErrNone)
			{
			LogExtra((TText8*)__FILE__, __LINE__, ESevrErr, KEConfigFile);
			}

		CSplitEchoSocket* newSocket = CSplitEchoSocket::NewL();

		err = newSocket->Echo(dest, src, packSize,packCount,protocol,aTol, theConn, i, isListener, this);
		if (err == KErrNone)
			{
			err = sockArray.Append(newSocket);
			if (err != KErrNone)
				{
				newSocket->DoCancel();
				delete newSocket;
				newSocket = NULL;
				}
			}
		else
			{
			TPtrC Errortxt = EpocErrorToText(err);
			Log(KELogger, &Errortxt);
			delete newSocket;
			}
		}	// for (i<numOfSockets)


	if(sockArray.Count() > 0)
		CEnhancedScheduler::Start();	// And start them off...
	// Wait for all to finish / time out.

	for (i = 0; i < sockArray.Count(); i++)
		{
		TInt err = KErrNone;
		TBool boolResult = EFalse;
		TInt percent = 0;
		TInt numSent = -1;
		err = sockArray[i]->LogReport(boolResult, numSent, percent);
		if (expectSocketsToSucceedArray[i] &&
			(KErrNone == err) && boolResult
			||
			// Failure means there was a socket error or no packets received
			!expectSocketsToSucceedArray[i] && (KErrNone != err || !boolResult))
			{
			Log(KSockPassed, i, numSent, percent);
			}
		else
			{
			TPtrC errorText = EpocErrorToText(err);
			Log(KSockFailed, i, &errorText, numSent, percent);
			iTestStepResult = EFail;
			}
		delete sockArray[i];
		}


	expectSocketsToSucceedArray.Close();
	sockArray.Close();
	return iTestStepResult;
	}

