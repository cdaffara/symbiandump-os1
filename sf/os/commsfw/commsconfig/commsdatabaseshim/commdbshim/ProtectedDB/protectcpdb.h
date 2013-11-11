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
// Protected tables/records functionality
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#ifndef PROTECTCONNECTPREF_H
#define	PROTECTCONNECTPREF_H

#include <commdb.h>
#include <cdbpreftable.h>
#include <metadatabase.h>

using namespace CommsDat;

class CCommsDbProtectConnectPrefTableView : public CCommsDbConnectionPrefTableView
/**
@internalTechnology
*/
	{
public:
	static CCommsDbProtectConnectPrefTableView* NewL(CCommsDatabase& aDb,const TDbQuery& aQuery);
	IMPORT_C ~CCommsDbProtectConnectPrefTableView();
	IMPORT_C	TInt ProtectTable();
	IMPORT_C	TInt UnprotectTable();
	IMPORT_C	TInt ProtectRecord();
	IMPORT_C	TInt UnprotectRecord();
	IMPORT_C	TInt GetTableAccess(RDbRowSet::TAccess& aAccessType);
	IMPORT_C	TInt GetRecordAccess(TInt& aAccess);

protected:
	CCommsDbProtectConnectPrefTableView(CCommsDatabase& aDb);

private:
	void DoProtectionL(CMDBSession* aSession, CMDBElement* aElement, TBool aProtect);

	void ConstructL(const TDbQuery& aQuery);
	};

#endif
