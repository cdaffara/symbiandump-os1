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
// QoS profile change notifier
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef CQOSCHAGENOTIFIER_H
#define CQOSCHAGENOTIFIER_H

#include <e32base.h>

#include "ceteldrivernotifier.h"
#include "ceteldrivercontext.h" // definition of TQoSR99_R4NegotiatedPckg
#include "pdpfsmnmspace.h"


class RPacketQoS;

/** QoS profile change notifier */
class CQoSChangeNotifier : public CEtelDriverNotifier
	{
public:
	CQoSChangeNotifier (TContextId aId, RPacketQoS& aPacketQoS, 
							CPdpFsmInterface& aPdpFsmInterface);
	virtual ~CQoSChangeNotifier();

	virtual void Start();

protected:
	virtual void Notify(const TRequestStatus& aStatus);
	virtual void DoCancel();

private:
	/** associated context id */
	TContextId							iId;
	/** ref to packet QoS */
	RPacketQoS&							iPacketQoS;

    TNegotiatedProfileBuffer iProfileBuffer;
    TNegotiatedProfileBufferPckg iProfilePckg;
	};

#endif
// CQOSCHAGENOTIFIER_H
