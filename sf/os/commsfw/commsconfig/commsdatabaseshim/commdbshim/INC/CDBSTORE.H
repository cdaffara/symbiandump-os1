// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#ifndef __CDBSTORE_H__
#define __CDBSTORE_H__

#include <s32mem.h>
#include <cdbover.h>

class CStoreableOverrideSettings : public CCommDbOverrideSettings
/**
Creates override settings for the columns of specific tables, and stores and
retrieves the override sets to and from both streams and buffers.

The buffer is a convenient way of passing this information across thread or 
process boundaries.

The CCommDbOverrideSettings base class provides the member functions for setting 
and fetching override values. 
@deprecated
	Overrides are deprecated from v9.1
*/
	{
public:
	IMPORT_C static CStoreableOverrideSettings* NewL(TParamList aParamList);
	IMPORT_C ~CStoreableOverrideSettings();

	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void RestoreL(HBufC8* aBuf);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C CBufFlat* StoreLC();
	inline CBufStore* GetStore() const;

	IMPORT_C static CStoreableOverrideSettings* NewL(TParamList aParamList, TCommDbDatabaseType aDbType);

protected:
	CStoreableOverrideSettings(TParamList aParamList);
	virtual void ConstructL();
private:
	/** @deprecated in v7.0. There is no need for this store */
	CBufStore* iStore;
	};

#include <cdbstore.inl>

#endif
