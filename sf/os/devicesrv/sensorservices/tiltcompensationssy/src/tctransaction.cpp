/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TTCTransaction class implementation.
*
*/


#include "tctransaction.h"
#include "trace.h"

TTCTransaction::TTCTransaction( TTCTransactionId aId ):
    iId( aId )
    {
    FUNC_LOG;
    }

// METHODS

// ----------------------------------------------------------------------------------
// TTCTransaction::Id
// ----------------------------------------------------------------------------------
//
TTCTransactionId TTCTransaction::Id() const
    {
    FUNC_LOG;
    
    return iId;
    }

// End of File
