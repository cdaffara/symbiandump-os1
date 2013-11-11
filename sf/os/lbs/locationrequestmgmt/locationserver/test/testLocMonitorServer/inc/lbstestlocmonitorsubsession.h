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


#ifndef __TESTPOSITIONERSUBSES_H__
#define __TESTPOSITIONERSUBSES_H__

#include <e32base.h>
#include <lbscommon.h>
#include <lbsipc.h>

#include "lbstestlocmonitorserver.h"
#include "lbstestlocmonitorsession.h"


class CTe_LocMonitorServerSubSession : public CObject
	{
public:
	static CTe_LocMonitorServerSubSession* NewL();
	~CTe_LocMonitorServerSubSession();
	void ServiceL(const RMessage2& aMessage);

private:
	CTe_LocMonitorServerSubSession();
	void ConstructL();

	};

#endif // __TESTPOSITIONERSUBSES_H__
