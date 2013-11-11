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
//

/**
 @file 
 @internalTechnology
*/

#ifndef __E_METADATA_INL__
#define __E_METADATA_INL__

namespace Meta
{


STypeId STypeId::CreateSTypeId()
/**
 * Constructor
 */
	{
	STypeId id;
	id.iUid.iUid = 0;
	id.iType = 0;
	return id;
	}

STypeId STypeId::CreateSTypeId(TUint32 aUid, TUint32 aTypeId)
/**
 * Constructor
 */
	{
	STypeId id;
	id.iUid.iUid = aUid;
	id.iType = aTypeId;
	return id;
	}

}	// namespace Meta
#endif	// __E_METADATA_INL__

