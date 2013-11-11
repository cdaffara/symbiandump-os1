// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef FBSGLYPHMETRICSARRAY_H
#define FBSGLYPHMETRICSARRAY_H

#include <e32def.h>
#include <openfont.h>
#include <fbs.h>

/**
@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(RFbsGlyphMetricsArray)
	{
public:
	IMPORT_C RFbsGlyphMetricsArray();
	IMPORT_C TInt Get(CFbsFont& aFont, const TUint* aGlyphCodes, TInt aCount);
	IMPORT_C void Close();
	IMPORT_C TInt Count() const;
	IMPORT_C const TOpenFontCharMetrics& operator[](TInt aIndex) const;
	
private:
	RFbsGlyphMetricsArray(const RFbsGlyphMetricsArray&);
	const RFbsGlyphMetricsArray& operator =(const RFbsGlyphMetricsArray&);
	TInt SendRecvGlyphMetrics(CFbsFont& aFont, TUint* aArrayIndices, TInt aArrayIndicesCount, RArray<TOpenFontCharMetrics>* aMetrics) const;    
	
private:
	RArray<TOpenFontCharMetrics> iMetrics;
	const TUint* iGlyphCodes;
	TInt iCount;
	};

#endif /* FBSGLYPHMETRICSARRAY_H */
