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
 @file The TEFUnit header file which tests the CloseChannel
*/

#ifndef CCSATCLOSECHANNELFU_H
#define CCSATCLOSECHANNELFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"



class CCSatCloseChannelFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyCloseChannelPCmd0001L();
	void TestNotifyCloseChannelPCmd0002L();
	void TestNotifyCloseChannelPCmd0004L();
	
private:
   /**
    * prepare tlv buffer for OpenChannel proactive command 
    * @param aTlv - tlv buffer to prepare
    * @param aCmdNum - command number
    * @param aCmdQualifier - type of command
    * @param aAlpha - alpha identifier 
    */   	
	static void PrepareTlv(TTlv& aTlv, TUint8 aCmdNum, TUint8 aCmdQualifier, TUint8 aChan, 
			const TDesC8& aAlpha = KNullDesC8); 
	
   /**
    * prepare mock for processing of proactive command
    * @param aTlvDsc - tlv buffer to send
    * @param aRes - result for request completion
    */   	
	void CompleteDispatchL(TPtrC8 aTlvDsc, TInt aRes = KErrNone);
	
   /**
    * prepare complete for dispatch of sat notification  
    */   	
	void PrepareDispatchL();

	/**
	 * Tests all possible "TERMINAL RESPONSE" cases. 
	 * API should return KErrNone for all valid responses and KErrCorrupt 
	 * for those that can not be used with "Close channel" command  
	 */
	void DoTestTerminalResponseL();
	
	}; // class CCSatCloseChannelFU

#endif // CCSATCLOSECHANNELFU_H

