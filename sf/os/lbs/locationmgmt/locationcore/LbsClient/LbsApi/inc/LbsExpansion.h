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
// LbsModStatus.h
// 
//

/**
 @file
 @internalComponent 
*/

class CModStatusHandler;

#ifndef __LBSEXPANSION_H__
#define __LBSEXPANSION_H__

#include <e32base.h>
#include "LbsInternalInterface.h"

class MModuleStatusObserver
	{
public:
	virtual void ModuleStatusChanged() = 0;
	};


class CExpansion : public CBase, public MModuleStatusObserver
	{
public:
	static CExpansion* NewL();
	
	~CExpansion();

protected:
	CExpansion(); 
	void ConstructL();

private:
	// private data
	
public:
	// From MModuleStatusObserver
	virtual void ModuleStatusChanged();	

	CModStatusHandler*	iModStatusHandlerGpsManager;
	CModStatusHandler*	iModStatusHandlerNetLocManager;

	};

#endif //__LBSEXPANSION_H__
