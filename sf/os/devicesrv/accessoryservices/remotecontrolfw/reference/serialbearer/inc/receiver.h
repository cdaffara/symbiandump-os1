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
//

/**
 @file
 @internalComponent
*/

#ifndef RECEIVER_H
#define RECEIVER_H

#include <e32base.h>

class RComm;
class MReceiveObserver;

/**
Active Object for receiving text over an RComm. Notifies completion to a 
MReceiveObserver.
*/
NONSHARABLE_CLASS(CReceiver) : public CActive
	{
public:
	static CReceiver* NewL(RComm& aComm, MReceiveObserver& aObserver);
	~CReceiver();

public:
	void Receive(TDes8& aBuf);

private: // from CActive
	void RunL();
	void DoCancel();

private:
	CReceiver(RComm& aComm, MReceiveObserver& aObserver);
	void ConstructL();

private: // unowned
	RComm& iComm;
	MReceiveObserver& iObserver;
	};

#endif // RECEIVER_H
