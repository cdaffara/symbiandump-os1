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

#ifndef CSERVICECHAGENOTIFIER_H
#define CSERVICECHAGENOTIFIER_H

#include <e32base.h>
#include <etelpckt.h>

#include "ceteldrivernotifier.h"

/** notifier for status of packet service */
class CServiceChangeNotifier : public CEtelDriverNotifier
	{
public:
	CServiceChangeNotifier(RPacketService& aPacketService, 
							CPdpFsmInterface& aPdpFsmInterface);
	~CServiceChangeNotifier();
	
	virtual void Start();

protected:
	virtual void Notify(const TRequestStatus& aStatus);
	virtual void DoCancel();

private:
	/** underlying packet service */
	RPacketService&				iPacketService;

	/** status of a packet service */
	RPacketService::TStatus		iServiceStatus;
	};

#endif // CSERVICECHAGENOTIFY_H
