// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTBASEBAND_H
#define BTBASEBAND_H

/** 
@file
@internalTechnology
@released
*/

#include <e32base.h>
#include <es_sock.h>
#include <btsockaddr.h>
#include <bttypes.h>
#include <btbasebandpolicy.h>
#include <bluetooth/lmoptions.h>

NONSHARABLE_CLASS(RBTBaseband)
/** API useful for Bluetooth as seen from a single physical link perspective
@internalTechnology
@released
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
*/
	{
public:
	RBTBaseband();
	//API useful for Bluetooth as seen from a single physical link perspective
	TInt Open(RSocketServ& aSocketServ, RSocket& aSocket);
	TInt Open(RSocketServ& aSocketServ, const TBTDevAddr& aDevAddr);
	void Close();
	TInt PhysicalLinkState(TUint32& aState);
	TInt BasebandState(TUint32& aState); 
	TInt PreventRoleSwitch();
	TInt AllowRoleSwitch();
	TInt RequestMasterRole();
	TInt RequestSlaveRole();
	TInt PreventLowPowerModes(TUint32 aLowPowerModes);
	TInt AllowLowPowerModes(TUint32 aLowPowerModes);
	TInt ActivateSniffRequester();
	TInt ActivateParkRequester();
	TInt CancelLowPowerModeRequester();
	TInt RequestExplicitActiveMode(TBool aActive);
	TInt RequestChangeSupportedPacketTypes(TUint16 aPacketTypes);
	//THE TWO NOTIFY METHODS BELOW MUST NOT BE CALLED CONCURRENTLY
	//Method to be used if only the next event should be notified
	void ActivateNotifierForOneShot(TBTBasebandEvent& aEventNotification, 
		                            TRequestStatus& aStatus, 
									TUint32 aEventMask);
	//Method to be used if it is intended to call it again 
	//(or call CancelNextBasebandChangeEventNotifier) when it completes 
	// - this sets up a continuous monitoring of events on the server.
	//Each time ActivateNotifierForOneShot is called it will either return
	//the next event in the servers notification queue or if the
	//queue is empty it will await the next event. 
	void ActivateNotifierForRecall(TBTBasebandEvent& aEventNotification, 
		                           TRequestStatus& aStatus, 
								   TUint32 aEventMask);
	void ReadNewPhysicalLinkMetricValue(TRequestStatus& aStatus,
								TDes8& aData,
								TBTLMIoctls aIoctl);
	void CancelPhysicalLinkMetricUpdate();
	void CancelNextBasebandChangeEventNotifier();
	TInt Authenticate();
	
	//API useful for Bluetooth as seen from a device perspective
	TInt Open(RSocketServ& aSocketServ);
	void Connect(const TBTDevAddr& aDevAddr, TRequestStatus& aStatus);
	void Connect(const TPhysicalLinkQuickConnectionToken& aToken, TRequestStatus& aStatus);
	TInt Broadcast(const TDesC8& aData); // testing broadcast writes
	TInt ReadRaw(TDes8& aData);
	TInt Enumerate(RBTDevAddrArray& aBTDevAddrArray, TUint aMaxNumber);
	void TerminatePhysicalLink(TInt aReason);
	void TerminatePhysicalLink(TInt aReason, TRequestStatus& aStatus);
	void TerminatePhysicalLink(TInt aReason, const TBTDevAddr& aDevAddr, TRequestStatus& aStatus);
	void ShutdownPhysicalLink(TRequestStatus& aStatus);
	void TerminateAllPhysicalLinks(TRequestStatus& aStatus);
	void TerminateAllPhysicalLinksForPowerOff(TRequestStatus& aStatus);
	TInt SubSessionHandle() const;

private:
	TInt RequestRole(TBTLMOptions aRole);
	void LocalComplete(TRequestStatus& aStatus, TInt aErr);
	void SetClientPending(TRequestStatus& aStatus);
	void DoConnect(TRequestStatus& aStatus);
	TInt Enumerate(TDes8& aData);
	TInt Construct();
		
private:
	TAny*					iUnusedPointer;
	RSocket					iSocket;

	TRequestStatus*							iClientRequestStatus;
	TBTSockAddr								iSocketAddress;				
	TPhysicalLinkQuickConnectionTokenBuf	iConnectToken;
	TBuf8<1>								iConnectInData; // not used yet - needed tho!
	TBuf8<1>								iDummySCOShutdownDescriptor;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};

#endif // BTBASEBAND_H
