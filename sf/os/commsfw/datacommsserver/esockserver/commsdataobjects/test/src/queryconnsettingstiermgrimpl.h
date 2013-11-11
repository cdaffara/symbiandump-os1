// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef QUERYCONNSETTINGSTIERMGRIMPL_H
#define QUERYCONNSETTINGSTIERMGRIMPL_H

#include <metadatabase.h>
#include <comms-infras/ss_commsdataobject.h>
#include <comms-infras/ss_tiermanagerutils.h>

using namespace ESock;

class CQueryConnSettingsTierMgrImpl : public CBase, public MQueryConnSettingsApiExt
	{
public:
	static CQueryConnSettingsTierMgrImpl* NewL(TUint aIap);
	
	virtual TInt GetInt(CommsDat::TMDBElementId aElementId, TUint32& aValue, MPlatsecApiExt* aPlatsecApiExt);
	virtual TInt GetInt(CommsDat::TMDBElementId aElementId, TInt& aValue, MPlatsecApiExt* aPlatsecApiExt);
	virtual TInt GetBool(CommsDat::TMDBElementId aElementId, TBool& aValue, MPlatsecApiExt* aPlatsecApiExt);
	virtual TInt GetText(CommsDat::TMDBElementId aElementId, HBufC8*& aValue, MPlatsecApiExt* aPlatsecApiExt);
	virtual TInt GetText(CommsDat::TMDBElementId aElementId, HBufC16*& aValue, MPlatsecApiExt* aPlatsecApiExt);

	~CQueryConnSettingsTierMgrImpl() 
		{
		delete iIapView;
		}

private:
	CQueryConnSettingsTierMgrImpl() 
		{
		}

private:
	CCommsDatIapView* iIapView;
	};


#endif
// QUERYCONNSETTINGSTIERMGRIMPL_H

