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
// PDP status change notifier
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef CPDPSTATUSCHAGENOTIFIER_H
#define CPDPSTATUSCHAGENOTIFIER_H

#include <e32base.h>
#include <etelpckt.h>

#include <networking/umtsnifcontrolif.h>
#include "ceteldrivernotifier.h"

/** notifier for status of packet context */
class CPdpStatusChangeNotifier : public CEtelDriverNotifier
	{
public:
	CPdpStatusChangeNotifier(TContextId aId, RPacketContext& aPacketContext, 
								CPdpFsmInterface& aPdpFsmInterface);
	virtual ~CPdpStatusChangeNotifier();
	
	virtual void Start();

protected:
	virtual void Notify(const TRequestStatus& aStatus);
	virtual void DoCancel();

private:
	/** context id */
	TContextId						iId;
	/** underlying packet context */
	RPacketContext&					iPacketContext;

	/** status of a packet context */
	RPacketContext::TContextStatus	iContextStatus;
	};
		
/** notifier for status of packet context */
class CMbmsPdpStatusChangeNotifier : public CEtelDriverNotifier
	{
public:
	CMbmsPdpStatusChangeNotifier(TContextId aId, RPacketMbmsContext& aMbmsPacketContext, 
								CPdpFsmInterface& aPdpFsmInterface);
	virtual ~CMbmsPdpStatusChangeNotifier();
	
	
	virtual void Start();

protected:
	virtual void Notify(const TRequestStatus& aStatus);
	virtual void DoCancel();

private:
	/** context id */
	TContextId						iId;
	/** underlying mbms packet context */
	RPacketMbmsContext&					iMbmsPacketContext;
	/** status of a packet context */
	RPacketContext::TContextStatus	iContextStatus;
	};




#endif // CPDPSTATUSCHAGENOTIFIER_H
