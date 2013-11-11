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
 @file The TEFUnit header file which tests the OpenChannel
*/

#ifndef CCSATOPENCHANNELFU_H
#define CCSATOPENCHANNELFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

_LIT8(KNoAlpha, "KNoAlpha");


class CCSatOpenChannelFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyOpenChannelPCmd0001L();
	void TestNotifyOpenChannelPCmd0002L();
	void TestNotifyOpenChannelPCmd0004L();
	void TestNotifyOpenChannelPCmd0001bL();
	void TestNotifyOpenChannelPCmd0001cL();
	void TestNotifyOpenChannelPCmd0001dL();
	
private:
   /**
    * prepare tlv buffer for OpenChannel proactive command 
    * @param aTlv - tlv buffer to prepare
    * @param aCmdNum - command number
    * @param aCmdQualifier - type of refresh command
    * @param aTon - TON and NPI bitmask
    * @param aNum - address number
    * @param aBearer - berarer type 
    * @param aBufSize - buffer size
    * @param aNetworkAccessName - network access name 
    * @param aAlpha - alpha identifier
    * @param  aUdpTcp - Udp or Tcp param (default Udp) 
    */   	
	static void PrepareTlv(TTlv& aTlv, TUint8 aCmdQualifier, TUint8 aTon, 
			const TDesC8& aNum, TUint8 aBearer, const TDesC8& aNetworkAccessName = KNullDesC8,
			const TDesC8& aAlpha = KNoAlpha, TUint8 aUdpTcp = 0x01, TBool aIPv4 = ETrue); 
	
   /**
    * prepare mock for processing of proactive command
    * @param aTlvDsc - tlv buffer to send
    * @param aRes - result for request completion
    */   	
	void PrepareAndCompleteDispatchL(TPtrC8 aTlvDsc, TInt aRes = KErrNone);
   /**
    * prepare mock for dispatch of sat notification  
    */   	
	void PrepareDispatchL(void);

	/**
	 * Tests all possible "TERMINAL RESPONSE" cases. 
	 * API should return KErrNone for all valid responses and KErrCorrupt 
	 * for those that can not be used with "Open channel" command  
	 */
	void DoTestTerminalResponseL();
	
	RSat::TOpenCsChannelV2 iOpenCsChannel;
	void CCSatOpenChannelFU::ReinitChannel(RSat::TOpenCsChannelV2 &aChannel);
	}; // class CCSatChannelFU

#endif // CCSATOPENCHANNELFU_H

