
// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This program is designed to test the MMF_ACLNT.
// 
//

/**
 @file TSI_MMFACLNT.cpp
*/
#include <mda/common/audio.h>
#include <mda/common/gsmaudio.h>
#include "TSI_MMFACLNT.h"
#include "TSI_MMFACLNT.inl"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mda/common/mdagsmwavcodec.h>
#endif

void CPlayerCallbackHandler::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/) 
	{ 
	iMchObserver->MchoComplete(ID(),aError); 
	}

void CPlayerCallbackHandler::MapcPlayComplete(TInt aError)
	{
	iMchObserver->MchoComplete(ID(),aError); 
	}

void CStateCallbackHandler::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
	{
	iMchObserver->MchoComplete(ID(),aErrorCode); 
	}

void CToneCallbackHandler::MatoPrepareComplete(TInt aError)
	{ 
	iMchObserver->MchoComplete(ID(),aError); 
	}

void CToneCallbackHandler::MatoPlayComplete(TInt aError)
	{ 
	iMchObserver->MchoComplete(ID(),aError); 
	}

/**
 * Timeout function
 */
void CTestMmfAclntStep::WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
	{
	TRequestStatus timerStatus;
	RTimer timer ;
	timer.CreateLocal() ;
	timer.After(timerStatus,aNumberOfMicroSeconds);

	User::WaitForRequest(aStatus, timerStatus);
	if (timerStatus == KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	else
		{
		INFO_PRINTF1(_L("Time is over!!!")) ;
		}
	timer.Close() ;
	}

/**
 * Time comparison utility function
 *
 * @param	"const TUint aActual"
 *			The actual timer value produced
 * @param	"const TUint aExpected"
 *			Expected timer value
 * @param	"const TUint aDeviation"
 *			Allowed deviation of the expected value
 *			from the actual value.
 * @return	"TBool"
 *			Did actual timed value fall within deviation limits
 */ 
TBool CTestMmfAclntStep::TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation)
	{
	// save unnessary conditions
	if(aActual == aExpected)
		return ETrue;	

	// Prevent unsigned wrapping errors 
	TUint difference;
	if(aActual > aExpected)
		difference = aActual - aExpected;
	else
		difference = aExpected - aActual;

	// comapare
	if(difference < aDeviation)
		return ETrue;
	return EFalse;
	}

/**
 * Test Preample routines.
 *
 * Creates our own Active Scheduler.
 *
 * @return	"TVerdict"
 *			Did Preamble complete.
 */
TVerdict CTestMmfAclntStep::DoTestStepPreambleL()
	{

	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);
	
	
	return EPass;
	}

/**
 * Test Postamble routines.
 *
 * Destroys our Active Scheduler.
 *
 * @return	"TVerdict"
 *			Did Postamble complete.
 */
TVerdict CTestMmfAclntStep::DoTestStepPostambleL()
	{
	delete iActiveScheduler;
	iActiveScheduler = NULL;

	return EPass;
	}

/**
 * CTestMMFACLNTStep Implementation
 *
 * @parameter	"const CTestSuite* aTestSuite"
 *				Sets test suite pointer
 */
void CTestMmfAclntStep::SetTestSuite(const CTestSuite* aTestSuite )
	{
	iTestSuite = aTestSuite; 
	}  

/**
 * CTestMMFACLNTStep Implementation
 */
CTestMmfAclntStep::CTestMmfAclntStep()
	:iActiveScheduler( NULL )
	{}

//------------------------------------------------------

/**
 * Deconstructors destroys iFormat and iCodec
 *
 */
TVerdict CTestMmfAclntCodecTest::DoTestStepPostambleL()
	{
	if(iFormat != NULL)
		{
		delete iFormat;
		iFormat = NULL;
		}
	if(iCodec != NULL)
		{
		delete iCodec;
		iCodec = NULL;
		}
	return CTestMmfAclntStep::DoTestStepPostambleL();
	}

