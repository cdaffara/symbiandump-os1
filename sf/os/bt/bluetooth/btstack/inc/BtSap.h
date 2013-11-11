// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _BTSAP_H
#define _BTSAP_H

#include <es_prot.h>
#include <bt_sock.h>
#include <btdevice.h>
#include "SecManNotifiers.h"

class CBTSecMan;
class CBTCodServiceMan;

NONSHARABLE_CLASS(CBluetoothSAP) : public CServProviderBase, public MAccessRequestResponseHandler
/**
	Acts as the base for Bluetooth SAPs; factoring aspects common to all.
	This should be extended in future...
**/
	{
public:
	// return reference to the security of the service represented by this SAP
	const TBTServiceSecurity& Security() const;

	// return a pointer to a possible device override to the security settings
	// returns NULL if none
	const TBTServiceSecurityPerDevice* Override(const TBTDevAddr& aAddress) const;
	
	// begin the security state machine
	void StartAccessRequest(const CBluetoothSAP& aSAPWithSecuritySettings, TBool aSecurityModeFourOutgoing = EFalse);
	void CancelAccessRequest();

	// callback from secman with either granted or denied access
	virtual void AccessRequestComplete(TInt aResult);

	virtual void IoctlComplete(TInt aErr, TUint aLevel, TUint aName, TDesC8* aBuf);
	inline CBTSecMan& SecMan() const;
	inline CBTCodServiceMan& CodMan() const;
	inline const TBTDevAddr& RemoteAddress() const;

	TInt SetCodServiceBits(TUint16 aNewCodServiceBits);
	void RegisterCodService();
	void DeregisterCodService();

	TInt SetOption(TUint aLevel,TUint aName,const TDesC8 &aOption);
	virtual TInt SAPSetOption(TUint aLevel,TUint aName,const TDesC8 &aOption) = 0;
	
	//From CServProviderBase
	TInt SecurityCheck(MProvdSecurityChecker *aSecurityChecker);

	~CBluetoothSAP();

protected:
	CBluetoothSAP(CBTSecMan& aSecMan, CBTCodServiceMan& aCodMan);
	void ConstructL();
	TInt SetDeviceOverride(const TDesC8& aOption);
	TInt GetDeviceOverride(TDes8& aOption) const;

private:
	static TInt AccessDeniedCallBack(TAny* aBtSap);

protected:
	TBTServiceSecurity	iSecurity;	// the security the service (on this SAP) has requested
	RBTSecurityArray	iDeviceOverrides;	// the overrides
	TUint				iIoctlLevel;
	TUint				iIoctlName;
	TBTDevAddr			iRemoteDev;
	TUint16				iCodServiceBits;// SAP's record Services they set (see TBTMajorServiceClass), 
										// so they can be removed from the CoD when a SAP dies.
										// MSB is Register Flag
										
	//Mixin providing security checking, This is not an owned variable.
	MProvdSecurityChecker* iSecurityChecker;

private:
	CBTSecMan&			iSecMan;
	CBTCodServiceMan&	iCodMan;

	CAsyncCallBack*		iAccessDeniedCallBack;
	TBool iNoSecurityRequired;
	};


inline const TBTDevAddr& CBluetoothSAP::RemoteAddress() const
	{
	return iRemoteDev;
	}

inline CBTSecMan& CBluetoothSAP::SecMan() const
	{
	return iSecMan;
	}

inline CBTCodServiceMan& CBluetoothSAP::CodMan() const
	{
	return iCodMan;
	}

#endif //_BTSAP_H
