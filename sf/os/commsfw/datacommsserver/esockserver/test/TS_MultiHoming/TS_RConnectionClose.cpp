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
 

#include "TS_RConnectionClose.h"

/* 
 * Test 1.1
 * Open a default / named connection
 */
CTS_RConnectionClose::CTS_RConnectionClose()
	{
	iTestStepName = KRConnectionClose;
	}

CTS_RConnectionClose::~CTS_RConnectionClose()
	{	
	}

enum TVerdict CTS_RConnectionClose::doTestStepPreambleL()
/**
 * Implements OOM testing in each test
 */
	{
	if (!(GetBoolFromConfig(KConnectionInfo,KOomTest,iIsOOMTest)))
		{
		iIsOOMTest=EFalse;
		}
	return EPass;
	}


enum TVerdict CTS_RConnectionClose::doTestStepL()
/**
 * Reads the configuration file to find out which
 * connection(s) need to be closed.
 * Started connections are stored in the array.
 */
	{

	TPtrC connPtr(KNameDefault);
	TInt err = KErrNone;
	TInt numOfConns = 0;	//< fall back to zero

	err = GetIntFromConfig(KConnectionInfo, KNumOfConns, numOfConns);
	
	if (!err)		// We have no config available, so 1 default connection
		{	
		LogExtra((TText8*)__FILE__, __LINE__, ESevrInfo, KDoingDefault);
		}


		
	// Get Config Values
	TBuf<12> keyName;
		
	for (TInt i=0; i < numOfConns; i++)	
		{
		keyName = KConnName;		//< Descriptor len is ok up to 99 connections...
		keyName.AppendNum(i);


		err = GetStringFromConfig(KConnectionInfo, keyName, connPtr);
		if (!err)
			{
			LogExtra((TText8*)__FILE__, __LINE__, ESevrErr, KEConfigFile);	// We must have the naming section, for >1 connection 
			return EFail;
			}

		TConnDetails *myConn = iOwnerSuite->GetTConnection(connPtr);
		if(myConn == 0)
			{
			LogExtra((TText8*)__FILE__, __LINE__, ESevrErr, KEConnNotFound);;	// We must have the naming section, for >1 connection 
			return EFail;
			}
		
			err = iOwnerSuite->CloseConnection(connPtr);

		TESTE(err==KErrNone,err);
		}	// for (<numOfConns>)
	return iTestStepResult;
	}


