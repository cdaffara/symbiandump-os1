/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

CLocInfoConversionHanlder* CLocInfoConversionHanlder::NewL()
    {
    CLocInfoConversionHanlder* self = new ( ELeave ) CLocInfoConversionHanlder;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CLocInfoConversionHanlder::CLocInfoConversionHanlder():CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

CLocInfoConversionHanlder::~CLocInfoConversionHanlder()
    {
    if( IsActive() )
        {
        Cancel();
        }
    }

void CLocInfoConversionHanlder::Start()
    {
    iStatus = KRequestPending;
    SetActive();
    }

void CLocInfoConversionHanlder::RunL()
    {
    CActiveScheduler::Stop();
    }


void CLocInfoConversionHanlder::ConstructL()
    {
    // Nothign to do here
    }


void CLocInfoConversionHanlder::DoCancel()
    {
    // Nothign to do here
    }
