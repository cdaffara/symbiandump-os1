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

#include <e32base.h>
#include <ecom/ecom.h>

#include "hwrmuipluginhandler.h"
#include "HWRMtrace.h"

CHwrmUiPluginHandler* CHwrmUiPluginHandler::NewL()
	{
	COMPONENT_TRACE1(_L( "CHwrmUiPluginHandler - CHwrmUiPluginHandler::NewL()" ) );    
	
    CHwrmUiPluginHandler* self = new( ELeave ) CHwrmUiPluginHandler();    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    COMPONENT_TRACE2(_L( "CHwrmUiPluginHandler - CHwrmUiPluginHandler::NewL - return 0x%x" ), self );
    return self;    
	}
	
void CHwrmUiPluginHandler::ConstructL()
    {    
    COMPONENT_TRACE1(_L( "CHwrmUiPluginHandler::ConstructL()" ));
        
    // get plugin instance
    iUiPluginService = CHwrmUiPluginService::NewL();
    
    
    COMPONENT_TRACE1(_L( "CHwrmUiPluginHandler::ConstructL - return" ));    
    }
    
    
CHwrmUiPluginHandler::CHwrmUiPluginHandler()
	{	
	}
	
	
CHwrmUiPluginHandler::~CHwrmUiPluginHandler()
	{ 
	COMPONENT_TRACE1(_L( "CHwrmUiPluginHandler::~CHwrmUiPluginHandler()" ));	
	
	delete iUiPluginService;	
	REComSession::FinalClose();
	
	COMPONENT_TRACE1(_L( "CHwrmUiPluginHandler::~CHwrmUiPluginHandler() - return" ));
	}


void CHwrmUiPluginHandler::AddForegroundObserverL(MHwrmForegroundObserver* aObserver)
	{
	COMPONENT_TRACE1(_L( "CHwrmUiPluginHandler::AddForegroundObserverL()" ));
		
	iUiPluginService->AddForegroundObserverL(aObserver);	
	
	COMPONENT_TRACE1(_L( "CHwrmUiPluginHandler::AddForegroundObserverL() - return" ));
	}  


TBool CHwrmUiPluginHandler::InForeground() const
	{
	COMPONENT_TRACE1(_L( "CHwrmUiPluginHandler::InForeground()" ));
	
	COMPONENT_TRACE1(_L( "CHwrmUiPluginHandler::InForeground() - return" ));
		
	return iUiPluginService->InForeground();					
	}
