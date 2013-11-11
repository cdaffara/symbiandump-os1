/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: STIF testclass declaration
*
*/

#include "LocInfoConversionHandler.h"

CLocInfoConversionHandler* CLocInfoConversionHandler::NewL()
    {
    CLocInfoConversionHandler* self = new ( ELeave ) CLocInfoConversionHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CLocInfoConversionHandler::CLocInfoConversionHandler():CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

CLocInfoConversionHandler::~CLocInfoConversionHandler()
    {
    if( IsActive() )
        {
        Cancel();
        }
    }

void CLocInfoConversionHandler::Start()
    {
    iStatus = KRequestPending;
    SetActive();
    }

void CLocInfoConversionHandler::RunL()
    {
    CActiveScheduler::Stop();
    }


void CLocInfoConversionHandler::ConstructL()
    {
    // Nothign to do here
    }


void CLocInfoConversionHandler::DoCancel()
    {
    // Nothign to do here
    }
