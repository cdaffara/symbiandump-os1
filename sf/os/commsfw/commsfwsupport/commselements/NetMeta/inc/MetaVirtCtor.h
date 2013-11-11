// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined METAVIRTCTOR_H)
#define METAVIRTCTOR_H

#include <elements/vc.h>
#include <elements/metadata.h>

namespace Meta
{

/** Base class for objects constructed in explicitly managed memory by the virtual ctor. Explicit management means
    not the default heap, ie the caller has the responsibility of providing enough memory and maintaining its existence
	until after the object has been destroyed. Generally this will be a stack-based buffer.
*/

class SMetaDataNetCtor : public Meta::SMetaData		// in-place
	{
public:
	IMPORT_C TAny* operator new(TUint aSize, TDes8& aBuff);
	// No "operator delete"; in-place by design
	};


class CMetaDataVirtualCtorInPlace : public VC::CVirtualCtorInPlace
	{
public:
	//Constructs only from the type
	IMPORT_C SMetaDataNetCtor* New(const Meta::STypeId& aType, TDes8& aBuff) const;
	
	//Constructs from the type and loads from the buffer
	IMPORT_C SMetaDataNetCtor* New(const Meta::STypeId& aType, TPtrC8& aParams, TDes8& aBuff) const;
	
	//Constructs and loads from the buffer
	IMPORT_C SMetaDataNetCtor* New(TPtrC8& aParams, TDes8& aBuff) const;

	};

} //namespace Meta


#endif //METAVIRTCTOR_H


