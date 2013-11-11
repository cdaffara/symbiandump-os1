// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CCommReader.
// 

#ifndef __COMMREADER_H__
#define __COMMREADER_H__

#include <e32base.h>

class MCommObserver;
class CCommEngine;

class CCommReader : public CActive
	{
public:
	CCommReader(CCommEngine* aComm, TInt aPriority);
	virtual ~CCommReader();
	void Activate();
	TRequestStatus& StatusRef();
protected:
	virtual void RunL();
	virtual void DoCancel();
private:
	CCommEngine* iComm; //Not owned
	};


#endif

