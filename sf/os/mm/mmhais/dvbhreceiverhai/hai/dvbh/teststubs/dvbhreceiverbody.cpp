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
// Example implementation of RDvbhReceiver::RBody for the unit test stub
// 
//

/**
 @file
 @internalComponent
 @prototype
*/

#include "dvbhreceiverbody.h"
#include <in_sock.h>

RDvbhReceiver::RBody::RBody()
: RBusLogicalChannel()
	{
	}
		
TInt RDvbhReceiver::RBody::GetDriverVersion( TVersion& /*aVersion*/ )
	{
	return KErrNotSupported;
	}	
	
TInt RDvbhReceiver::RBody::GetSupportedReceiverTypes( RArray<TDvbhReceiverType>& /* aReceiverTypes */ )
    {
	return KErrNotSupported;
    }

TInt RDvbhReceiver::RBody::Open( const TDvbhReceiverType /* aReceiverType */ )
	{
	return KErrNotSupported;
	}	

void RDvbhReceiver::RBody::Close()
	{
	}	

TInt RDvbhReceiver::RBody::PowerOn( TRequestStatus& /* aStatus */ )
    {
    return KErrNotSupported;
    }

void RDvbhReceiver::RBody::CancelPowerOn()
    {
    }

void RDvbhReceiver::RBody::PowerOff( TRequestStatus& /* aStatus */ )
	{
	}	

void RDvbhReceiver::RBody::CancelPowerOff()
    {
    }

void RDvbhReceiver::RBody::SetDisabled( TBool /*aDisable*/, TRequestStatus& /* aStatus */ )
	{
	}	

void RDvbhReceiver::RBody::CancelSetDisabled()
    {
    }

TInt RDvbhReceiver::RBody::SetScanConfiguration( const TDvbhScanConfiguration& /*aScanConfiguration*/ )
	{
	return KErrNotSupported;
	}	

TInt RDvbhReceiver::RBody::GetScanConfiguration( TDvbhScanConfiguration& /*aScanConfiguration*/ )
	{
	return KErrNotSupported;
	}	

TInt RDvbhReceiver::RBody::GetDvbhVersion( TVersion& /*aVersion*/ )
	{
	return KErrNotSupported;
	}	

TInt RDvbhReceiver::RBody::GetHardwareInfo( TDvbhHardwareInfo& /*aHardwareInfo*/ )
	{
	return KErrNotSupported;
	}	

TInt RDvbhReceiver::RBody::Scan( MDvbhScanObserver& /*aObserver*/, TRequestStatus& /*aStatus*/ )
	{
	return KErrNotSupported;
	}	

void RDvbhReceiver::RBody::CancelScan()
	{
	}	

TInt RDvbhReceiver::RBody::SetPlatform( const TDvbhNetwork& /*aNetwork*/, const TDvbhPlatform& /*aPlatform*/, TRequestStatus& /*aStatus*/ )
	{
	return KErrNotSupported;
	}	

void RDvbhReceiver::RBody::CancelSetPlatform()
	{
	}	

TInt RDvbhReceiver::RBody::CreateFilter( const TIp6Addr& /*aSourceAddress*/, TInt& /*aFilterId*/, TRequestStatus& /*aStatus*/ )
	{
	return KErrNotSupported;	
	}	

TInt RDvbhReceiver::RBody::CancelFilter( TInt /*aFilterId*/ )
	{
	return KErrNotSupported;
	}	

TInt RDvbhReceiver::RBody::ReceiveIPData( MDvbhDataObserver& /*aObserver*/ )
	{
	return KErrNotSupported;
	}	

void RDvbhReceiver::RBody::CancelReceiveIPData()
	{
	}	

TInt RDvbhReceiver::RBody::UpdateNetworkTime( TRequestStatus& /*aStatus*/ )
	{
	return KErrNotSupported;
	}	

void RDvbhReceiver::RBody::CancelUpdateNetworkTime()
	{
	}	

TInt RDvbhReceiver::RBody::CustomCommand(
            TInt /*aCommand*/,
            const TDesC8& /*aInputData*/,
            TDes8& /*aOutputBuffer*/,
            TRequestStatus& /*aStatus*/ )
	{
	return KErrNotSupported;
	}	

void RDvbhReceiver::RBody::CancelCustomCommand( TRequestStatus& /*aStatus*/ )
	{
	}	

TInt RDvbhReceiver::RBody::CustomCommand( TInt /*aCommand*/, const TDesC8& /*aInputData*/ )
	{
	return KErrNotSupported;
	}	


	

