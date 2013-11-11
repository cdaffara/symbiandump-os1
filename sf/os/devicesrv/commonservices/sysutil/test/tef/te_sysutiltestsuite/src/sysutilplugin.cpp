// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// file sysutilplugin.cpp
// 
//

#include <e32base.h>
#include <e32std.h>
#include <e32test.h>
#include <bafl/sysutilplugindef.h>
#include "sysutilconstants.h"

EXPORT_C void GetDeviceAttributesAsArraysL( RArray<TInt32>& aKeys, CDesC16Array& aValues )
	{
	RDebug::Printf("In sysutilplugin.dll");
	_LIT16(KManufacturerNameValue, "Manufacturer");
	TPtrC16 value(KManufacturerNameValue);
	aKeys.Append(KManufacturerNameUid.iUid);
	aValues.AppendL(value);
	
	_LIT16(KModelNameValue, "Model");
	value.Set(KModelNameValue);
	aKeys.Append(KModelNameUid.iUid);
	aValues.AppendL(value);

    _LIT16(KModelCodeValue, "Symbian");
    value.Set(KModelCodeValue);
    aKeys.Append(KModelCodeUid.iUid);
    aValues.AppendL(value);	
	
	_LIT16(KRevisionIDValue, "001.234");
	value.Set(KRevisionIDValue);
	aKeys.Append(KRevisionIDUid.iUid);
	aValues.AppendL(value);
	
	_LIT16(KUIPlatformVersionMajorValue, "654");
	value.Set(KUIPlatformVersionMajorValue);
	aKeys.Append(KUIPlatformVersionMajorUid.iUid);
	aValues.AppendL(value);
	
	_LIT16(KUIPlatformVersionMinorValue, "987");
	value.Set(KUIPlatformVersionMinorValue);
	aKeys.Append(KUIPlatformVersionMinorUid.iUid);
	aValues.AppendL(value);
	
	_LIT16(KUIPlatformVersionValue, "654.987");
	value.Set(KUIPlatformVersionValue);
	aKeys.Append(KUIPlatformVersionUid.iUid);
	aValues.AppendL(value);
	
	_LIT16(KOSVersionMajorValue, "9");
	value.Set(KOSVersionMajorValue);
	aKeys.Append(KOSVersionMajorUid.iUid);
	aValues.AppendL(value);
	
	_LIT16(KOSVersionMinorValue, "5");
	value.Set(KOSVersionMinorValue);
	aKeys.Append(KOSVersionMinorUid.iUid);
	aValues.AppendL(value);
	
	_LIT16(KOSVersionValue, "9.5");
	value.Set(KOSVersionValue);
	aKeys.Append(KOSVersionUid.iUid);
	aValues.AppendL(value);
	}
