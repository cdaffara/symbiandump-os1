// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mmruf.h"
#include <e32uid.h>

#include "constants.h"
#include "parsers.h"

#ifdef _DEBUG
_LIT(KRUFPanic, "MMRUF");
static const TInt KReasonNoFunction = 1;
#endif

//
// A table of recognised file formats.
// It should be sorted in order of format popularity.
//
static const TSignature KSigs[] =
	{
	CUSTOM_SIG(KMimeAAC,		TAACParser::DoRecognise),
	CUSTOM_SIG(KMimeMP3,		TMP3Parser::DoRecognise),
	CUSTOM_SIG(KMimeMP4_V,		TMPEG4Parser::DoRecognise),
	CUSTOM_SIG(KMimeWMV,		TASFParser::DoRecognise),
	CUSTOM_SIG(KMimeMPEG2_V,	TMPEG2Parser::DoRecognise),
	CUSTOM_SIG(KMimeRM_V,		TRMParser::DoRecognise),
	CUSTOM_SIG(KMimeRAM,		TRAMParser::DoRecognise),
	CUSTOM_SIG(KMimeSDP,		TSDPParser::DoRecognise),
	CUSTOM_SIG(KMimeXPS,		TXPSParser::DoRecognise),
	HEADER_SIG(KMimeRA,			KExtRA,			KSigRA),
	HEADER_SIG(KMimeAIFF,		KExtAIFF_1,		KSigAIFF),
	HEADER_SIG(KMimeAIFF,		KExtAIFF_2,		KSigAIFF),
	HEADER_SIG(KMimeAIFF,		KExtAIFF_3,		KSigAIFF),
	HEADER_SIG(KMimeAMR,		KExtAMR,		KSigAMR_1),
	HEADER_SIG(KMimeAMR,		KExtAMR,		KSigAMR_2),
	HEADER_SIG(KMimeAMRWB,		KExtAMRWB,		KSigAMRWB_1),
	HEADER_SIG(KMimeAMRWB,		KExtAMRWB,		KSigAMRWB_2),
	HEADER_SIG(KMimeAU,			KExtAU,			KSigAU),
	HEADER_SIG(KMimeAVI,		KExtAVI,		KSigAVI),
	HEADER_SIG(KMimeDIVX,		KExtDIVX,		KSigDIVX_1),
	HEADER_SIG(KMimeDIVX,		KExtDIVX,		KSigDIVX_2),
	HEADER_SIG(KMimeDLS,		KExtDLS,		KSigDLS),
	HEADER_SIG(KMimeFLAC,		KExtFLAC_1,		KSigFLAC),
	HEADER_SIG(KMimeFLAC,		KExtFLAC_2,		KSigFLAC),
	HEADER_SIG(KMimeGSM,		KExtGSM,		KSigGSM),
	HEADER_SIG(KMimeM3U,		KExtM3U,		KSigM3U),
	HEADER_SIG(KMimeMID,		KExtMID,		KSigMID),
	HEADER_SIG(KMimeMLD,		KExtMLD,		KSigMLD),
	HEADER_SIG(KMimeMMF,		KExtMMF,		KSigMMF),
	HEADER_SIG(KMimeMXMF,		KExtMXMF,		KSigMXMF),
	HEADER_SIG(KMimeOGG,		KExtOGG,		KSigOGG),
	HEADER_SIG(KMimeBeatnikRMF, KExtBeatnikRMF, KSigBeatnikRMF),
	HEADER_SIG(KMimeSMF,		KExtSMF,		KSigSMF),
	HEADER_SIG(KMimeSND,		KExtSND,		KSigSND),
	HEADER_SIG(KMimeWAV,		KExtWAV,		KSigWAV_1),
	HEADER_SIG(KMimeWAV,		KExtWAV,		KSigWAV_2),
	HEADER_SIG(KMimeXMF,		KExtXMF,		KSigXMF),
	CUSTOM_SIG(KMimeMAT_V,		TMatroskaParser::DoRecognise),
	};

static const TInt KDataTypesCount = sizeof(KSigs) / sizeof(TSignature);

//
// Map all combinations of [ext_match][header_match] to confidence levels.
//
static const TInt KResultToConfidence[] =
	{
	KConfNotRecognised,
	KConfProbable,
	KConfPossible,
	KConfCertain
	};
	
//
//
//
const TImplementationProxy ImplementationTable[] =
	{
	// UID taken from Multimedia's UID allocation table.
	IMPLEMENTATION_PROXY_ENTRY(KRecogniserUID, CMMRUF::NewL)
	};


//
//
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }


//
//
//
CMMRUF::CMMRUF()
 : CApaDataRecognizerType(KMMRUFDLLUid, KRecogniserPriority)
	{
	iCountDataTypes = KDataTypesCount;
	}


//
//
//
CMMRUF* CMMRUF::NewL()
	{
	return new(ELeave)CMMRUF();
	}


//
//
//
CMMRUF::~CMMRUF()
	{
	}


