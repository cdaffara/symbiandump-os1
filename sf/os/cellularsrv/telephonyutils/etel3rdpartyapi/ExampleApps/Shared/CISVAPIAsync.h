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


#ifndef __CISVAPIASYNC_H__
#define __CISVAPIASYNC_H__

#include "CISVAPIBase.h"

/**
Extended interface providing functionality for a member active object to
interact with its owning class. Execution is not predicable and so uses
notifications to alert applications when state changes occur.
*/
class MExecAsync : public MExecController
	{
public:
	/**
	Provides functionality for member objects to notify the Menu object (their
	owner) that they have completed their request.
	*/
	virtual void ExecComplete(TTelISVExampleType aDerivedType) = 0;

	/**
	Provides functionality for member objects to notify the Menu object	(their
	owner) that they have been notified of a change.
	*/
	virtual void ExecNotify(TTelISVExampleType aDerivedType) = 0;
	};

/**
Extended class providing functionality for objects to make Etel 3rd party
function calls and interact with their owning class.
*/
class CISVAPIAsync : public CISVAPIBase
	{
public:
	CISVAPIAsync(MExecAsync* aController,
			     TTelISVExampleType aExampleType);

	/**
	Calls ExecComplete() through the iController instance.
	*/
	void ExampleComplete();

	/**
	Calls ExecNotify() through the iController instance.
	*/
	void ExampleNotify();

protected:
	/**
	Specifies if a completing request is a notification or not.
	*/
	TBool iRequestNotify;

	};

#endif // __CISVAPIASYNC_H__
