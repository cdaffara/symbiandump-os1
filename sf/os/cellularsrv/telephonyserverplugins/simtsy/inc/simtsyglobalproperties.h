// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declares public publish and subscribe categories and keys. 
// 
//

/**
 @file
*/
#ifndef __SIMTSYGLOBALPROPERTIES_H__
#define __SIMTSYGLOBALPROPERTIES_H__

#include <e32property.h>

/**
@deprecated

Not used.
**/
const TInt KUidSimtsyCategoryValueLocal = 0x0;
/**
@deprecated

Use Publish & Subscribe KUidPSSimTsyCategory instead.
**/
const TUid KUidSimtsyCategoryLocal = {KUidSimtsyCategoryValueLocal};

/**
@deprecated

Use Publish & Subscribe KPSSimTsyTestNumberValue instead.
**/
const TInt KSimtsyTestNumberKeyLocal = 0;
/**
@deprecated

Use Publish & Subscribe KPSSimTsyTestNumberKeyType instead.
**/
const RProperty::TType KSimtsyTestNumberKeyTypeLocal = RProperty::EInt;
/**
@deprecated

Use Publish & Subscribe KPSSimTsyRegStatChangeValue instead.
**/
const TInt KSimtsyRegStatChange = 1;
/**
@deprecated

Use Publish & Subscribe KPSSimTsyRegStatChangeKeyType instead.
**/
const RProperty::TType KSimtsyRegStatChangeType = RProperty::EInt;
/**
@deprecated

Use Publish & Subscribe KPSSimTsySignalStrengthChangeValue instead.
**/
const TInt KSimtsySignalStrengthChange = 2;
/**
@deprecated

Use Publish & Subscribe KPSSimTsySignalStrengthChangeKeyType instead.
**/
const RProperty::TType KSimtsySignalStrengthChangeType = RProperty::EInt;

#endif // __SIMTSYGLOBALPROPERTIES_H__
