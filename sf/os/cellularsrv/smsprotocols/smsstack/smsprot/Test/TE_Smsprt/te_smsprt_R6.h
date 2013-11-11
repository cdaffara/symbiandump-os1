// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TE_SMSPRT_R6_H
#define TE_SMSPRT_R6_H



#include "smsstackbaseteststeps.h"


class CTestSinglePDUHyperLinks : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestMultiplePDUHyperLinks : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSinglePDUReplyAddress : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestMultiplePDUReplyAddress : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSpecialMessageOperations : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSinglePDUSpecialMessageWaiting : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestMultiplePDUSpecialMessageWaiting : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CSmsEnhancedVoiceMailTestStep : public CSmsBaseTestStep
{
protected:
	CEnhancedVoiceMailNotification* CreateEnhancedVoiceMailNotificationL(TGsmSmsTelNumber aAccessAddress);
	CVoiceMailNotification*  CreateVoiceMailNotificationL(TGsmSmsTelNumber aCallingLineIdentity);
};


class CTestEnhancedVoiceMailOperations : public CSmsEnhancedVoiceMailTestStep
	{
public:
	enum TBoundaries {EInside=0,EOn,EOutside};
	enum TAddressType {EInternational=0,EParsed};

	virtual TVerdict doTestStepL();

protected:
	void AddAddressToNotifcationL( 	CEnhancedVoiceMailNotification* aEnhancedVoiceMailNotification,
		CVoiceMailNotification* aVoiceMailNotification,
		TPtrC aCallingLineIdentity,TUint aAddressType);
	};


class CTestSinglePDUEnhancedVoiceMailInformation : public CSmsEnhancedVoiceMailTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestMultiplePDUEnhancedVoiceMailInformation : public CSmsEnhancedVoiceMailTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestEnhancedVoiceMailDeleteOperations : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSinglePDUEnhancedVoiceMailDeleteConfirmation : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestMultiplePDUEnhancedVoiceMailDeleteConfirmation : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTest7BitDefaultAlphabet : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestZeroLengthIE : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestAutomaticDeletionInDCS : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestAutomaticDeletionInDCSAndInESK : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestAutomaticDeletionTypeZeroSMS : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestAutomaticDeletionTypeZeroAndSetInESK : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestOOMSendR6Sms : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestEnhancedVoiceMessageBoundary : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestEnhancedVoiceMessageBoundary1 : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestSubmitReportDefaultsTo7BitDecoding : public CSmsBaseTestStep
    {
public:
    virtual TVerdict doTestStepL();
    };


class CTestDeliverReportDefaultsTo7BitDecoding : public CSmsBaseTestStep
    {
public:
    virtual TVerdict doTestStepL();
    };


class CTestStatusReportDefaultsTo7BitDecoding : public CSmsBaseTestStep
    {
public:
    virtual TVerdict doTestStepL();
    };

#endif // TE_SMSPRT_R6_H
