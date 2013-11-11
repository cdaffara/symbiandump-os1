// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _MBTHOSTCONTROLEREVENTINTERNALNOTIFIER_H_
#define _MBTHOSTCONTROLEREVENTINTERNALNOTIFIER_H_

// provides the conduit for propagating events through the SAPs and the ConnectionsManager
// the methods are void, non-leaving to encourage the fact that the HCIFacade will not
// have implementation of policy
// it is up to the notifiees to instruct the HCIFacade to issue commands (or not)
//	in case of an error

#include <bttypes.h>
#include <btdevice.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hcitypes.h>

class MPINCodeResponseHandler;
class MLinkKeyResponseHandler;
class MIOCapabilityResponseHandler;
class MNumericComparisonResponseHandler;
class MPasskeyResponseHandler;
class MOOBDataResponseHandler;
class TBTDevAddr;
class TBTFeatures;
class TBTConnect;

class MBtHostControllerEventInternalNotifier
	{
public:
	virtual void ConnectionComplete(THCIErrorCode /*aErr*/, const TBTConnect& /*aConn*/) {};
	virtual void ConnectionRequest(const TBTConnect& /*aConn*/) {};
	virtual void Disconnection(THCIErrorCode /*aErr*/, THCIConnHandle /*aConnH*/, THCIErrorCode /*aResult*/) {};
	virtual void CompletedPackets(THCIConnHandle /*aConnH*/, TUint16 /*aNumPackets*/) {};
	virtual void AuthenticationComplete(THCIErrorCode /*aErr*/, THCIConnHandle /*aConnH*/) {};
	virtual void EncryptionChange(THCIErrorCode /*aErr*/, THCIConnHandle /*aConnH*/, TBool /*aEncrypted*/) {};
	virtual void ACLDataReceived(THCIConnHandle /*aConnH*/, TUint8 /*aFlag*/, const TDesC8& /*aData*/) {};
	virtual void SCODataReceived(THCIConnHandle /*aConnH*/, const TDesC8& /*aData*/) {};

	virtual void PinRequest(const TBTDevAddr& /*aBDAddr*/, MPINCodeResponseHandler& /*aRequester*/) {};
	virtual void NewLinkKey(const TBTDevAddr& /*aBDAddr*/, const TBTLinkKey& /*aLinkKey*/, THCILinkKeyType /* aLinkKeyType */) {};
	virtual void LinkKeyRequest(const TBTDevAddr& /*aBDAddr*/, MLinkKeyResponseHandler& /*aRequester*/) {};

	virtual void ReadRemoteVersionInfoComplete(THCIErrorCode /*aErr*/, THCIConnHandle /*aConnH*/, const TBTDevRemoteHwVersion& /*aVer*/) {};
	virtual void ReadRemoteSupportedFeaturesComplete(THCIErrorCode /*aErr*/, THCIConnHandle /*aConnH*/, const TBTFeatures& /*aBitMask*/) {};
	virtual void ReadRemoteExtendedFeaturesComplete(THCIErrorCode /*aErr*/, THCIConnHandle /*aConnH*/,  TUint64 /*aBitMask*/, TUint8 /* aPageNumber */, TUint8 /* aMaximimPageNumber */) {};
	virtual void PacketTypeChange(THCIErrorCode /*aErr*/, THCIConnHandle /*aConnH*/, TUint16 /*aNewPacket*/) {};
	virtual void MaxSlotsChange(THCIConnHandle /*aConnH*/, TUint8 /*aSlots*/) {};

	virtual void ModeChange(THCIErrorCode /*aErr*/, THCIConnHandle /*aConnH*/, TBTLinkMode /*aMode*/, TBasebandTime /*aInterval*/) {};
	virtual void RoleChange(THCIErrorCode /*aErr*/, const TBTDevAddr& /*aAddr*/, TBTBasebandRole /*aRole*/) {};
	virtual void WriteLinkPolicySettingsCompleteEvent(THCIErrorCode /*aErr*/, THCIConnHandle /*aConnH*/) {};

	virtual void ClockOffset(THCIErrorCode /*aErr*/, THCIConnHandle /*aConnH*/, TBasebandTime /*aClockOffset*/) {};
	virtual void RemoteName(THCIErrorCode /*aErr*/, const TBTDevAddr& /*aAddr*/, const TBTDeviceName8& /*aName*/) {};

	virtual void SimplePairingComplete(const TBTDevAddr& /*aBDAddr*/, THCIErrorCode /*aErr*/) {};
	
	virtual void EncryptionKeyRefreshComplete(THCIErrorCode /*aErr*/, THCIConnHandle /*aConnH*/) {};
	};

#endif //_MBTHOSTCONTROLEREVENTINTERNALNOTIFIER_H_
