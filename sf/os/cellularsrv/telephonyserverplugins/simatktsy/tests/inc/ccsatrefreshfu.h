// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// functional unit of the SAT.
//



/**
 @file The TEFUnit header file which tests the Refresh
*/

#ifndef CCSATREFRESHFU_H
#define CCSATREFRESHFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatRefreshFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyRefreshPCmd0001L();
	void TestNotifyRefreshPCmd0002L();
	void TestNotifyRefreshPCmd0004L();
	void TestNotifyRefreshPCmd0001bL();

private:

	/**
    * prepare tlv buffer for refresh proactive command 
    * @param aTlv - tlv buffer to prepare
    * @param aCmdNum - command number
    * @param aCmdQualifier - type of refresh command
    */   	
	static void PrepareTlv(TTlv& aTlv, TUint8 aCmdNum, TUint8 aCmdQualifier);

	/**
    * prepare mock for dispatch of sat notification  
    */   	
	void PrepareRefreshDispatchL();

	/**
    * prepare mock complete for processing of proactive command
    * @param aTlvDsc - tlv buffer to send
    * @param aRes - result for request completion
    */   	
	void CompleteRefreshDispatchL(const TDesC8& aTlvDsc, TInt aRes = KErrNone);
	
	/**
    * call RefreshAllowed() for completion of NotifyRefreshPCmd()
    * @param aRes - result for completion
    * @param aCache - bitmask indicating files in cache to refresh
    * @param aFileList - buffer with filelist
    * @param aAid - application id   
    */   	
	void PrepareAndCallRefreshAllowedL(RSat::TPCmdResult aRes = RSat::KSuccess, TUint16 aCache = 0x00, 
			const TDesC8& aFileList = KNullDesC8, const TDesC8& aAid = KNullDesC8);
	
	}; // class CCSatRefreshFU

#endif // CCSATREFRESHFU_H

