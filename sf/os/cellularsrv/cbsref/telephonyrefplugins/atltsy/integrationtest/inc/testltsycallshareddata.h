// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file testltsycallshareddata.h
// 

#ifndef TESTTELEPHONYCALLSHAREDDATA_H
#define TESTTELEPHONYCALLSHAREDDATA_H

// system include
#include <e32cmn.h>
#include <e32base.h>
#include <etelmm.h>
#include <etel3rdparty.h>
#include <RMmCustomAPI.h>

/**
 * This class defines the shared data related to phone call among test steps.
 * 
 */

class CTestLtsyCallSharedData : public CBase
	{
/**
 * @internalAll
 * @test
 */	
	
public:
	static CTestLtsyCallSharedData* NewL();
	virtual ~CTestLtsyCallSharedData();
	RLine& VoiceLine();
	RMobileCall& VoiceCallOne();
	RMobileCall& VoiceCallTwo();
	RMobileCall& EmergencyCall();
	RMmCustomAPI& MmCustomAPI();
	CTelephony*& ThirdPartyTelephony();
	
protected:
	CTestLtsyCallSharedData();
	void ConstructL();
	
private:
	RLine iVoiceLine;
	RMobileCall iVoiceCallOne;
	RMobileCall iVoiceCallTwo;
	RMobileCall iEmergencyCall;
	RMmCustomAPI iMmCustomAPI;
	CTelephony* iThirdPartyTel;
	};

#endif 
