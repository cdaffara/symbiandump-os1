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
 @internalTechnology
*/

#include "factory_log.h"


namespace Factories
{

void Logging::Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, ...)
    {
	VA_LIST list;
	VA_START(list,aFmt);
	Printf(aSubTag, Logging::ELogInfo, aFmt, list);
	VA_END(list);
    }

    
void Logging::Printf(const TDesC8& aSubTag, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
    {
    Printf(aSubTag, Logging::ELogInfo, aFmt, aList);
    }

void Logging::Printf(const TDesC8& aSubTag, TLogEntryType aType, TRefByValue<const TDesC8> aFmt, ...)
    {
	VA_LIST list;
	VA_START(list,aFmt);
	Printf(aSubTag, aType, aFmt, list);
	VA_END(list);
    }

    
void Logging::Printf(const TDesC8& /*aSubTag*/, TLogEntryType aType, TRefByValue<const TDesC8> aFmt, VA_LIST& aList)
    {
    TBuf8<250> buf;
	TLogIgnoreOverflow8 overflowHandler;
	buf.AppendFormatList(aFmt, aList, &overflowHandler);
	UTracePfAny(KPrimaryFilter, KText, ETrue, EFalse, aType, buf.Ptr(), buf.Size());
    }

}
// namespace Factories


