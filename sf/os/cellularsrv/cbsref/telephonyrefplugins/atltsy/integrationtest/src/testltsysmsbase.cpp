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
// Implementation of the Class CTestLtsySmsBase
// 
//

// user include
#include "testltsysmsbase.h"
#include "testltsysmsshareddata.h"

//const define
_LIT16(KPhoneTSY, "phonetsy");


/**
 * Constructor
 * 
 */
CTestLtsySmsBase::CTestLtsySmsBase(CTestLtsyModel& aTestModel):
                                   iModel(aTestModel)
	{
	}

/**
 * Destructor
 *
 */
CTestLtsySmsBase::~CTestLtsySmsBase()
	{

	}

TVerdict CTestLtsySmsBase::doTestStepPreambleL()
	{
	OpenPhoneL();
	
	if (TestStepResult() == EPass)
		{
		GetIndex();
		}

	return TestStepResult();
	}

TVerdict CTestLtsySmsBase::doTestStepPostambleL()
	{
	return TestStepResult();
	}

/**
 * Get sms message number
 * 
 */
void CTestLtsySmsBase::GetIndex()
	{
	iIndexNum = 0;
	iStoreIndex = 0;
	}

void CTestLtsySmsBase::OpenPhoneL()
	{
	INFO_PRINTF1(_L("starting opne phone"));
		
	if (!iModel.SharedEnv().PhoneOpened())
		{
		iModel.SharedEnv().OpenPhoneL(KPhoneTSY);
		}
	}
/**
 * Open sms message
 * 
 */
void CTestLtsySmsBase::OpenSmsL()
	{
	RMobileSmsMessaging smsMsg;
	iModel.SmsSharedData().SmsMessagingArray().AppendL(smsMsg);
    
	iIndexNum = iModel.SmsSharedData().SmsMessagingArray().Count()-1;
    
	TInt ret( KErrNone );
    ret = Sms().Open(iModel.SharedEnv().Phone());
	if (KErrNone != ret)
		{
		INFO_PRINTF1(_L("Can't open the RMobileSmsMessaging service"));
		SetTestStepResult(EFail);
		}
	}

/**
 * Get the sms object
 * 
 */
RMobileSmsMessaging& CTestLtsySmsBase::Sms()
	{
	return iModel.SmsSharedData().SmsMessagingArray()[iIndexNum];
	}

/**
 * Get the sms store object
 * 
 */
RMobileSmsStore& CTestLtsySmsBase::SmsStore()
	{
	return iModel.SmsSharedData().SmsStoreArray()[iStoreIndex];
	}

/**
 * Open sms message store
 * 
 */
void CTestLtsySmsBase::OpenSmsStoreL()
	{
	RMobileSmsStore smsStore;
	iModel.SmsSharedData().SmsStoreArray().AppendL(smsStore);

	iStoreIndex = iModel.SmsSharedData().SmsStoreArray().Count()-1;

	TInt ret = SmsStore().Open(Sms(), KETelIccSmsStore);
	if (KErrNone != ret)
		{
		ERR_PRINTF1(_L("Can't open the RMobileSmsStore service"));
		SetTestStepResult(EFail);
		}
	}

/**
 * Close sms
 */
void CTestLtsySmsBase::CloseSmsL()
	{
	Sms().Close();
	iModel.SmsSharedData().SmsMessagingArray().Remove(iIndexNum);
	iModel.SmsSharedData().SmsMessagingArray().Compress();
	}

/**
 * Close sms store
 * 
 */
void CTestLtsySmsBase::CloseSmsStoreL()
	{
	SmsStore().Close();
	iModel.SmsSharedData().SmsStoreArray().Remove(iStoreIndex);
	iModel.SmsSharedData().SmsStoreArray().Compress();
	}

// end of file
