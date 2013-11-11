// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Base class to the various states required in Spud FSM. Each real state is derived from
// this base class. The bass class is never instanciated but it's base functionality is 
// used for default handling in the most part.
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef TPDPSTATE_H
#define TPDPSTATE_H

#include <networking/umtsnifcontrolif.h>
#include "eteldrivernmspace.h"
#include "reteldriverinput.h"
#include "pdpfsmnmspace.h"



class CPdpFsm;
//class REtelDriverInput;
class CPdpFsmFactory;

// parent class
class TPdpState
{
public:
	TPdpState();
	
	// used as a default error handler
	virtual TInt Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aParam);

	const TDesC * LogOperation (CPdpFsm& aFsm, TInt aOperation);	

protected:
	// access helpers for use in the derived class
	void EtelDriverInput (CPdpFsm& aFsm, EtelDriver::TEtelInput aOperation);
	void EtelDriverCancel (CPdpFsm& aFsm);
	void SpudManNotify (CPdpFsm& aFsm, TInt aNotification, TInt aParam);
	
public:
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)
	TBuf<100> iName;
#else
	TBuf<1> iName;
#endif		

protected:
	CPdpFsmFactory   * iPdpFsmFactory;

private:
	
};


#endif // TPDPSTATE_H
