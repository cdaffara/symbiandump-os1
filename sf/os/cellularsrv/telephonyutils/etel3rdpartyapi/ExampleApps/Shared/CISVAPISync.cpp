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

#include "CISVAPISync.h"

/**
Constructor.

@param aController  iController is set to point to this object
@param aExampleType The type of the derived class that has extended the
                    functionality of this class
*/
CISVAPISync::CISVAPISync(MExecSync* aController, TTelISVExampleType aExampleType)
	: CISVAPIBase(aController, aExampleType)
	{
	// Empty method
	}

/**
Calls ExecComplete() through the iController instance.
*/
void CISVAPISync::ExampleComplete()
	{
	reinterpret_cast<MExecSync*> (iController)->ExecComplete();
	}
