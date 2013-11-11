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
//

#include <e32std.h>
#include <f32file.h>
#include <s32strm.h>
#include <barsread.h>
#include "Mda/Client/Recognizer.h"

#include "CompatAids.h"

#ifdef __PANIC_COMPAT_IS_ACTIVE
_LIT(KMdaResourceEntryParser, "TMdaResourceEntryParser");	
#endif // __PANIC_COMPAT_IS_ACTIVE


// *****************************************************************
// TMdaResourceEntryParser
// *****************************************************************

EXPORT_C void TMdaResourceEntryParser::Set(const TDesC8& /*aResource*/)
	{
	__PANIC_COMPAT(User::Panic(KMdaResourceEntryParser, KMdaResourceEntryParserNotImplemented));
	}

EXPORT_C TUint TMdaResourceEntryParser::Count(TType /*aType*/) const
	{
	__PANIC_COMPAT(User::Panic(KMdaResourceEntryParser, KMdaResourceEntryParserNotImplemented));
	return 0;
	}

EXPORT_C const TPtrC8 TMdaResourceEntryParser::Entry(TType /*aType*/, TInt /*aIndex*/) const
	{
	__PANIC_COMPAT(User::Panic(KMdaResourceEntryParser, KMdaResourceEntryParserNotImplemented));
	return TPtrC8(KNullDesC8);
	}

