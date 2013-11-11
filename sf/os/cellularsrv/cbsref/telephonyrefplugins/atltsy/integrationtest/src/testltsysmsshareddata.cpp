// TestTelephonySmsSharedData.cpp
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonySmsSharedData

// CTSY include

// user include
#include "TestLtsySmsSharedData.h"

/**
 * Constructor
 * 
 */
CTestLtsySmsSharedData::CTestLtsySmsSharedData()
	{
	}

/**
 * Factory function to create a new CTestLtsySmsSharedData object
 * 
 */
CTestLtsySmsSharedData*CTestLtsySmsSharedData:: NewL()
	{
	CTestLtsySmsSharedData* self = new (ELeave) CTestLtsySmsSharedData();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
 * Destructor
 * 
 */
CTestLtsySmsSharedData::~CTestLtsySmsSharedData()
	{
	Reset();
	}

/**
 * Delete all message, message store objects
 * 
 */
void CTestLtsySmsSharedData::Reset()
	{
	for(TInt i = iMessaging.Count()-1; i >= 0; i--)
		{
		iMessaging[i].Close();
		}
	
	for(TInt j = iSmsStore.Count()-1; j >= 0; j--)
		{
		iSmsStore[j].Close();
		}
	
	iMessaging.Reset();
	iSmsStore.Reset();
	}

/**
 * ConstructL
 * 
 */
void CTestLtsySmsSharedData::ConstructL()
	{
	}

/**
 * return sms array
 * 
 */
RArray<RMobileSmsMessaging>& CTestLtsySmsSharedData::SmsMessagingArray()
	{
	return iMessaging;
	}

/**
 * return sms store array
 * 
 */
RArray<RMobileSmsStore>& CTestLtsySmsSharedData::SmsStoreArray()
	{
	return iSmsStore;
	}

// end of file
