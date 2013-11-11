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
// The TEFUnit header file which tests the TelephonyAudioControl
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYTELEPHONYAUDIOCONTROLFU_H
#define CCTSYTELEPHONYAUDIOCONTROLFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cmockltsyfactory.h"
#include "cctsycomponenttestbase.h"


class CCTsyTelephonyAudioControlFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	~CCTsyTelephonyAudioControlFU();

public:
	// Individual test steps
	void TestSetupTelephonyAudio0001aL();
	void TestSetupTelephonyAudio0001bL();
	void TestSetupTelephonyAudio0004L();
	void TestTeardownTelephonyAudio0001aL();
	void TestTeardownTelephonyAudio0001bL();
	void TestTeardownTelephonyAudio0004L();
	void TestCallStateChange0001aL();
	void TestCallStateChange0001bL();
	void TestCallStateChange0004L();
	void TestNotifyTelephonyAudioControlError0001L();

private:

    void MOCallAudioTeardownL();
    void MTCallAudioTeardownL();

    void UpdateCallStatusL( RMobileCall& aMobileCall, 
                            RMobileCall::TMobileCallStatus aMobileCallStatus,
                            TBool aUpdateTimer = EFalse,
                            TBool aHangUp = EFalse );
                                
    void DefinePropeties( TBool aReset, TMockLtsyFactoryVersions aVersion );
    void DeletePropeties();
    
    void CheckCallRoutingValues( RLine& aLine, RMobileCall& aCall );
    
    void CheckCallStatus( TName& aCallName, RMobileCall::TMobileCallStatus aMobileCallStatus );
    
    void CheckNoCallRouting();
                            
	}; // class CCTsyTelephonyAudioControlFU

#endif // CCTSYTELEPHONYAUDIOCONTROLFU_H

