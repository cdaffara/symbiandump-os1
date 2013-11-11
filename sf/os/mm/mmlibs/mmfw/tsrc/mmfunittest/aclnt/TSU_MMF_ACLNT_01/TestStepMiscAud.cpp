
// TestStepMiscAud.cpp

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

#include "TestStepMiscAud.h"

//
// RTestStepAudInStreamCustIntf - exercise CMdaAudioInputStream::CustomInterface
//

RTestStepAudInStreamCustIntf* RTestStepAudInStreamCustIntf::NewL()
	{
	RTestStepAudInStreamCustIntf* self = new (ELeave) RTestStepAudInStreamCustIntf();
	return self;
	}

RTestStepAudInStreamCustIntf::RTestStepAudInStreamCustIntf()
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-1000-HP");
	}

TVerdict RTestStepAudInStreamCustIntf::DoTestStepL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler; // only indirectly used 
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);
	INFO_PRINTF1(_L("Test CMdaAudioInputStream::CustomInterface()"));

	TVerdict result = EPass;

	CMdaAudioInputStream* stream = CMdaAudioInputStream::NewL(*this);
	CleanupStack::PushL(stream);
	TAny* returnVal = stream->CustomInterface(KNullUid);
	if (returnVal != NULL)
		{
		INFO_PRINTF2(_L("Non-NULL return value %0x"), returnVal);
		result = EFail;
		}

	CleanupStack::PopAndDestroy(2, scheduler);
	return result;
	}

void RTestStepAudInStreamCustIntf::MaiscOpenComplete(TInt /*aError*/)
	{
	ASSERT(EFalse); // not expected to reach here in this version
	}

void RTestStepAudInStreamCustIntf::MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	ASSERT(EFalse); // not expected to reach here in this version
	}

void RTestStepAudInStreamCustIntf::MaiscRecordComplete(TInt /*aError*/)
	{
	ASSERT(EFalse); // not expected to reach here in this version
	}

//
// RTestStepAudOutStreamCustIntf - exercise CMdaAudioOutputStream::CustomInterface
//

RTestStepAudOutStreamCustIntf* RTestStepAudOutStreamCustIntf::NewL()
	{
	RTestStepAudOutStreamCustIntf* self = new (ELeave) RTestStepAudOutStreamCustIntf();
	return self;
	}

RTestStepAudOutStreamCustIntf::RTestStepAudOutStreamCustIntf()
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-1001-HP");
	}

TVerdict RTestStepAudOutStreamCustIntf::DoTestStepL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler; // only indirectly used 
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);
	INFO_PRINTF1(_L("Test CMdaAudioOutputStream::CustomInterface()"));

	TVerdict result = EPass;

	CMdaAudioOutputStream* stream = CMdaAudioOutputStream::NewL(*this);
	CleanupStack::PushL(stream);
	TAny* returnVal = stream->CustomInterface(KNullUid);
	if (returnVal != NULL)
		{
		INFO_PRINTF2(_L("Non-NULL return value %0x"), returnVal);
		result = EFail;
		}

	CleanupStack::PopAndDestroy(2, scheduler);
	return result;
	}

void RTestStepAudOutStreamCustIntf::MaoscOpenComplete(TInt /*aError*/)
	{
	ASSERT(EFalse); // not expected to reach here in this version
	}

void RTestStepAudOutStreamCustIntf::MaoscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/)
	{
	ASSERT(EFalse); // not expected to reach here in this version
	}

void RTestStepAudOutStreamCustIntf::MaoscPlayComplete(TInt /*aError*/)
	{
	ASSERT(EFalse); // not expected to reach here in this version
	}

//
// RTestStepTonePlayCustIntf - exercise CMdaAudioToneUtility::CustomInterface
//

RTestStepTonePlayCustIntf* RTestStepTonePlayCustIntf::NewL()
	{
	RTestStepTonePlayCustIntf* self = new (ELeave) RTestStepTonePlayCustIntf();
	return self;
	}

RTestStepTonePlayCustIntf::RTestStepTonePlayCustIntf()
	{
	iTestStepName = _L("MM-MMF-ACLNT-U-1002-HP");
	}

TVerdict RTestStepTonePlayCustIntf::DoTestStepL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler; // only indirectly used 
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);
	INFO_PRINTF1(_L("Test CMdaAudioToneUtility::CustomInterface()"));

	TVerdict result = EPass;

	CMdaAudioToneUtility* tonePlay = CMdaAudioToneUtility::NewL(*this);
	CleanupStack::PushL(tonePlay);
	TAny* returnVal = tonePlay->CustomInterface(KNullUid);
	if (returnVal != NULL)
		{
		INFO_PRINTF2(_L("Non-NULL return value %0x"), returnVal);
		result = EFail;
		}

	CleanupStack::PopAndDestroy(2, scheduler);
	return result;
	}

void RTestStepTonePlayCustIntf::MatoPrepareComplete(TInt /*aError*/)
	{
	ASSERT(EFalse); // not expected to reach here in this version
	}

void RTestStepTonePlayCustIntf::MatoPlayComplete(TInt /*aError*/)
	{
	ASSERT(EFalse); // not expected to reach here in this version
	}

