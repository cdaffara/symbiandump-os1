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
// Defines the base class for baseband SAPs
// 
//

/**
 @file
 @internalComponent
*/

#ifndef _BASEBANDSAP_H_
#define _BASEBANDSAP_H_

#include <es_prot.h>
#include "BtSap.h"
#include "AcceptWatchdog.h"
#include "MBtHostControllerEventInternalNotifier.h"
#include "notification.h"
#include "SecManNotifiers.h"
#include "btconsts.h"

static const TUint8 KActiveDisconnectReason = 0x13;


class TBTConnect;
class CBTBasebandModel;
class CPhysicalLinksManager;
class CPhysicalLink;

enum TBasebandTimeout
	{
	EAccept,
	};

class MLogicalLink
/**
	@internalComponent
	Logical links mixin in from this to be offered the logical link connection requests
	As it happens, for ACL these occur when PHY connect requests arrive
	For SCO its a separate event
**/
	{
public:
/**
	@return ETrue if connection is accepted, EFalse if not
	@param	aConnect a record detailing the connection
	@param	aPhysicalLink the physical link bearing the potential logical link
*/
	virtual TBool ConnectRequest(const TBTConnect& aConnect, const CPhysicalLink& aPhysicalLink) =0;

/**
	@param	aConnect the new logical link details
*/
	virtual void ConnectComplete(const TBTConnect& aConnect) =0;

/**
	Signalled when logical link has disconnected
*/
	virtual void Disconnection() =0;

/**
	Signalled when an error situation affecting the logical link has occurred
	@param	aError	The error
*/
	virtual void Error(TInt aError) =0;

/**
	Data for the logical link has been received
*/
	virtual void DataReceived(THCIConnHandle aConnH, TUint8 aFlag, const TDesC8& aData) =0;

/**
	@param	aConnH	The handle on which data has been sent
	@param	aNumPackets	The number of packets sent
*/
	virtual void PacketsSent(THCIConnHandle aConnH, TUint16 aNumPackets) =0;

/**
	@return ETrue if the logical link considers itself to be idle
*/
	virtual TBool IsIdle() const = 0;
	};

/**
	@internalComponent
	class CBTBasebandSAP
	The (abstract) base SAP for the linkmgr
	It forms the basis for ACL,SCO and Proxy SAPs

	Derivation allows for SAPs to spy on the phy
	
**/

/*abstract*/ NONSHARABLE_CLASS(CBTBasebandSAP) : public CBluetoothSAP, public MPhysicalLinkObserver
	{
public:
// from SAP
	virtual void Start();
	virtual void LocalName(TSockAddr& anAddr) const ;
	virtual TInt SetLocalName(TSockAddr& anAddr);
	virtual void RemName(TSockAddr& anAddr) const ;
	virtual TInt SetRemName(TSockAddr& anAddr);
	virtual TInt GetOption(TUint level,TUint name,TDes8& anOption)const ;
	virtual void Ioctl(TUint level,TUint name,TDes8* anOption);
	virtual void CancelIoctl(TUint aLevel,TUint aName);
	virtual TInt SAPSetOption(TUint level,TUint name,const TDesC8 &anOption);
	virtual void ActiveOpen();
	virtual void ActiveOpen(const TDesC8& aConnectionData);
	virtual TInt PassiveOpen(TUint aQueSize);
	virtual TInt PassiveOpen(TUint aQueSize,const TDesC8& aConnectionData);
	virtual void Shutdown(TCloseType option);
	virtual void Shutdown(TCloseType option,const TDesC8& aDisconnectionData);
	virtual void AutoBind();
	TUint Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* aAddr);
	void GetData(TDes8& aDesc,TUint options,TSockAddr* anAddr=NULL);

	TInt BindLink(TLinkType aLinkType, CPhysicalLink&	aPhysicalLink);
	void UnbindLink(TLinkType aLinkType);

	void LinkStateIdle();

	void RemoveChild(CBTBasebandSAP* aChild);
	void DeleteChild(CBTBasebandSAP* aChild);

	virtual void Timeout(TBasebandTimeout aTimeout) =0;

	inline THCIConnHandle Handle() const;
	inline MSocketNotify* Socket() const;
	inline CBTBasebandSAP*& ListeningSAP();
	virtual TBool IsIdle() const;
	virtual TPhysicalLinkObserverQLink& ObserverQLink() {return iQueLink;};
	virtual void ParentClosing();
	
protected:
	CBTBasebandSAP(CPhysicalLinksManager& aConnectionMan, CPhysicalLink* aConnection);
	void ConstructL();
	CBTBasebandModel& Baseband() const;

protected:
	THCIConnHandle			iHandle;	// the handle this class represents
	CPhysicalLinksManager&	iLinksMan;
	CPhysicalLink*			iPhysicalLink;
	CBTBasebandSAP*			iParent;
	CBTBasebandSAP*			iChild;
	TAcceptWatchdog			iAcceptWatchdog;
	TPhysicalLinkObserverQLink	iQueLink;
	};

inline THCIConnHandle CBTBasebandSAP::Handle() const
	{
	return iHandle;
	}


inline MSocketNotify* CBTBasebandSAP::Socket() const
	{
	return iSocket;
	}

inline CBTBasebandSAP*& CBTBasebandSAP::ListeningSAP()
	{
	return iParent;
	}

#endif //_BASEBANDSAP_H_
