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
// This contains TS_MultiHoming RConnection Stop primitive.
// RConnection starts opens and starts a connection, and 
// stores this connection in C
// 
//

/**
 @file
*/
 
#include "TS_RConnectionStop.h"

/* 
 * Test 1.1
 * Open a default / named connection
 */
CTS_RConnectionStop::CTS_RConnectionStop()
	{
	iTestStepName = KRConnectionStop;
	}

CTS_RConnectionStop::~CTS_RConnectionStop()
	{	
	}

enum TVerdict CTS_RConnectionStop::doTestStepPreambleL(void)
/**
 * Implements OOM testing in each test
 */
	{
	if (!(GetBoolFromConfig(KConnectionInfo,KOomTest,iIsOOMTest)))
		iIsOOMTest=EFalse;
	return EPass;
	}


enum TVerdict CTS_RConnectionStop::doTestStepL()
/**
 * Reads the configuration file to find out which
 * connection(s) need to be stopped.
 * Any clients using this connection will be disconnected
 * as a result.
 */
	{

	TPtrC connPtr(KNameDefault);
	TInt err = KErrNone;
	TInt numOfConns = 0;	//< fall back to zero
	TBool defaultOnly = EFalse;

	err = GetIntFromConfig(KConnectionInfo, KNumOfConns, numOfConns);
	
	// Is the default a vailid case
	if (!err)		// We have no config available, so 1 default connection
		{	
		LogExtra((TText8*)__FILE__, __LINE__, ESevrInfo, KDoingDefault);
		defaultOnly = ETrue;
		}
	
		
	// Get Config Values
	TBuf<10> keyName;
		
	for (TInt i=0; i < numOfConns; i++)	
		{
		keyName = KConnName;		//< Descriptor len is ok up to 99 connections...
		if (!defaultOnly)
			{
			keyName.AppendNum(i);
			err = GetStringFromConfig(KConnectionInfo, keyName, connPtr);
			if (!err)
				{
				LogExtra((TText8*)__FILE__, __LINE__, ESevrErr, KEConfigFile);;	// We must have the naming section, for >1 connection 
				return EFail;
				}
			}	
		err = iOwnerSuite->StopConnection(connPtr);
		TESTE(err==KErrNone,err);
		}	// for (<numOfConns>)
		User::After(5000*1000);
	return iTestStepResult;
	}