//
//
//
TUint CMMRUF::PreferredBufSize()
	{
	return KPreferredBufSize;	
	}


//
// Required for CApaDataRecognizerType parent class.
// It creates a mime-type string for the signature at aIndex.
//
TDataType CMMRUF::SupportedDataTypeL(TInt aIndex) const
	{
	if ((aIndex < 0) || (aIndex >= KDataTypesCount))
		{
		User::Leave(KErrArgument);
		}

	return SupportedDataTypeL(KSigs[aIndex].iMime);
	}


//
//
//
TDataType CMMRUF::SupportedDataTypeL(const TText8* aMime) const
	{
	if (aMime == NULL)
		{
		User::Leave(KErrArgument);
		}

	return TDataType(TPtrC8(aMime));
	}


//
// Records the matched data type and the confidence.
//
void CMMRUF::SetDataTypeL(TInt aConfidence, const TText8* aMime)
	{
	iDataType = SupportedDataTypeL(aMime);
	iConfidence = aConfidence;
	}


//
// Utility function for matching file extensions.
// Returns 1 if the extensions match, 0 otherwise.
//
TUint8 CMMRUF::MatchExtension(const TDesC& aFileExt, const TText* aKnownExt) const
	{
	if (aFileExt.Length() > 0)
		{
		return ((aFileExt.MatchF(TPtrC(aKnownExt)) != KErrNotFound) ? 1 : 0);
		}

	return 0;
	}


//
// Utility function for matching known header data.
// Returns 1 if the buffer contents match the header, 0 otherwise.
//
TUint8 CMMRUF::MatchHeader(const TDesC8& aBuffer, const TDesC8& aHeader) const
	{
	if (aBuffer.Length() > 0)
		{
		if (aHeader.Length() <= aBuffer.Length())
			{
			return ((aBuffer.Match(aHeader) != KErrNotFound) ? 1 : 0);
			}
		}

	return 0;
	}


//
// This function is called by AppArc when something needs recognising.
//
void CMMRUF::DoRecognizeL(const TDesC& aFileName, const TDesC8& aBuffer)
	{
	TMatch match;
	RArray<TMatch> matches;
	CleanupClosePushL(matches);

	// If a handle to the file is available, AppArc prepends "::" to the
	// aFileName parameter for some reason. This causes TParse to fail
	// to get the file extension, so we have to fix it.
	TPtrC fName(aFileName);
	if (aFileName.Match(_L("::*")) == 0)
 		{
		fName.Set(aFileName.Mid(2));
 		}
 	
 	TParse parse;
	User::LeaveIfError(parse.Set(fName, NULL, NULL));
	const TPtrC& ext = parse.Ext();

	RFile* pFile = FilePassedByHandleL();
	CReader* pReader = (pFile ? CFileReader::NewLC(pFile) : CBufferReader::NewLC(aBuffer));
	
	// The main recognition loop.
	TBool certainMatch = EFalse;
	for (TInt i = 0; (i < KDataTypesCount) && !certainMatch; i++)
		{
		// Reset the results for each iteration.
		pReader->Reset();
		match.Reset();

		if (KSigs[i].iHeaderLen == 0)
			{
			// Call the custom recognition function.
			// match contains the results.
			__ASSERT_DEBUG(KSigs[i].iHeaderOrProc, User::Panic(KRUFPanic, KReasonNoFunction));
			TCustomProc proc = (TCustomProc)(KSigs[i].iHeaderOrProc);
			proc(ext, *pReader, match);
			}
		else
			{
			// This format has a known signature.
			TPtrC8 header(_S8(KSigs[i].iHeaderOrProc), KSigs[i].iHeaderLen);
			
			TUint8 sameHeader = MatchHeader(aBuffer, header);
			TUint8 sameExt = MatchExtension(ext, KSigs[i].iExt);
			TUint8 confIndex = MAKE_BYTE2(sameExt, sameHeader);

			match.iConfidence = KResultToConfidence[confIndex];
			match.iMime = KSigs[i].iMime; 
			}
			
		// Evaluate the result.
		switch (match.iConfidence)
			{
			case KConfNotRecognised:
				continue;
				
			case KConfCertain:
				certainMatch = ETrue;
				// Deliberate fall through to add match to array of matches.

			case KConfProbable:
				// Add it to the start of the array.
				User::LeaveIfError(matches.Insert(TMatch(match), 0));
				break;

			case KConfPossible:
				// Add it to the end of the array.
				if (matches.Count() == 0)
					{
					User::LeaveIfError(matches.Append(TMatch(match)));
					}
				break;
				
			default:
				// Should never get here.
				User::Leave(KErrArgument);
			}
		}	// for (TInt i = 0; i < KDataTypesCount; i++)

	// We've done looping.
	if (matches.Count() > 0)
		{
		match = matches[0];
		SetDataTypeL(match.iConfidence, match.iMime);
		}

	CleanupStack::PopAndDestroy(pReader);
	CleanupStack::PopAndDestroy(&matches);
	}




