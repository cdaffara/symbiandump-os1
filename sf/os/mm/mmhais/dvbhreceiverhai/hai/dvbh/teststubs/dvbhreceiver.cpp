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
// Example implementation of RDvbhReceiver
// 
//

/**
 @file
 @internalComponent
 @prototype
*/

#include "dvbhreceiver.h"
#include "dvbhreceiverbody.h"
#include <in_sock.h>

		
EXPORT_C RDvbhReceiver::RDvbhReceiver()
: iBody(NULL) //Not a C-class so no free initialisation
	{
	}	
	
EXPORT_C TInt RDvbhReceiver::GetSupportedReceiverTypes( RArray<TDvbhReceiverType>& aReceiverTypes )
    {
	//RBody::GetSupportedReceiverTypes is static.
	return RBody::GetSupportedReceiverTypes(aReceiverTypes);
    }

EXPORT_C TInt RDvbhReceiver::GetDriverVersion( TVersion& aVersion )
	{
	//RBody::GetDriverVersion is static.
	return RBody::GetDriverVersion(aVersion);
	}	
	
EXPORT_C TInt RDvbhReceiver::Open( const TDvbhReceiverType aReceiverType )
	{
	//Delegate to OpenL
	TRAPD(err, OpenL( aReceiverType ));
	return err;
	}	

EXPORT_C void RDvbhReceiver::Close()
	{
	if (iBody != NULL)
		{
		iBody->Close();
		delete iBody;
		iBody = NULL;
		}
	else
		{
		ASSERT(EFalse); //Generate a panic in debug builds to help developers detect this situation.	
		}
	}	

EXPORT_C TInt RDvbhReceiver::PowerOn( TRequestStatus& aStatus )
    {
    if (iBody != NULL)
        {
        return iBody->PowerOn(aStatus);
        }
    else
        {
        return KErrNotReady;
        }
    }   

EXPORT_C void RDvbhReceiver::CancelPowerOn()
    {
    if (iBody != NULL)
        {
        return iBody->CancelPowerOn();
        }
    else
        {
        ASSERT(EFalse); //Generate a panic in debug builds to help developers detect this situation.
        }
    }   

EXPORT_C void RDvbhReceiver::PowerOff( TRequestStatus& aStatus )
    {
	if (iBody != NULL)
		{
		iBody->PowerOff(aStatus);
		}
	else
		{
		ASSERT(EFalse); //Generate a panic in debug builds to help developers detect this situation.	
		}
    }

EXPORT_C void RDvbhReceiver::CancelPowerOff()
    {
	if (iBody != NULL)
		{
		iBody->CancelPowerOff();
		}
	else
		{
		ASSERT(EFalse); //Generate a panic in debug builds to help developers detect this situation.	
		}
    }

EXPORT_C void RDvbhReceiver::SetDisabled( TBool aDisable, TRequestStatus& aStatus )
	{
	if (iBody != NULL)
		{	
		iBody->SetDisabled(aDisable, aStatus);
		}
	else
		{
		ASSERT(EFalse); //Generate a panic in debug builds to help developers detect this situation.	
		}
	}	

EXPORT_C void RDvbhReceiver::CancelSetDisabled()
	{
	if (iBody != NULL)
		{	
		iBody->CancelSetDisabled();
		}
	else
		{
		ASSERT(EFalse); //Generate a panic in debug builds to help developers detect this situation.	
		}
	}	

EXPORT_C TInt RDvbhReceiver::SetScanConfiguration( const TDvbhScanConfiguration& aScanConfiguration )
	{
	if (iBody != NULL)
		{
		return iBody->SetScanConfiguration(aScanConfiguration);
		}
	else
		{
		return KErrNotReady;
		}
	}	

EXPORT_C TInt RDvbhReceiver::GetScanConfiguration( TDvbhScanConfiguration& aScanConfiguration )
	{
	if (iBody != NULL)
		{
		return iBody->GetScanConfiguration(aScanConfiguration);
		}
	else
		{
		return KErrNotReady;
		}
	}	

EXPORT_C TInt RDvbhReceiver::GetDvbhVersion( TVersion& aVersion )
	{
	if (iBody != NULL)
		{
		return iBody->GetDvbhVersion(aVersion);
		}
	else
		{
		return KErrNotReady;
		}
	}	

EXPORT_C TInt RDvbhReceiver::GetHardwareInfo( TDvbhHardwareInfo& aHardwareInfo )
	{
	if (iBody != NULL)
		{
		return iBody->GetHardwareInfo(aHardwareInfo);
		}
	else
		{
		return KErrNotReady;
		}
	}	

