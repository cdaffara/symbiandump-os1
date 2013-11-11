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

#ifndef __DSCSTORE_H__
#define __DSCSTORE_H__

#include <e32base.h>
#include <dscstoredefs.h>
#include <startupproperties.h>

class CDscItem;
class CDscDatabase;

/** 
Represents a DSC client interface to manipulate the DSC(Dynamic Start-up Configuration) database. A thread uses this class to
set up a connection with the DSC database. 

Is used to store DSC items that contain the information needed to launch After Market Application (AMA).

Multiple DSCs can be stored in the DSC database identified by different DSC IDs. A default DSC Uid, KDefaultSymbianDsc,
is provided.

All items within a DSC are unique. A duplicate item is defined as having the same filename and
arguments as another already in that particular DSC.

Uses transaction mechanism provided by the data source to ensure that DSC items will be added in a consistent manner 
to maintain data integrity. 

Requires PlatSec capabilities to add, delete DSC, and to read, add, update, or remove the items in a DSC. 

@publishedPartner
@released
*/
NONSHARABLE_CLASS (RDscStore)
	{

public:
	IMPORT_C	RDscStore();
	
	IMPORT_C    void OpenL();
	IMPORT_C	void Close();
	IMPORT_C	TBool IsOpened() const;
	
	IMPORT_C	void CreateDscL(const TUid& aDscId, const TDesC& aDescription);
	IMPORT_C	void CreateDscL();
	IMPORT_C    void DeleteDscL(const TUid& aDscId);
	IMPORT_C    TBool DscExistsL(const TUid& aDscId) const;
	IMPORT_C    TBool DscExistsL() const;
	IMPORT_C	void GetDscDescriptionL(const TUid &aDscId, TDes& aDescription) const;
	IMPORT_C 	void AddItemL(CDscItem& aItem,TDscPosition aPos);
	IMPORT_C 	void AddItemL(CDscItem& aItem);
	IMPORT_C	TBool ItemExistsL( const CDscItem& aItem) const;
	IMPORT_C	void EnumOpenLC(const TUid& aDscId);
	IMPORT_C	void EnumOpenLC();
	IMPORT_C	CDscItem* EnumReadNextL();
	IMPORT_C	void EnumClose();
	IMPORT_C 	void ReadItemL(CDscItem& aItem);
	IMPORT_C	void UpdateItemL(const CDscItem& aItem);
	IMPORT_C	void DeleteItemL(const CDscItem& aItem);
	IMPORT_C	void GetMaxStringLength( TInt& aStringLength ) const;

private:
	// declare copy constructor and assign operator as private to avoid copy
	RDscStore(const RDscStore&);
	RDscStore& operator=(const RDscStore&);
	void CheckDbIsOpenL() const;

private:
	CDscDatabase* iDscDatabase;
	TAny * iSpare;
	};
	
#endif
