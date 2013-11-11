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
//

#include <e32base.h>

#ifndef __FILENAME_H__
#define __FILENAME_H__

/**
 *
 * Heap based Filename
 *
 * Simply a CBase wrapper for TFilename
 *
 * @xxxx
 *
 */
class CFileName : public CBase
	{

public:
	CFileName() {};

	static CFileName* NewL();
	static CFileName* NewLC();

public:
	TFileName& operator=(const TPtrC& aPtr);
	TFileName& operator=(const TText* aString);
	TFileName& operator=(const TDesC& aDes);
	TFileName& operator=(const TFileName& aBuf);
	TFileName& operator=(const CFileName& aFilename);

	void Copy(const TDesC8 &aDes);
	void Copy(const TDesC16 &aDes);
	void Copy(const TUint16 *aBuf,TInt aLength);
	void Copy(const TUint16 *aString);

	TInt Locate(TChar aChar) const;

	TPtrC16 Left(TInt aLength) const;

public:
	TFileName FileName() const;


private:
	TFileName iBuf;
	};

#endif //__FILENAME_H__
