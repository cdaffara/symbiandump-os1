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

#ifndef MMRUF_H
#define MMRUF_H

#include <e32cons.h>
#include <apmrec.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "constants.h"

//
// This class is used to store the results of a match.
// iConfidence - the confidence of the match
// iMime - the matched mime-type.
//
class TMatch
	{
public:
	TMatch()
	  : iConfidence(CApaDataRecognizerType::ENotRecognized),
	 	iMime(NULL)
		{
		}
		
	TMatch(TInt aConfidence, const TText8* aMime)
	  : iConfidence(aConfidence),
	    iMime(aMime)
		{
		}
	
	void Reset()
		{
		iConfidence = KConfNotRecognised;
		iMime = NULL;
		}
		
	TInt iConfidence;
	const TText8* iMime;
	};


// 
// Function prototype for custom format recogniser functions.
// Forward declaration of CReader required.
//
class CReader;
typedef void (*TCustomProc)(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch);


//
// These macros are used when creating the KSigs table.
// CUSTOM_SIG - this is to be used when a format cannot be identified
//              from a known header pattern. It takes the MIME-type it
//              tries to identify and a pointer to a function that
//              does the identification.
// HEADER_SIG - this is to be used when a format can be identified
//              from a known header pattern.
//
#define CUSTOM_SIG(mime, funcPtr)		{NULL, (mime), (TAny*)(funcPtr), 0}
#define HEADER_SIG(mime, ext, header) 	{(ext), (mime), (TAny*)_S8(header), sizeof(header) - 1}


//
// This structure tells the recogniser how to match a format.
// iExt - the standard file extension for the format.
// iMime - the defined MIME-type for the format.
// iHeaderOrProc - a pointer to either a header signature pattern
//                 (if iHeaderLen = 0) or a function that carries
//                 out in-depth parsing of the file (if iHeaderLen > 0)
// iHeaderLen - the length of the header signature in characters, or zero
//              if iHeaderOrProc is a pointer to a function.
//
typedef struct
	{
	const TText* iExt;
	const TText8* iMime;
	TAny* iHeaderOrProc;
	TInt iHeaderLen;
	}
TSignature;


// UIDs taken from Multimedia Allocation Table.
static const TInt KRecogniserUID = 0x1027381A;
static const TUid KMMRUFDLLUid = {0x102825F6};

// The amount of file data wanted from the AppArc framework.
static const TInt KPreferredBufSize = 256;

// The recogniser priority.
static const TInt KRecogniserPriority = CApaDataRecognizerType::ENormal;


//
// This class does the recognising.
//
class CMMRUF : public CApaDataRecognizerType
  	{
public:
	static CMMRUF* NewL();
	virtual ~CMMRUF();
	TDataType SupportedDataTypeL(TInt aIndex) const;
	TDataType SupportedDataTypeL(const TText8* aMime) const;
	TUint PreferredBufSize();

protected:
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	void SetDataTypeL(TInt aConfidence, const TText8* aMime);
	TUint8 MatchExtension(const TDesC& aFileName, const TText* aExt) const;
	TUint8 MatchHeader(const TDesC8& aBuffer, const TDesC8& aHeader) const;
	
private:
	CMMRUF();
	};

	
#endif
