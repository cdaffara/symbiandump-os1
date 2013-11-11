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

#ifndef __SSMSWP_H__
#define __SSMSWP_H__

#include <e32std.h>
#include <e32def.h>

/**
 @publishedPartner
 @released
 */
NONSHARABLE_CLASS(TSsmSwp)
	{
public:
	IMPORT_C TSsmSwp(TUint aKey, TInt32 aValue);
	IMPORT_C TSsmSwp(const TSsmSwp& aSwp);
	IMPORT_C void Set(TUint aKey, TInt32 aValue);
	IMPORT_C TUint Key() const;
	IMPORT_C TInt32  Value() const;

	IMPORT_C TSsmSwp& operator=(const TSsmSwp& aSwp);
	IMPORT_C TBool operator==(const TSsmSwp& aSwp) const;
	IMPORT_C TBool operator!=(const TSsmSwp& aSwp) const;

private:
	TSsmSwp();
	
private:
	TUint iKey;
	TInt32  iValue;
	};

#endif
