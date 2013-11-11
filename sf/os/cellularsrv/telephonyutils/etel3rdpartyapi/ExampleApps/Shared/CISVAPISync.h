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


#ifndef __CISVAPISYNC_H__
#define __CISVAPISYNC_H__

#include "CISVAPIBase.h"

/**
Class derived from MExecController, used when execution of an application
is predictable and the application will not use notifications in derived 
objects.
*/
class MExecSync : public MExecController
	{
	// Methods
public:
	/**
	Provides functionality for member objects to notify the Menu object
	(their owner) that they have completed their request.
	*/
	virtual void ExecComplete() = 0;

	};

/**
Extended class providing functionality for objects to make Etel 3rd party
function calls and interact with their owning class.
*/
class CISVAPISync : public CISVAPIBase
	{
	// Methods
public:
	CISVAPISync(MExecSync* aController
			   ,TTelISVExampleType aExampleType);
	/**
	Calls ExecComplete() through the iController instance.
	*/
	virtual void ExampleComplete();

	};

#endif // __CISVAPISYNC_H__
