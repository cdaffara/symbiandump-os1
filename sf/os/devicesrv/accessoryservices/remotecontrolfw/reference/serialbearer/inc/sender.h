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

#ifndef SENDER_H
#define SENDER_H

#include <e32base.h>

class RComm;
class MSendObserver;

/**
Active Object for sending text over an RComm. Notifies completion to a 
MSendObserver.
*/
NONSHARABLE_CLASS(CSender) : public CActive
	{
public:
	static CSender* NewL(RComm& aComm, MSendObserver& aObserver);
	~CSender();

public:
	void Send(const TDesC8& aBuf);

private: // from CActive
	void RunL();
	void DoCancel();

private:
	CSender(RComm& aComm, MSendObserver& aObserver);
	void ConstructL();

private: // unowned
	RComm& iComm;
	MSendObserver& iObserver;
	};

#endif // SENDER_H
