// dobject_compat.cpp
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "dobject_compat.h"

extern TBool gRunningWithOldDefinition = ETrue;

NONSHARABLE_CLASS(DObjectWithPaddingOnly) : public DObject
	{
public:
	DOBJECT_PADDING;
	};

TBool CalculateDObjectSize()
	{
	DObjectWithPaddingOnly* obj = new DObjectWithPaddingOnly;
	if (!obj) return EFalse;
	
	// objectId points to the mem location where iObjectId will be. So if running on a system with the new size DOBject it will always be non-zero (because objectIds are set in the DObject constructor, and are always non-zero), but if running on earlier systems it will be zero because DBase zero-fills the object
	TUint64& objectId = *reinterpret_cast<TUint64*>((TUint8*)&obj->iName + sizeof(HBuf*));
	
	if (objectId != 0)
		{
		//Kern::Printf("Detected MemoryAccess is running with new larger DObject");
		gRunningWithOldDefinition = EFalse;
		}
	else
		{
		//Kern::Printf("Detected MemoryAccess is running with old-format DObject");		
		}
	obj->Close(NULL);
	return ETrue;
	}
