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
// This file defines TGsmSmsSlot class.
// 
//

/**
 @file
 @internalAll
*/


#ifndef __GSMUSMSSLOT_H__
#define __GSMUSMSSLOT_H__

#include <etelmm.h>

/**
 *  @internalComponent
 */
class TGsmSmsSlot : public RMobileSmsStore::TMobileGsmSmsEntryV1
	{
public:
	IMPORT_C TGsmSmsSlot();
	IMPORT_C TGsmSmsSlot(const RMobileSmsStore::TMobileGsmSmsEntryV1& aSmsEntry);

public:
	RMobilePhone::TMobileName iStore; //< Name of the store where slot is
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	};

#endif // __GSMUSMSSLOT_H__
