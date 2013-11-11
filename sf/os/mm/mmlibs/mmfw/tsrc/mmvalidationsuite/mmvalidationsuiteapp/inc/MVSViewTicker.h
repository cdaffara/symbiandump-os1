// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CMVSViewTicker.h
// Part of the MVS Application for TechView
//

#ifndef CMVSVIEWTICKER_H
#define CMVSVIEWTICKER_H


#include "MVSAppUI.h"

class CMVSViewTicker
	{
public:
    //Create a new ticker
	static CMVSViewTicker* NewL(CMVSAppUi* aAppUi);
	~CMVSViewTicker();

private:
    //Overriden 'tick' method, causes a callback.
	static TInt Tick(TAny* aObject);
	CMVSViewTicker(CMVSAppUi* aAppUi);
	void ConstructL();

private:
	CPeriodic* iPeriodic;
    CMVSAppUi* iAppUi;
	TInt	iTickInterval; //The 'tick' interval.
	};

#endif CMVSVIEWTICKER_H
