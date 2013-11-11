// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#include "TE_smspdudbbase.h"

void CSmsPduDbTestStep::PrintBuffer(const CSmsPduDbBase& aPdu)
/**
 *  utility function PrintBuffer
 */
{
	const CSmsBufferBase& buffer = aPdu.iSmsMessage->Buffer();
	const TInt len = buffer.Length();
	const TInt KMaxPrintLen = 200;
	for (TInt j = 0; j < len; j += KMaxPrintLen)
	{
	TBuf<KMaxPrintLen> buf;
	buf.Zero();
	const TInt midLen = Min(KMaxPrintLen, len - j);
	buffer.Extract(buf, j, midLen);
	INFO_PRINTF2(_L("\tBody: %S"), &buf);
	}
}

void CSmsPduDbTestStep::PrintMessage(TInt aIndex, const CSmsPduDbMessage& aMessage)
/**
 *  utility function PrintMessage
 */
{
	INFO_PRINTF4(_L("%d Message [Id=%d Ttpe=%d Body=...]"), aIndex, aMessage.iId, aMessage.iType);
	PrintBuffer(aMessage);
}


void CSmsPduDbTestStep::PrintPdu(TInt aIndex, const CSmsPduDbPdu& aPdu)
/**
 *  utility function PrintPdu
 */
{
	INFO_PRINTF6(_L("%d Pdu [Id=%d Ttpe=%d DecodeErr=%d MsgErr=%d Body=...]"), aIndex, aPdu.iId, aPdu.iType, aPdu.iDecodeError, aPdu.iSmsMessageError);
	if (aPdu.iDecodeError == KErrNone && aPdu.iSmsMessageError == KErrNone)
	{
	PrintBuffer(aPdu);
	}
}

void CSmsPduDbTestStep::PrintSegment(TInt aIndex, const CSmsPduDbConcatSegment& aPdu)
/**
 *  utility function PrintSegment
 */
{
	INFO_PRINTF7(_L("%d Segment [Id=%d Segment=%d Ttpe=%d DecodeErr=%d MsgErr=%d Body=...]"), aIndex, aPdu.iId, aPdu.iSegment, aPdu.iType, aPdu.iDecodeError, aPdu.iSmsMessageError);
	if (aPdu.iDecodeError == KErrNone && aPdu.iSmsMessageError == KErrNone)
	{
	PrintBuffer(aPdu);
	}
}

void CSmsPduDbTestStep::PrintConcat(TInt aIndex, const CSmsPduDbConcat& aConcat)
/**
 *  utility function PrintConcat
 */
{
	const TInt count = aConcat.iSegments.Count();
	INFO_PRINTF5(_L("%d Concat [Id=%d Concat=%d MsgErr=%d Body=...]"), aIndex, aConcat.iId, count, aConcat.iSmsMessageError);
	if (aConcat.iSmsMessageError == KErrNone)
	{
	PrintBuffer(aConcat);
	}
	INFO_PRINTF1(_L("\tSegments..."));
	for (TInt i = 0; i < count; i++)
	{
	PrintSegment(i, *aConcat.iSegments[i]);
	}
}

void CSmsPduDbTestStep::DisplayCompileErrorL(const CTestConfigSection& aSection, const CTestConfigItem& aItem, TInt aError)
/**
 *  utility function DisplayCompileErrorL
 */
{
	const TDesC8& itemTag = aItem.Item();
	const TDesC8& itemVal = aItem.Value();
	const TDesC8& sectionName = aSection.SectionName();

	HBufC* itemTag16 = HBufC::NewLC(itemTag.Length());
	itemTag16->Des().Copy(itemTag);
	HBufC* itemVal16 = HBufC::NewLC(itemVal.Length());
	itemVal16->Des().Copy(itemVal);
	HBufC* sectionName16 = HBufC::NewLC(sectionName.Length());
	sectionName16->Des().Copy(sectionName);

	INFO_PRINTF4(_L("Section: \"%S\" Item: \"%S\" Error: %d"), sectionName16, itemTag16, aError);

	const TInt KMaxBodyLen = 128;
	const TInt valLen = itemVal16->Length();

	for (TInt i = 0; i < valLen; i += KMaxBodyLen)
	{
	const TInt midLen = Min(KMaxBodyLen, valLen - i);
	const TPtrC valPtr(itemVal16->Mid(i, midLen));
	INFO_PRINTF2(_L("\tValue: %S"), &valPtr);
	}

	CleanupStack::PopAndDestroy(sectionName16);
	CleanupStack::PopAndDestroy(itemVal16);
	CleanupStack::PopAndDestroy(itemTag16);

}

TVerdict CSmsPduDbTestStep::doTestStepPreambleL( void )
{
	__UHEAP_MARK;

	User::LeaveIfError(iFs.Connect());

	iSmsStackTestUtils = CSmsStackTestUtils::NewL(this, iFs);

	return TestStepResult() ;
}

TVerdict CSmsPduDbTestStep::doTestStepPostambleL( void )
{
	delete iSmsStackTestUtils;
	iSmsStackTestUtils = NULL;
	
	iFs.Close();

	__UHEAP_MARKEND;

	return TestStepResult() ;
}

