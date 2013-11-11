// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HWRMTESTUIPLUGIN_B_H
#define HWRMTESTUIPLUGIN_B_H


#include "hwrmuipluginhandler.h"


class CHwrmTestUiPlugin_B : public CHwrmUiPluginService
    {
public:
	static CHwrmTestUiPlugin_B* NewL();
	~CHwrmTestUiPlugin_B();
	
protected:
	CHwrmTestUiPlugin_B();
	void ConstructL();
	
protected: // from CHwrmUiPluginService
	void AddForegroundObserverL(MHwrmForegroundObserver* aObserver);
	TBool InForeground() const;

    };
    
#endif
