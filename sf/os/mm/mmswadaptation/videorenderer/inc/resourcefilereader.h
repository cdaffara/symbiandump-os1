// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef RESOURCEFILEREADER_H
#define RESOURCEFILEREADER_H

#include <e32std.h>
#include <e32base.h>
#include <barsc.h>

// class declaration
class TUncompressedVideoFormat;


/** This class is responsible for reading the supplied resource file */
NONSHARABLE_CLASS(CResourceFileReader) : public CBase
	{
public:
	static CResourceFileReader* NewL(const TDesC& aResourceFile);
	static CResourceFileReader* NewLC(const TDesC& aResourceFile);
	~CResourceFileReader();
	
	void ReadSupportedFormatL(RArray<TUncompressedVideoFormat>& aArray);
	void ReadTimerInfoL(TInt64& aDefaultDelay, TInt64& aMaxDelay);

private:
	CResourceFileReader();
	void ConstructL(const TDesC& aResourceFile);

private:
	RFs iFs;
	RResourceFile iResourceFile;
	};

#endif // RESOURCEFILEREADER_H

