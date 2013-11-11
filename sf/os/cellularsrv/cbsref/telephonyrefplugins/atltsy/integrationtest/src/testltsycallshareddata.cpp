// TestTelephonyCallSharedData.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonyCallSharedData

// CTSY include

// user include
#include "testltsycallshareddata.h"


CTestLtsyCallSharedData* CTestLtsyCallSharedData::NewL()
	{
	CTestLtsyCallSharedData* self = new (ELeave) CTestLtsyCallSharedData;
	CleanupStack::PushL(self);
	
	self->ConstructL();
	
	CleanupStack::Pop();
	return self;
	}

CTestLtsyCallSharedData::~CTestLtsyCallSharedData()
	{
	iVoiceLine.Close();
	
	iVoiceCallOne.Close();
	
	iVoiceCallTwo.Close();
	
	iEmergencyCall.Close();
	
	iMmCustomAPI.Close();
	
	delete iThirdPartyTel;
	iThirdPartyTel = NULL;
	}


CTestLtsyCallSharedData::CTestLtsyCallSharedData()
	{
	
	}


void CTestLtsyCallSharedData::ConstructL()
	{
	
	}

RLine& CTestLtsyCallSharedData::VoiceLine()
	{
	return iVoiceLine;
	}

RMobileCall& CTestLtsyCallSharedData::VoiceCallOne()
	{
	return iVoiceCallOne;
	}

RMobileCall& CTestLtsyCallSharedData::VoiceCallTwo()
	{
	return iVoiceCallTwo;
	}

RMobileCall& CTestLtsyCallSharedData::EmergencyCall()
	{
	return iEmergencyCall;
	}

RMmCustomAPI& CTestLtsyCallSharedData::MmCustomAPI()
	{
	return iMmCustomAPI;
	}

CTelephony*& CTestLtsyCallSharedData::ThirdPartyTelephony()
	{
	return iThirdPartyTel;
	}

// end of file
