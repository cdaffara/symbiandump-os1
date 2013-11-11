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
 @test
 @internalComponent
*/

#include <iniparser.h>
#include "t_inidata.h"
#include "t_wservconsts.h"

EXPORT_C HBufC* CTIniData::ReadStrL(const TPtrC& aIniParam, const TDesC& aConfigFile)
	{
	CIniData * iniData = CIniData::NewL(aConfigFile);
	CleanupStack::PushL(iniData);
	
	TPtrC value;
	User::LeaveIfError(iniData->FindVar(KDefaultSectionName, aIniParam, value));
	HBufC* ret = value.Alloc();

	CleanupStack::PopAndDestroy(iniData);
	return ret;
	}

EXPORT_C TInt CTIniData::ReadIntL(const TPtrC& aIniParam, const TDesC& aConfigFile)
	{
	CIniData * iniData = CIniData::NewL(aConfigFile);
	CleanupStack::PushL(iniData);
	
	TInt ret;
	User::LeaveIfError(iniData->FindVar(KDefaultSectionName, aIniParam, ret));
	CleanupStack::PopAndDestroy(iniData);
	return ret;
	}
