// ltkhal.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef LTKHAL_H
#define LTKHAL_H

#include <e32base.h>

namespace LtkUtils
	{
	NONSHARABLE_CLASS(CHalAttribute) : public CBase
		{
	public:
		CHalAttribute(TInt aAttribute, TInt aDeviceNumber, TInt aValue, TInt aError, const TDesC& aAttributeName, HBufC* aDescription);
		IMPORT_C ~CHalAttribute();

		TInt iAttribute;
		TInt iDeviceNumber;
		TInt iValue;
		TInt iError; // If not KErrNone, iValue will not be valid
		const TDesC& iAttributeName;
		HBufC* iDescription;
		};

	IMPORT_C void GetHalInfoL(RPointerArray<CHalAttribute>& aAttributes);
	IMPORT_C CHalAttribute* GetHalInfoL(TInt aAttribute);
	IMPORT_C CHalAttribute* GetHalInfoL(TInt aDeviceNumber, TInt aAttribute);

	IMPORT_C CHalAttribute* GetHalInfoForValueL(TInt aDeviceNumber, TInt aAttribute, TInt aValue);
	}

#endif
