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
// Example declaration of RDvbhReceiver::RBody for the test stub
// 
//

/**
 @file
 @internalComponent
 @prototype
*/

#ifndef RDVBHRECEIVERBODY_H
#define RDVBHRECEIVERBODY_H

#include "dvbhreceiver.h"
#include <e32cons.h>
#include <e32cmn.h>

/**
* @internalComponent
* @prototype
* @see RDvbhReceiver
*
* This is the RDvbhReceiver internal, RHandleBase derived class to which the
* RDvbhReceiver test stub implementation delegates its methods.  Its public
* interface should contain the RDvbhReceiver public interface and for these
* methods the documentation is the same as for RDvbhReceiver and are not 
* documented here.
*
* This example shows RDvbhReceiver::RBody deriving from RBusLogicalChannel.
* Another suitable parent class would have been RSessionBase.
*/
NONSHARABLE_CLASS(RDvbhReceiver::RBody) : public RBusLogicalChannel
    {
public:
	RBody();  

	static TInt GetDriverVersion( TVersion& aVersion );

    static TInt GetSupportedReceiverTypes( RArray<TDvbhReceiverType>& aReceiverTypes );

	TInt Open( const TDvbhReceiverType aReceiverType );

	void Close();

    TInt PowerOn( TRequestStatus& aStatus );

    void CancelPowerOn();

    void PowerOff( TRequestStatus& aStatus );

    void CancelPowerOff();
	
	void SetDisabled( TBool aDisable, TRequestStatus& aStatus );
    
    void CancelSetDisabled();
	
	TInt SetScanConfiguration( const TDvbhScanConfiguration& aScanConfiguration );

	TInt GetScanConfiguration( TDvbhScanConfiguration& aScanConfiguration );

	TInt GetDvbhVersion( TVersion& aVersion );

	TInt GetHardwareInfo( TDvbhHardwareInfo& aHardwareInfo );

	TInt Scan( MDvbhScanObserver& aObserver, TRequestStatus& aStatus );

	void CancelScan();

	TInt SetPlatform( const TDvbhNetwork& aNetwork, const TDvbhPlatform& aPlatform, TRequestStatus& aStatus );

	void CancelSetPlatform();

	TInt CreateFilter( const TIp6Addr& aSourceAddress, TInt& aFilterId, TRequestStatus& aStatus );

	TInt CancelFilter( TInt aFilterId );

	TInt ReceiveIPData( MDvbhDataObserver& aObserver );

	void CancelReceiveIPData();

	TInt UpdateNetworkTime( TRequestStatus& aStatus );

	void CancelUpdateNetworkTime();

	TInt CustomCommand( TInt aCommand, const TDesC8& aInputData, TDes8& aOutputBuffer, TRequestStatus& aStatus );

	void CancelCustomCommand( TRequestStatus &aStatus );

	TInt CustomCommand( TInt aCommand, const TDesC8& aInputData ); 

private:
	// Copy construction and assignment not allowed
    RBody( const RBody& );
    RBody& operator=( const RBody& );
    
private:
	// Private data needed for the implementation would go here.
    };

#endif // RDVBHRECEIVERBODY_H
