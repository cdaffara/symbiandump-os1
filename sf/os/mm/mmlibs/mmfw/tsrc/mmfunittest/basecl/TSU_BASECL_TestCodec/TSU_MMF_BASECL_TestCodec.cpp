/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include "TSU_MMF_BASECL_TestCodec.h"


// __________________________________________________________________________
// Implementation

CMMFCodec* CMMF_TSU_BASECL_TestCodec::NewL(TAny* aInitParams)
	{
	CMMF_TSU_BASECL_TestCodec* self=new(ELeave) CMMF_TSU_BASECL_TestCodec();
	CleanupStack::PushL(self);
	self->ConstructL(aInitParams);
	CleanupStack::Pop(self);
	return STATIC_CAST( CMMFCodec*, self );
	}

CMMF_TSU_BASECL_TestCodec::~CMMF_TSU_BASECL_TestCodec()
	{

	}

CMMF_TSU_BASECL_TestCodec::CMMF_TSU_BASECL_TestCodec()
	{
	}

/**
*
* Test codec constructor
* The test codec on construction creates a test file
* whose presence is checked for in the TSU_MMF_BASECL test harness
* to verify the codec has been created
*
*/
void CMMF_TSU_BASECL_TestCodec::ConstructL(TAny* /*aInitParams*/)
	{
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());

	_LIT(KTestFileName,"c:\\MMF_TSU_BASECL_TestCodecCreated");

	TPtrC fileName(KTestFileName);

	RFile testFile; //handle to test file to verify that the test codec has been created
	TInt err = testFile.Create(fsSession,fileName,EFileWrite);

	testFile.Close();
	fsSession.Close();

	User::LeaveIfError(err);
	}

/**
*
* Converts upper case ascii to lower case ascii
*
*/
TCodecProcessResult CMMF_TSU_BASECL_TestCodec::ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst)
	{//this codec converts upper case to lower case
	//convert from generic CMMFBuffer to CMMFDataBuffer
	iSrc = STATIC_CAST(const CMMFDataBuffer*, &aSrc);
	iDst = STATIC_CAST(CMMFDataBuffer*, &aDst);

	const TUint dstMaxLen = iDst->Data().MaxLength();
	const TUint dstAvailableLength = dstMaxLen - iDst->Position();
	const TUint srcLen = iSrc->Data().Length();
	const TUint sourceRemain = srcLen - iSrc->Position();

	//make sure we don't blow source by checking against remaining
	//and clipping to minimum remaining if necessary
	TUint8* pSrc = CONST_CAST(TUint8*,iSrc->Data().Ptr());
	pSrc += iSrc->Position();
	TUint8* pDst = CONST_CAST(TUint8*,iDst->Data().Ptr());
	pDst += iDst->Position();

	TUint samplesToProcess = Min((TInt)sourceRemain, (TInt)dstAvailableLength);
	TUint sampleCount = samplesToProcess;
	TUint8 A('A');
	TUint8 UpperToLowerConvertionConstant(0x20);
	while (sampleCount--)
		{ 
		if (*pSrc >= A) *pDst++ = (TUint8)(UpperToLowerConvertionConstant + *pSrc++); //check test is not a number or punctuation
		else *pDst++ = *pSrc++;
		}

	TCodecProcessResult result;
	if (dstAvailableLength == sourceRemain) result.iStatus = TCodecProcessResult::EProcessComplete;
	else if (dstAvailableLength > sourceRemain) result.iStatus = TCodecProcessResult::EDstNotFilled;
	else if (dstAvailableLength < sourceRemain) result.iStatus = TCodecProcessResult::EProcessIncomplete;

	result.iSrcBytesProcessed = samplesToProcess;
	result.iDstBytesAdded = samplesToProcess;

	iDst->Data().SetLength(iDst->Position() + result.iDstBytesAdded);

	return result;
	}

