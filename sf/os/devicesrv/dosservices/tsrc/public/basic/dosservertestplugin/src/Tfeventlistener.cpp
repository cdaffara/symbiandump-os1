/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
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


#include <DosSvrServices.h>
#include "siftrace.h"
#include "tfdostypes.h"
#include "ctfeventlistener.h"
#include "mtfdoscontrol.h"

CTFEventListener::CTFEventListener( RDosServer& aServer )
: CDosEventListenerBase( aServer )
    {
    }


CTFEventListener::~CTFEventListener( void )
    {
    }


static void NotifyEvent( TInt aEvent, TInt aParameter )
    {
    MTFDosControl* control = GetDosServerControl();
    if ( control != NULL )
        {
        control->NotifyDosEvent( aEvent, aParameter );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        }
    }   

void CTFEventListener::GeneralEventL(const TRegisterEvent& aEvent, TAny* aParameter)
    {        
    NotifyEvent( KLDSYDummyEvent, *(TInt*)aParameter );
    }
