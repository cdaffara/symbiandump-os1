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
* Description:  CTCStateTiltListenData class implementation.
*
*/


#include <sensrvchanneldatatypes.h>
#include <sensrvgeneralproperties.h>
#include <tiltcompensation.h>
#include <tiltapplicationpskeys.h>

#include "propertylistener.h"
#include "trace.h"

CProprertyListener::CProprertyListener( TInt& aX, TInt& aY,
    TInt& aZ, TInt& aAccX, TInt& aAccY,
    TInt& aAccZ, TInt& aFilterAngle, TInt& aFilterAttenation,
    TInt& aAccAverageSize, TInt& aMagAverageSize, TInt& aFilterDivisor ):
    CActive( CActive::EPriorityStandard ),
    iDataXAxis( aX ),
    iDataYAxis( aY ),
    iDataZAxis( aZ ),
    iDataXAccAxis( aAccX ),
    iDataYAccAxis( aAccY ),
    iDataZAccAxis( aAccZ ),
    iFilterAngle( aFilterAngle ),
    iFilterAttenation( aFilterAttenation ),
    iAccAverageSize( aAccAverageSize ),
    iMagAverageSize( aMagAverageSize ),
    iFilterDivisor( aFilterDivisor )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    iSubscribe.Attach( KTiltApplication, KAttenationFilter );
    if( !IsActive() )
        {
        iStatus = KRequestPending;
        iSubscribe.Subscribe( iStatus );
        SetActive();
        }
    }

CProprertyListener* CProprertyListener::NewL( TInt& aX, TInt& aY,
    TTInt& aFilterAngle, TInt& aFilterAttenation, TInt& aAccAverageSize,
     TInt& aMagAverageSize, TInt& aFilterDivisor )
    {
    CProprertyListener* self = new( ELeave )CProprertyListener( aX, aY, aZ, aAccX,
     aAccY, aAccZ, aFilterAngle, aFilterAttenation, aAccAverageSize, aMagAverageSize, aFilterDivisor );
    return self;
    }

void CProprertyListener::RunL()
    {
    User::LeaveIfError(RProperty::Get( KTiltApplication, KValueXAxis, iDataXAxis ) );
    User::LeaveIfError(RProperty::Get( KTiltApplication, KValueYAxis, iDataYAxis ) );
    User::LeaveIfError(RProperty::Get( KTiltApplication, KValueZAxis, iDataZAxis ) );
    User::LeaveIfError(RProperty::Get( KTiltApplication, KHXData, iDataXAccAxis ) );
    User::LeaveIfError(RProperty::Get( KTiltApplication, KHYData, iDataYAccAxis ) );
    User::LeaveIfError(RProperty::Get( KTiltApplication, KHZData, iDataZAccAxis ) );
    User::LeaveIfError(RProperty::Get( KTiltApplication, KAngleFilter, iFilterAngle ) );
    User::LeaveIfError(RProperty::Get( KTiltApplication, KAttenationFilter, iFilterAttenation ) );
    User::LeaveIfError(RProperty::Get( KTiltApplication, KAccAverageSize, iAccAverageSize ) );
    User::LeaveIfError(RProperty::Get( KTiltApplication, KMagAverageSize, iMagAverageSize ) );
    User::LeaveIfError(RProperty::Get( KTiltApplication, KFilterDivisor, iFilterDivisor ) );
    iSubscribe.Subscribe( iStatus );
    SetActive();
    }

void CProprertyListener::DoCancel()
    {
    iSubscribe.Cancel();
    }
    
TInt CProprertyListener::RunError( TInt aError )
    {
    ERROR_1( aError, "CProprertyListener::RunError - aError: %d ", aError );
    return KErrNone;
    }

CProprertyListener::~CProprertyListener()
    {
    Cancel();
    iSubscribe.Close();
    }

// End of File
