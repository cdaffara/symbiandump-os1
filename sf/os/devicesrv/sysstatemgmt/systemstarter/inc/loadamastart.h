// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOADAMASTART_H__
#define __LOADAMASTART_H__

#include <ssm/ssmloadamastart.h>

class CAmaStart;

/**
Class to start After Market Applications (AMA).

@see CAmaStart

@internalTechnology
@released
*/
NONSHARABLE_CLASS (CLoadAmaStart) : public CSsmLoadAmaStart
	{
public:
	IMPORT_C static CLoadAmaStart* CreateL();

	// from CSsmLoadAmaStart
	void StartL(const TUid aDscId);
	void Start(const TUid aDscId, TRequestStatus& aRequestStatus);
	void StartCancel();

	~CLoadAmaStart();

private:
	void ConstructL();
	CLoadAmaStart();

private:
	CAmaStart* iAmaStart;
	};

#endif // __LOADAMASTART_H__
