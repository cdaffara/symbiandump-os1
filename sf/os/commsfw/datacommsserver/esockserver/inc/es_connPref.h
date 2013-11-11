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
 @publishedPartner
 @released
*/

#if !(defined __SS_CONNPREF_H__)
#define __SS_CONNPREF_H__

#define TCommIdList TConnIdList
#include <connpref.h>

class TConnIdList : public TConnPref
/**
Class used to carry list of arbitrarry Ids stored as integer values.
Class doesn't allow remove only append and overwrite
@publishedPartner
@released since v9.1
*/
	{
public:
	IMPORT_C TConnIdList();

	IMPORT_C TInt& operator[](TInt aIndex);
	IMPORT_C TInt Get(TInt aIndex) const;
	IMPORT_C void Append(TInt aId);
	IMPORT_C TInt Count() const;
	IMPORT_C static TInt MaxCount();

protected:
	TUint16& CountRef();
	TInt* IdList();
	};

inline TInt* TConnIdList::IdList()
	{
	return reinterpret_cast<TInt*>(UserPtr());
	}

inline TUint16& TConnIdList::CountRef()
	{
	return reinterpret_cast<TUint16&>(reinterpret_cast<SConnPref*>(iBuf)->iSpare);
	}

#endif // __SS_CONNPREF_H__

