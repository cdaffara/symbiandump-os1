/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef __LBSTESTLOCMONITORSESSION_H__
#define __LBSTESTLOCMONITORSESSION_H__

#include <e32base.h>
#include <lbscommon.h>
#include <lbsipc.h>
#include "lbstestlocmonitorserver.h"

class CTe_LocMonitorServer; // TODO ??? required when header included

class CTe_LocMonitorServerSession : public CSession2
	{
public:
	CTe_LocMonitorServerSession();
	~CTe_LocMonitorServerSession();
	virtual void ServiceL(const RMessage2& aMessage);
	virtual void CreateL();
private:
	// Helpers
	void DispatchL(const RMessage2& aMessage);
	inline CTe_LocMonitorServer& Server();

private:
	// Services available 
	void NewPositionerL(const RMessage2& aMessage);
	void ClosePositionerL(const RMessage2& aMessage);

private:
	CObjectIx* iObjectIx;
	CObjectConIx* iContainerIx;
	CObjectCon* iObjectCon;

	};

inline CTe_LocMonitorServer& CTe_LocMonitorServerSession::Server()
	{return *static_cast<CTe_LocMonitorServer*>(const_cast<CServer2*>(CSession2::Server()));}


#endif  // __LBSTESTLOCMONITORSESSION_H__
