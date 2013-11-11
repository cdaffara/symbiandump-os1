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
//
 
#include "TS_ResolveAddress.h"

/*       
 * Resolve an address to a hostname
 */
CTS_ResolveAddress::CTS_ResolveAddress()
	{
	iTestStepName = KResolveAddress;
	}

CTS_ResolveAddress::~CTS_ResolveAddress()
	{	
	}


enum TVerdict CTS_ResolveAddress::doTestStepPreambleL(void)
/**
 * Implements OOM testing in each test
 */
	{
	if (!(GetBoolFromConfig(KResolveAddress, KOomTest, iIsOOMTest)))
		iIsOOMTest=EFalse;
	return EPass;
	}

enum TVerdict CTS_ResolveAddress::doTestStepL(void)
/**
 * Resolve a hostname to address (reverse dns lookup)
 * @return The test step verdict  
 */
	{				
	TPtrC connPtr(KNameDefault);
	TBuf<10> keyName;
	keyName = KDestAddr; 
	
	// Get destination address from config file	 		
	TBool returnValue = GetStringFromConfig(KResolveAddress, keyName, connPtr);
	if (!returnValue)
		{
		LogExtra((TText8*)__FILE__, __LINE__, ESevrErr, KEConfigFile);;	 
		return EFail;
		}			
	
	// Create address
	TInetAddr ipAddr;
	TInt err = ipAddr.Input(connPtr);	
		
	// Resolve address
	TNameEntry result;
	RSocketServ sockServ;
	User::LeaveIfError(sockServ.Connect());
	RHostResolver resolver;
		
	// Does not have functionality to use an exisiting explicit connection 
	// (like ResolveName) but this test step is not used in any of the test cases.
	err = resolver.Open(sockServ, KAfInet, KProtocolInetTcp);
	TESTE(err==KErrNone,err);

	err = resolver.GetByAddress(ipAddr, result);
	TESTE(err==KErrNone,err);
	
	// Clean up
	resolver.Close();
	sockServ.Close();
		
	// Return result depending on what is expected	
	TBool expectTimeout = EFalse;
	TBool expectSuccess = ETrue;
	GetBoolFromConfig(KResolveName, KExpectTimeout, expectTimeout);
	GetBoolFromConfig(KResolveName, KExpectSuccess, expectSuccess);
	
	if (err != KErrNone)
		{
		if ((err==KErrNotFound) && !expectSuccess)
			return EPass;
		if ((err==KErrTimedOut) && expectTimeout)
			return EPass;
		else
			return EFail;
		}
	// No error so...
	if (expectSuccess)
		return EPass;
	else
		return EFail;	
	}


