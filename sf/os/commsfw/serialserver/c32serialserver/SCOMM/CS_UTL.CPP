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


/** @file
 *
 * Implements the startup code for C32 server
 */

#include "CS_STD.H"
#include <e32svr.h>
#include "C32LOG.H"

_LIT(KC32FaultReason, "C32-fault");

GLDEF_C void Fault(TECommFault aFault, TRefByValue<const TDesC8> aFmt, ...)
/**
 Panic the server
 If the supplied aFmt defaults to a blank string, we don't check here since
 we can't actually evaluate the TRefByValue string here.
 If using Fault in __ASSERT_ALWAYS or other release code, make sure not to provide log strings
 since these will use up ROM space.
 (For a time we did check for blank strings instead inside the printf, but this became
 too hard once we started writing worker id too - so now on fault there will come a blank line
 in many cases -  not such a big deal)
 
 * @param aFault Fault code as defined in cs_std.h
 */
	{
#ifdef __FLOG_ACTIVE
	VA_LIST list;
	VA_START(list,aFmt);
	C32_STATIC_LOG2(KC32Warning,aFmt,list);
	VA_END (list);
#else
	(void)aFmt;
#endif
	User::Panic(KC32FaultReason, aFault);
	}
	
	
GLDEF_C void Fault(TECommFault aFault, TRefByValue<const TDesC16> aFmt, ...)
/**
 Panic the server
 If the supplied aFmt defaults to a blank string, we don't check here since
 we can't actually evaluate the TRefByValue string here.
 If using Fault in __ASSERT_ALWAYS or other release code, make sure not to provide log strings
 since these will use up ROM space.
 (For a time we did check for blank strings instead inside the printf, but this became
 too hard once we started writing worker id too - so now on fault there will come a blank line
 in many cases -  not such a big deal)
 * @param aFault Fault code as defined in cs_std.h
 */
	{
#ifdef __FLOG_ACTIVE
	VA_LIST list;
	VA_START(list,aFmt);
	C32_STATIC_LOG2(KC32Warning,aFmt,list);
	VA_END (list);
#else
	(void)aFmt;
#endif
	User::Panic(KC32FaultReason, aFault);
	}

// EOF - CS_UTL.CPP
