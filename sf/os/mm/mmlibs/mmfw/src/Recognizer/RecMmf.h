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

#ifndef __MMF_CLIENT_RECOGNIZER_H__
#define __MMF_CLIENT_RECOGNIZER_H__

#include <e32base.h>
#include <apmrec.h>
#include <apmstd.h>
#include "mmfcontrollerpluginresolver.h"

const int KRecMmfDllUidValue = 0x101f7c0c;

typedef	TBuf8<KMaxDataTypeLength> TMimeDataType;

/**
 * @internalComponent
 *
 * Class owned by the CApaMmfRecognizer recognizer class.
 * maintains a list of MIME-types supported and
 * an instance of the CMmfRecognizerUtil class which contains
 * ECOM-derived information about the header data to match against.
 * 
 */
class CMmfRecognizer : public CBase
	{
public:
	enum TMatchMethod
		{
		ENotMatched,
		EByName,
		EBySignature
		};
public:
	static CMmfRecognizer* NewL();
	virtual ~CMmfRecognizer();

	void BuildListL();

	TMatchMethod MatchL(const TDesC& aFileName, const TDesC8& aBuffer);
	
	const TDesC8& MimeString() const;

	const TPtrC8 SupportedDataTypeL(TInt aIndex) const;
	TInt NumMimeTypes() const;

private:
	CMmfRecognizer();
	void ConstructL();

private:
	CDesC8Array* iMimeTypes;				// owned
	TBuf8<KMaxDataTypeLength> iMimeString;
	CMmfRecognizerUtil* iMmfRecognizerUtil;	// owned 
	};

/**
 * @internalAll
 *
 * Concrete implementation of the recognizer API
 * Used to recognize non-native files supported by the
 * Multimedia framework. e.g. .WAV files, .AU files etc
 */
class CApaMmfRecognizer : public CApaDataRecognizerType 
	{
public: 
	static CApaMmfRecognizer* NewL();	
	~CApaMmfRecognizer();

	TDataType SupportedDataTypeL(TInt aIndex) const;
	TUint PreferredBufSize();	
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);

private: 
	CApaMmfRecognizer();
	void ConstructL();

private:
	CMmfRecognizer* iMmfRecognizer;
	};


#endif
