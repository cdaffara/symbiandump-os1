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
// Implementation for XParameterSetBase and XParameterSet.
// 
//
//

/**
 @file
 @internalTechnology
*/

#include <comms-infras/es_parameterset.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCSckCSPrmtS, "ESockCSckCSPrmtS");
#endif

EXPORT_REGISTER_TYPEID( XParameterSet, KParameterSetTypeUid, Meta::KNetMetaTypeAny )

EXPORT_C /*virtual*/ XParameterSet::~XParameterSet()
	{
	}

EXPORT_C /*virtual*/ void XParameterSet::UpdateWithL(const XParameterSet& aRhsBase)
	{
	// you need to implement this function for your specific data type
	//  so that unset/inappropriate fields in aRhsBase are not copied
	(void)aRhsBase;
	__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockCSckCSPrmtS, 1));
	}

EXPORT_C /*virtual*/ TInt XParameterSet::AppendPrintableTo(TDes& /*aDestination*/) const
    {
    // You need to implement this function for your specific data type
    //  to append your instance in some user friendly format to the passed in descriptor
    __ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockCSckCSPrmtS, 2));
    return KErrNotSupported;
    }

EXPORT_C /*virtual*/ TInt XParameterSet::_extension(TUid /*aExtensionId*/, TAny*& /*aReturnedExtension*/)
    {
    return KErrNotSupported;
    }


