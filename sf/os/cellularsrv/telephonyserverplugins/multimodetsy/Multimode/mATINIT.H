// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// GATINIT.H
// Not used at the moment 13:7:98
// Initialization of the GSM capabilites of the Mobile Equipment (ME)
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __GATININT_H__
#define __GATININT_H__

#include "atbase.h"
#include "atinit.h"

class CATBase;
class CGATInit : public CATBase
	{
public:
	static CGATInit* NewL(CATIO* aIo,CTelObject* aTelObject,CHayesStatus* aHayesStatus);
	~CGATInit();
protected:
	CGATInit(CATIO* aIo,CTelObject* aTelObject,CHayesStatus* aHayesStatus);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
private:
	};

#endif
