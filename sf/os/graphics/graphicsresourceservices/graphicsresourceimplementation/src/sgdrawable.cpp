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
// Graphics Resource - drawable implementation
//

#include "sgdriver.h"

EXPORT_C TInt RSgDrawable::Open(TSgDrawableId aId, const TSgAttributeArrayBase* aAttributes)
	{
	if (iImpl)
		{
		return KErrInUse;
		}
	__ASSERT_ALWAYS(gPls.iDriver, Panic(ESgPanicNoDriver));
	return gPls.iDriver->FindAndOpenImage(aId, aAttributes, iImpl);
	}

EXPORT_C void RSgDrawable::Close()
	{
	if (iImpl)
		{
		__ASSERT_ALWAYS(gPls.iDriver, Panic(ESgPanicNoDriver));
		__ASSERT_ALWAYS(gPls.iDriver->CheckImage(iImpl), Panic(ESgPanicBadDrawableHandle));
		static_cast<XSgImage*>(iImpl)->Close();
		iImpl = NULL;
		}
	}

EXPORT_C TSgDrawableId RSgDrawable::Id() const
	{
	if (!iImpl)
		{
		return KSgNullDrawableId;
		}
	__ASSERT_ALWAYS(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_ALWAYS(gPls.iDriver->CheckImage(iImpl), Panic(ESgPanicBadDrawableHandle));
	return static_cast<XSgImage*>(iImpl)->Id();
	}

EXPORT_C TUid RSgDrawable::DrawableType() const
	{
	if (!iImpl)
		{
		return KNullUid;
		}
	__ASSERT_ALWAYS(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_ALWAYS(gPls.iDriver->CheckImage(iImpl), Panic(ESgPanicBadDrawableHandle));
	return static_cast<XSgImage*>(iImpl)->DrawableType();
	}

EXPORT_C TInt RSgDrawable::GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr) const
	{
	aInterfacePtr = NULL;
	if (!iImpl)
		{
		return KErrBadHandle;
		}
	__ASSERT_ALWAYS(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_ALWAYS(gPls.iDriver->CheckImage(iImpl), Panic(ESgPanicBadDrawableHandle));
	return static_cast<XSgImage*>(iImpl)->GetInterface(aInterfaceUid, aInterfacePtr);
	}
