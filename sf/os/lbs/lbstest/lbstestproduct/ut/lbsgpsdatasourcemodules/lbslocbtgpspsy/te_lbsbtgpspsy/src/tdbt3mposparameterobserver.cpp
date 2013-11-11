// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tt3MPosParameterObserver implementation
//



/**
 @file tdbt3mposparameterobserver.cpp
 @internalTechnology
*/

#include <lbs.h>
#include "tdbt3mposparameterobserver.h"

Tt3MPosParameterObserver::Tt3MPosParameterObserver(){}


TInt Tt3MPosParameterObserver::GetRequiredPositionQuality(TPositionQuality& /*aPositionQuality*/) const
    {
    return KErrNone;
    }

TInt Tt3MPosParameterObserver::QueryId() const
    {
    return 1234;
    }

void Tt3MPosParameterObserver::GetMaxAge(TTime& /*aMaxAge*/) const
    {
    }

TBool Tt3MPosParameterObserver::IsPartialUpdateAllowed() const
    {
    return EFalse;
    }

void Tt3MPosParameterObserver::GetUpdateTimeOut(TTimeIntervalMicroSeconds& /*aUpdateTimeOut*/) const
	{
	//Intentionally left blank
	}

void Tt3MPosParameterObserver::SetPsyDefaultUpdateTimeOut(const TTimeIntervalMicroSeconds& /*aUpdateTimeOut*/)
	{
	//Intentionally left blank
	}

void Tt3MPosParameterObserver::ExtendUpdateTimeOut(const TTimeIntervalMicroSeconds& /*aAdditionalTime*/)
	{
	//Intentionally left blank
	}

// End of file
