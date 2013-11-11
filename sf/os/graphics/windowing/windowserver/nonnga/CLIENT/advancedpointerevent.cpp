// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains dummy implementations for exported methods implemented in NGA version
// of this file which share def files with WSERV NONNGA.
// 
//

#include <w32std.h>
#include "advancedpointerevent.inl"

EXPORT_C TInt TAdvancedPointerEvent::DoGetPointerNumber() const
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C TInt TAdvancedPointerEvent::DoGetProximity() const
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C TInt TAdvancedPointerEvent::DoGetPressure() const
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C TInt TAdvancedPointerEvent::DoGetProximityAndPressure() const
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	ASSERT(0);
	return 0;
	}

EXPORT_C void TWsEvent::InitAdvancedPointerEvent(TPointerEvent::TType /*aType*/, TUint /*aModifiers*/, const TPoint3D &/*aPoint3D*/, TUint8 /*aPointerNumber*/)
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{ 
	ASSERT(0);
	}

EXPORT_C void TWsEvent::SetPointerNumber(TUint8 /*aPointerNumber*/)
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	ASSERT(0);
	}

EXPORT_C void TWsEvent::SetPointerZ(TInt /*aZ*/)
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	ASSERT(0);
	}

EXPORT_C TAdvancedPointerEvent::TAdvancedPointerEvent(const TAdvancedPointerEvent& /*aPointerEvent*/) // Copy Constructor
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	ASSERT(0);
	}

EXPORT_C TAdvancedPointerEvent& TAdvancedPointerEvent::operator=(const TAdvancedPointerEvent& /*aPointerEvent*/) // Operator = overload
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	ASSERT(0);
	return *this;
	}
