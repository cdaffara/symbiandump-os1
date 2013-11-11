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



#ifndef __CPPUNIT_ASSERT_FAILURE_H
#define __CPPUNIT_ASSERT_FAILURE_H

#include <e32base.h>

_LIT8(KCppUnitUnknownFilename, "-unknown-");
const TInt KCppUnitUnknownLineNumber=(-1);


class CAssertFailure : public CBase
	{
public:

	static CAssertFailure* NewL (const TDesC8& aMessage,
	                             TInt aLineNumber=KCppUnitUnknownLineNumber,
	                             const TDesC8& aFileName=KCppUnitUnknownFilename);

	static CAssertFailure* NewLC (const TDesC8& aMessage,
	                              TInt aLineNumber=KCppUnitUnknownLineNumber,
	                              const TDesC8& aFileName=KCppUnitUnknownFilename);

	static CAssertFailure* NewL (CAssertFailure& aAssertFailure);

	static CAssertFailure* NewLC (CAssertFailure& aAssertFailure);

    ~CAssertFailure ();

    const TDesC8& What() const;
    TInt LineNumber() const;
    const TDesC8& FileName() const;

	void SetMyHeapCellCount (TInt aHeapCellCount);
	TInt MyHeapCellCount ();

private:

    CAssertFailure ();
    CAssertFailure (TInt aLineNumber);
    void ConstructL (const TDesC8& aMessage, const TDesC8& aFileName);
    void ConstructL (CAssertFailure& aAssertFailure);

    HBufC8* iMessage;
    TInt  iLineNumber;
    HBufC8* iFileName;
	TInt iMyHeapCellCount;
	};

#endif