EXPORT_C TInt RDvbhReceiver::Scan( MDvbhScanObserver& aObserver, TRequestStatus& aStatus )
	{
	if (iBody != NULL)
		{
		return iBody->Scan(aObserver, aStatus);
		}
	else
		{
		return KErrNotReady;
		}
	}	

EXPORT_C void RDvbhReceiver::CancelScan()
	{
	if (iBody != NULL)
		{	
		iBody->CancelScan();
		}
	else
		{
		ASSERT(EFalse); //Generate a panic in debug builds to help developers detect this situation.	
		}
	}	

EXPORT_C TInt RDvbhReceiver::SetPlatform( const TDvbhNetwork& aNetwork, const TDvbhPlatform& aPlatform, TRequestStatus& aStatus )
	{
	if (iBody != NULL)
		{
		return iBody->SetPlatform(aNetwork, aPlatform, aStatus);
		}
	else
		{
		return KErrNotReady;
		}
	}	

EXPORT_C void RDvbhReceiver::CancelSetPlatform()
	{
	if (iBody != NULL)
		{	
		iBody->CancelSetPlatform();
		}
	else
		{
		ASSERT(EFalse); //Generate a panic in debug builds to help developers detect this situation.	
		}
	}	

EXPORT_C TInt RDvbhReceiver::CreateFilter( const TIp6Addr& aDestAddress, TInt& aFilterId, TRequestStatus& aStatus )
	{
	if (iBody != NULL)
		{
		return iBody->CreateFilter(aDestAddress, aFilterId, aStatus);
		}
	else
		{
		return KErrNotReady;
		}	
	}	

EXPORT_C TInt RDvbhReceiver::CancelFilter( TInt aFilterId )
	{
	if (iBody != NULL)
		{
		return iBody->CancelFilter(aFilterId);
		}
	else
		{
		return KErrNotReady;
		}
	}	

EXPORT_C TInt RDvbhReceiver::ReceiveIPData( MDvbhDataObserver& aObserver )
	{
	if (iBody != NULL)
		{
		return iBody->ReceiveIPData(aObserver);
		}
	else
		{
		return KErrNotReady;
		}
	}	

EXPORT_C void RDvbhReceiver::CancelReceiveIPData()
	{
	if (iBody != NULL)
		{	
		iBody->CancelReceiveIPData();
		}
	else
		{
		ASSERT(EFalse); //Generate a panic in debug builds to help developers detect this situation.	
		}
	}	

EXPORT_C TInt RDvbhReceiver::UpdateNetworkTime( TRequestStatus& aStatus )
	{
	if (iBody != NULL)
		{
		return iBody->UpdateNetworkTime(aStatus);
		}
	else
		{
		return KErrNotReady;
		}
	}	

EXPORT_C void RDvbhReceiver::CancelUpdateNetworkTime()
	{
	if (iBody != NULL)
		{	
		iBody->CancelUpdateNetworkTime();
		}
	else
		{
		ASSERT(EFalse); //Generate a panic in debug builds to help developers detect this situation.	
		}
	}	

EXPORT_C TInt RDvbhReceiver::CustomCommand(
            TInt aCommand,
            const TDesC8& aInputData,
            TDes8& aOutputBuffer,
            TRequestStatus& aStatus )
	{
	if (iBody != NULL)
		{
		return iBody->CustomCommand(aCommand, aInputData, aOutputBuffer, aStatus);
		}
	else
		{
		return KErrNotReady;
		}
	}	

EXPORT_C void RDvbhReceiver::CancelCustomCommand( TRequestStatus& aStatus )
	{
	if (iBody != NULL)
		{	
		iBody->CancelCustomCommand(aStatus);
		}
	else
		{
		ASSERT(EFalse); //Generate a panic in debug builds to help developers detect this situation.	
		}
	}	

EXPORT_C TInt RDvbhReceiver::CustomCommand( TInt aCommand, const TDesC8& aInputData )
	{
	if (iBody != NULL)
		{
		return iBody->CustomCommand(aCommand, aInputData);
		}
	else
		{
		return KErrNotReady;
		}
	}
	
void RDvbhReceiver::OpenL( const TDvbhReceiverType aReceiverType )
	{
	if (iBody == NULL)
		{
		iBody = new (ELeave) RBody;
		}
	User::LeaveIfError(iBody->Open( aReceiverType ));
	}

