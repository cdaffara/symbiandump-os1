// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !(defined __TESTBASES_H__)
#define __TESTBASES_H__

#include <e32base.h>
#include "NetConTestBases.h"
#include "CNetworkController.h"

class CQueueTestBase : public CNetConTestBase, public MNetworkControllerObserver
	{
public:
	virtual ~CQueueTestBase();

	// from MNetworkControllerObserver
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);
	virtual void ReconnectComplete(TInt aError);

protected:
	CQueueTestBase();
	void ConstructL();

protected:
	CNetworkController* iNetCon;
	};


#endif // __TESTBASES_H__

