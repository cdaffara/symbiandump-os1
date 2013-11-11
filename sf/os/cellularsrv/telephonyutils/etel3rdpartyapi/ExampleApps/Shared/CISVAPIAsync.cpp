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


#include "CISVAPIAsync.h"

/**
Default constructor.

@param aController  iController is set to point to this object
@param aExampleType The type of the derived class that has extended the
                    functionality of this class
*/
CISVAPIAsync::CISVAPIAsync(MExecAsync* aController,
						   TTelISVExampleType aExampleType)
	: CISVAPIBase(aController, aExampleType)
	{
	// Empty method
	}

/**
Calls MExecAsync::ExecComplete() to notify the Menu object that they have
completed their request.
*/
void CISVAPIAsync::ExampleComplete()
	{
	reinterpret_cast<MExecAsync*>
	(iController)->ExecComplete(GetExampleType());
	}

/**
Calls MExecAsync::ExecNotify() to notify the Menu object that they have been
notified of a change.
*/
void CISVAPIAsync::ExampleNotify()
	{
	reinterpret_cast<MExecAsync*>
	(iController)->ExecNotify(GetExampleType());
	}

