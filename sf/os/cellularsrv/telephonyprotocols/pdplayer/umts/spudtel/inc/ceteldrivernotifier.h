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
// service change notify
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef CETELDRIVERNOTIFIER_H
#define CETELDRIVERNOTIFIER_H

#include <e32base.h>

class CPdpFsmInterface;


/** base notifier class */
class CEtelDriverNotifier : public CActive
	{
public:
	CEtelDriverNotifier(CPdpFsmInterface& aPdpFsmInterface);

	/** starts notifications */
	virtual void Start() = 0;

protected:
	/** notifies SPUD FSM about network events */
	virtual void Notify(const TRequestStatus& aStatus) = 0;

	// CActive methods
	virtual void RunL();

protected:
	/** Pdp Fsm Interface for FSM notification */
	CPdpFsmInterface&	iPdpFsmInterface;
	};

#endif // CETELDRIVERNOTIFIER_H
