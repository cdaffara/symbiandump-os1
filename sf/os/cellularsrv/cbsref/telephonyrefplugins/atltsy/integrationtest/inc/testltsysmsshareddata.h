// TestTelephonySmsSharedData.h
// Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
// Implementation of the Class CTestTelephonySmsSharedData

#ifndef TESTTELEPHONYSMSSHAREDDATA_H
#define TESTTELEPHONYSMSSHAREDDATA_H

// system include
#include <e32cmn.h>
#include <e32base.h>
#include <etelmm.h>

/**
 * This class defines the shared data of sms test category between 
 * different test steps.
 */
class CTestLtsySmsSharedData : public CBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsySmsSharedData();
	virtual ~CTestLtsySmsSharedData();
	static CTestLtsySmsSharedData* NewL();
	RArray<RMobileSmsMessaging>& SmsMessagingArray();
	RArray<RMobileSmsStore>& SmsStoreArray();
	void Reset();
	void InitializeL();
private:
	void ConstructL();
private:
	RArray<RMobileSmsMessaging> iMessaging;
	RArray<RMobileSmsStore> iSmsStore;
	};

#endif // TESTTELEPHONYSMSSHAREDDATA_H
