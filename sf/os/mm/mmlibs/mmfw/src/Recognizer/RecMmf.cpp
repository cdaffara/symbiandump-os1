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
//

#include <f32file.h>
#include <barsread.h>
#include "RecMmf.h"
#include "mmfcontrollerpluginresolver.h"

const TInt KMimeMmfRecognizerValue = 0x101F7C0C;
const TUid KUidMimeMmfRecognizer = {KMimeMmfRecognizerValue};
const TInt KMmfRecognizerPriority = 10; // The recognizer priority is set to 10(a value between ENormal and EHigh)


// the minimum buffer size that will theoretically guarantee recognition 
const TInt KPreferredBufSize = 256;

// the granularity of the internal MIME type array 
const TInt KMimeArrayGranularity = 4;

// CApaMmfRecognizer

/**
 * @internalAll
 *
 * Call base constructor with the recognizer's UID and confidence level
 * The MMF recognizer priority is set to 10(a value between ENormal and EHigh) to allow 
 * third-party recognizers to specify high priority.
 */
CApaMmfRecognizer::CApaMmfRecognizer()
	:CApaDataRecognizerType(KUidMimeMmfRecognizer,KMmfRecognizerPriority)
	{
	}

CApaMmfRecognizer::~CApaMmfRecognizer()
	{
	delete iMmfRecognizer;
	}

/**
 * @internalAll
 *
 * Return the supposed minimum buffer size we need to 
 * successfully recognize the data
 */
TUint CApaMmfRecognizer::PreferredBufSize()
	{
	return KPreferredBufSize;
	}

/**
 * @internalAll
 *
 * Gets one of the data (MIME) types that the recognizer can recognize.
 */
TDataType CApaMmfRecognizer::SupportedDataTypeL(TInt aIndex) const
	{
	return (iMmfRecognizer->SupportedDataTypeL(aIndex));
	}


/**
 * @internalAll
 *
 * Attempt to recognize the data.
 * This recognizer only attempts to match the data header on its own,
 * or the data header plus the file suffix together.
 *
 * NB if the file is not recognized, this function should NOT leave :
 * it should instead set iConfidence = ENotRecognized and return
 * the function should only leave if there is an out-of-memory condition
 */
void CApaMmfRecognizer::DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer)
	{
	// assume Match will fail :
	iConfidence = CApaDataRecognizerType::ENotRecognized;

	CMmfRecognizer::TMatchMethod matchMethod = iMmfRecognizer->MatchL(aName,aBuffer);
	
	// return whether the data was matched by setting iConfidence
	// if matched the MIME type is returned in iDataType
	if (matchMethod == CMmfRecognizer::ENotMatched)
		{	
		iConfidence = CApaDataRecognizerType::ENotRecognized;
		}
	else
		{
		iDataType = iMmfRecognizer->MimeString();

		//Match on data only
		if (matchMethod == CMmfRecognizer::EBySignature)
			iConfidence = CApaDataRecognizerType::EPossible;
		else
			{//Match on data and suffix
			ASSERT(matchMethod == CMmfRecognizer::EByName);
			iConfidence = CApaDataRecognizerType::EProbable;
			}

		}
	}

void CApaMmfRecognizer::ConstructL()
	{
	iMmfRecognizer = CMmfRecognizer::NewL();
    iCountDataTypes = iMmfRecognizer->NumMimeTypes();
	}

CApaMmfRecognizer* CApaMmfRecognizer::NewL()
	{
	CApaMmfRecognizer* self = new (ELeave) CApaMmfRecognizer();   
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// CMmfRecognizer - the main utility class owner by CApaMmfRecognizer

CMmfRecognizer::CMmfRecognizer()
	{
	}

CMmfRecognizer::~CMmfRecognizer()
	{
	delete iMmfRecognizerUtil;
	delete iMimeTypes;
	}

void CMmfRecognizer::ConstructL()
	{
	iMimeTypes = new (ELeave) CDesC8ArrayFlat(KMimeArrayGranularity);
	BuildListL();
	iMmfRecognizerUtil = CMmfRecognizerUtil::NewL();
	}

CMmfRecognizer* CMmfRecognizer::NewL()
	{
	CMmfRecognizer* self = new (ELeave) CMmfRecognizer();   
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
 * @internalAll
 *
 * Return the number of MIME types supported 
 */
TInt CMmfRecognizer::NumMimeTypes() const
	{
	return(iMimeTypes->Count());
	}


/**
 * @internalAll
 *
 * Call into the MMF Controller Framework DLL to get
 * a list of supported MIME types
 */
void CMmfRecognizer::BuildListL()
	{
	iMimeTypes->Reset();
	CMmfRecognizerUtil::GetMimeTypesL(iMimeTypes);
	}


/**
 * @internalAll
 *
 * Get one of the data (MIME) types that MMF can recognize.
 */
const TPtrC8 CMmfRecognizer::SupportedDataTypeL(TInt aIndex) const
	{
	if ((aIndex < 0) || (aIndex >= iMimeTypes->Count()))
		User::Leave(KErrArgument);

	return(iMimeTypes->MdcaPoint(aIndex));
	}


/**
 * @internalAll
 *
 * Get a reference to the last MIME type string successfully matched
 */
const TDesC8& CMmfRecognizer::MimeString() const
	{
	return iMimeString;
	}

/**
 * @internalAll
 *
 * Attempt to recognize the data
 */
CMmfRecognizer::TMatchMethod CMmfRecognizer::MatchL(const TDesC& aFileName, const TDesC8& aBuffer)
	{
	CMmfRecognizerUtil::TMatchLevel matchLevel = CMmfRecognizerUtil::EMatchNone;
	matchLevel = iMmfRecognizerUtil->GetMimeTypeL(aFileName, aBuffer, iMimeString);
	
	TMatchMethod bestMatchMethod = ENotMatched;
	if(matchLevel==CMmfRecognizerUtil::EMatchData)
		bestMatchMethod = EBySignature;
	else if(matchLevel==CMmfRecognizerUtil::EMatchName)
		bestMatchMethod = EByName;

	return bestMatchMethod;
	}


#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x101F7C41, CApaMmfRecognizer::NewL)
	};


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }


