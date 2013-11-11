// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mda\Client\Recognizer.H
// 
//

#ifndef __MDA_CLIENT_RECOGNIZER_H__
#define __MDA_CLIENT_RECOGNIZER_H__

#include <e32base.h>

#include <mda/client/resource.h>

/**
 * @publishedAll
 * @deprecated
 */
enum { KMdaResourceEntryParserNotImplemented = 0 };


class TMdaResourceEntryParser; // declared here.


NONSHARABLE_CLASS( TMdaResourceEntryParser )
/**
 * @internalTechnology
 */
	{
public:
	enum TType
		{
		EConfigDef,
		ESignature,
		ECapabilities,
		EExtension,
		EStrings
		};
public:
	IMPORT_C void Set(const TDesC8& aResource);
	IMPORT_C TUint Count(TType aType) const;
	IMPORT_C const TPtrC8 Entry(TType aType, TInt aIndex) const;
private:
	TPtrC8 iPtr;
	TUint iIndices[EStrings+1];
	TUint iCounts[EStrings+1];
	};

#endif
