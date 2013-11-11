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
// file sysutilplugin_test.cpp
// 
//

#include <e32base.h>
#include <e32std.h>
#include <e32test.h>
#include <bafl/sysutilplugindef.h>
#include "sysutilconstants.h"

EXPORT_C void GetDeviceAttributesAsArraysL( RArray<TInt32>& aKeys, CDesC16Array& aValues )
	{
	RDebug::Printf("In sysutilplugin_test.dll");
	_LIT16(KManufacturerNameValue, "MyManufacturerName");
	TPtrC16 value(KManufacturerNameValue);
	aKeys.Append(KManufacturerNameUid.iUid);
	aValues.AppendL(value);
	
	_LIT16(KRevisionIDValue, "MyRevisionID");
	value.Set(KRevisionIDValue);
	aKeys.Append(KRevisionIDUid.iUid);
	aValues.AppendL(value);
	
	_LIT16(KUIPlatformVersionMajorValue, "100");
	value.Set(KUIPlatformVersionMajorValue);
	aKeys.Append(KUIPlatformVersionMajorUid.iUid);
	aValues.AppendL(value);
	
	_LIT16(KUIPlatformVersionValue, "100.3");
	value.Set(KUIPlatformVersionValue);
	aKeys.Append(KUIPlatformVersionUid.iUid);
	aValues.AppendL(value);
	
	_LIT16(KDeviceCreatorAttributeValue, "MyDeviceCreatorAttribute");
	value.Set(KDeviceCreatorAttributeValue);
	aKeys.Append(KDeviceCreatorAttributeUid.iUid);
	aValues.AppendL(value);
	}