/**
 * Setup codec and format to test
 *
 * @parameter	"const TTestFormat aFormat"
 *				enum of format to use
 */
void CTestMmfAclntCodecTest::SetupFormatL(const TTestFormat aFormat)
	{
	if(iFormat)
		delete iFormat;
	if(iCodec)
		delete iCodec;
	iFormat = NULL;
	iCodec = NULL;

	switch(aFormat)
		{
	case EPcm16Wav:
		iFormat = new (ELeave) TMdaWavClipFormat;
		CleanupStack::PushL(iFormat);
		iCodec = new (ELeave) TMdaPcmWavCodec;
		CleanupStack::PushL(iCodec);
		break;
	case EMulawRaw:
		iFormat = new (ELeave) TMdaRawAudioClipFormat;
		CleanupStack::PushL(iFormat);
		iCodec = new (ELeave) TMdaAlawRawAudioCodec;
		CleanupStack::PushL(iCodec);
		break;
	case E16BitAu:
		iFormat = new (ELeave) TMdaAuClipFormat;
		CleanupStack::PushL(iFormat);
		iCodec = new (ELeave) TMdaAuCodec;
		CleanupStack::PushL(iCodec);
		break;
	case EAlawAu:
		iFormat = new (ELeave) TMdaAuClipFormat;
		CleanupStack::PushL(iFormat);
		iCodec = new (ELeave) TMdaAlawAuCodec;
		CleanupStack::PushL(iCodec);
		break;
	case EPcm16Au:
		iFormat = new (ELeave) TMdaAuClipFormat;
		CleanupStack::PushL(iFormat);
		iCodec = new (ELeave) TMdaPcm16BitAuCodec;
		CleanupStack::PushL(iCodec);
		break;
	case EImaAdpcmWav:
		iFormat = new (ELeave) TMdaWavClipFormat;
		CleanupStack::PushL(iFormat);
		iCodec = new (ELeave) TMdaImaAdpcmWavCodec;
		CleanupStack::PushL(iCodec);
		break;
	case EAlawWav:
		iFormat = new (ELeave) TMdaWavClipFormat;
		CleanupStack::PushL(iFormat);
		iCodec = new (ELeave) TMdaAlawWavCodec;
		CleanupStack::PushL(iCodec);
		break;
	case EPcmU16:
		iFormat = new (ELeave) TMdaRawAudioClipFormat();
		CleanupStack::PushL(iFormat);
		iCodec = new (ELeave) TMdaUB16RawAudioCodec();
		CleanupStack::PushL(iCodec);
		break;
	case EPcmU8:
		iFormat = new (ELeave) TMdaRawAudioClipFormat();
		CleanupStack::PushL(iFormat);
		iCodec = new (ELeave) TMdaU8PcmRawAudioCodec();
		CleanupStack::PushL(iCodec);
		break;
	case EImasPcmWav:
		iFormat = new (ELeave) TMdaWavClipFormat();
		CleanupStack::PushL(iFormat);
		iCodec = new (ELeave) TMdaImaAdpcmWavCodec();
		CleanupStack::PushL(iCodec);
		break;
	case EPcm8:
		iFormat = new (ELeave) TMdaWavClipFormat();
		CleanupStack::PushL(iFormat);
		iCodec = new (ELeave) TMdaPcmWavCodec(TMdaPcmWavCodec::E8BitPcm);
		CleanupStack::PushL(iCodec);
		break;
	case EGsmWav:
		iFormat = new (ELeave) TMdaWavClipFormat;
		CleanupStack::PushL(iFormat);
		iCodec = new (ELeave) TMdaGsmWavCodec;
		CleanupStack::PushL(iCodec);
		break;
	case EEpocWve:
	case ENone:
	default:
		// will create an inconlusive result as preamble leaves.
		iFormat = NULL;
		iCodec = NULL;
		break;
		}

	if((iFormat != NULL) && (iCodec != NULL))
		{
		CleanupStack::Pop(); // iFormat
		CleanupStack::Pop(); // iCodec
		}		
	}

