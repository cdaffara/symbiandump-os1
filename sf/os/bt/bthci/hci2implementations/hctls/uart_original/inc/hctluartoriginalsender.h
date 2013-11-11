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
//

/**
 @file
 @internalComponent
*/

#ifndef HCTLUARTORIGINALSENDER_H
#define HCTLUARTORIGINALSENDER_H

#include <e32base.h>

class RBusDevComm;
class MHCTLChannelObserver;

NONSHARABLE_CLASS(CHCTLUartOriginalSender) : public CActive
    {
public:
    static CHCTLUartOriginalSender* NewL(RBusDevComm& aPort);
    ~CHCTLUartOriginalSender();         

    TInt Write(const TDesC8& aData);
	void SetChannelObserver(MHCTLChannelObserver& aObserver);

private:
	// From CActive
	virtual void RunL();     
	virtual void DoCancel();

    CHCTLUartOriginalSender(RBusDevComm& aPort);

private:
	MHCTLChannelObserver* iChannelObserver;
	RBusDevComm& iPort;
    };

#endif // HCTLUARTORIGINALSENDER_H

