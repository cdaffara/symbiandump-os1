/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CXXXStubTestCaseParam class 
*	             member functions.
*
*/


#include "siftrace.h"
#include "remconcontroltestcaseparam.h"

CRemConControlTestCaseParam::CRemConControlTestCaseParam( void )
    {
    }


void CRemConControlTestCaseParam::ConstructL( const TRemConControlTestCaseState* aStates, TInt aStateCount )
    {
    //TRACE_ASSERT( aStates != NULL );
    //TRACE_ASSERT( aStateCount > 0 );
    iStates = REINTERPRET_CAST( TRemConControlTestCaseState*, User::Alloc( sizeof ( TRemConControlTestCaseState ) * aStateCount ) );
    User::LeaveIfNull( iStates );
    Mem::Copy( iStates, aStates, sizeof ( TRemConControlTestCaseState ) * aStateCount );
    iStateCount = aStateCount;
    }

    
CRemConControlTestCaseParam::~CRemConControlTestCaseParam( void )
    {
    User::Free( iStates );
    iStates = NULL;
    }
    
    
const TRemConControlTestCaseState* CRemConControlTestCaseParam::States( void ) const
    {
    return iStates;
    }
       
TInt CRemConControlTestCaseParam::StateCount( void ) const
    {
    return iStateCount;
    }
//End of File
