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
* Description: ExifLibTest
*
*/


#include "AssertFailure.h"


CAssertFailure* CAssertFailure::NewL (const TDesC8& aMessage,
									  TInt aLineNumber,
									  const TDesC8& aFileName)
	{
	CAssertFailure* self=NewLC(aMessage, aLineNumber, aFileName);
	CleanupStack::Pop();
	return self;
	}


CAssertFailure* CAssertFailure::NewLC (const TDesC8& aMessage,
									   TInt aLineNumber,
									   const TDesC8& aFileName)

	{
	CAssertFailure* self = new (ELeave) CAssertFailure(aLineNumber);
	CleanupStack::PushL(self);
	self->ConstructL(aMessage, aFileName);
	return self;
	}


CAssertFailure* CAssertFailure::NewL (CAssertFailure& aAssertFailure)
	{
	CAssertFailure* self=NewLC(aAssertFailure);
	CleanupStack::Pop();
	return self;
	}


CAssertFailure* CAssertFailure::NewLC (CAssertFailure& aAssertFailure)
	{
	CAssertFailure* self = new(ELeave)CAssertFailure;
	CleanupStack::PushL(self);
	self->ConstructL(aAssertFailure);
	return self;
	}


void CAssertFailure::ConstructL (const TDesC8& aMessage,
								 const TDesC8& aFileName)
	{
    iMessage = aMessage.AllocL();
    iFileName = aFileName.AllocL();
	}


void CAssertFailure::ConstructL (CAssertFailure& aAssertFailure)
	{
	iLineNumber = aAssertFailure.iLineNumber;
	iMessage = aAssertFailure.iMessage->AllocL();
	iFileName = aAssertFailure.iFileName->AllocL();
	}


CAssertFailure::CAssertFailure ()
	{
	}


CAssertFailure::CAssertFailure (TInt aLineNumber)
 : iLineNumber (aLineNumber)
	{
	}


CAssertFailure::~CAssertFailure () 
	{
    delete iFileName;
	delete iMessage;
	}


const TDesC8& CAssertFailure::What() const
	{
	return *iMessage;
	}


TInt CAssertFailure::LineNumber()  const
	{
	return iLineNumber;
	}


const TDesC8& CAssertFailure::FileName()  const
	{
	return *iFileName;
	}


void CAssertFailure::SetMyHeapCellCount (TInt aHeapCellCount)
	{
	iMyHeapCellCount = aHeapCellCount;
	}
	

TInt CAssertFailure::MyHeapCellCount ()
	{
	return iMyHeapCellCount;
	}
