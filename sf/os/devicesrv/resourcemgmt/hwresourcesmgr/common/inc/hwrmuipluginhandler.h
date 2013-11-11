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
 
#ifndef HWRMUIPLUGINHANDLER_H
#define HWRMUIPLUGINHANDLER_H

#include <e32base.h>
#include "hwrmuipluginservice.h"

/*	
* Plugin handler: encapsulates the plugin loading and unloading and
* delegates requests onto plugin implementation	
*/
class CHwrmUiPluginHandler : public CBase
	{
public:
	static CHwrmUiPluginHandler* NewL();
	virtual ~CHwrmUiPluginHandler();

private:
	 CHwrmUiPluginHandler();
     void ConstructL();
     
public:        
    void AddForegroundObserverL(MHwrmForegroundObserver* aObserver);        
    TBool InForeground() const;
  
private:
	CHwrmUiPluginService* iUiPluginService;                            
	};

#endif      // HWRMUIPLUGINHANDLER_H   
            
// End of File
