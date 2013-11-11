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

#ifndef _SECMANNOTIFIERS_H
#define _SECMANNOTIFIERS_H

#include <e32base.h>
#include <bluetooth/hci/hcitypes.h>

class TBTDevAddr;


class MAccessRequestResponseHandler
	{
public:
	virtual void AccessRequestComplete(TInt aResult)=0;
	};

/**
Interface for a class that can handle PIN commands (HCI Facade typically)
**/
class MPINCodeResponseHandler
	{
public:
	virtual TInt PINCodeRequestReply(const TBTDevAddr& aBdaddr,const TDesC8& aPIN) const =0;
	virtual TInt PINCodeRequestNegativeReply(const TBTDevAddr& aBdaddr) const =0;
	};

/**
Interface for a class that can handle link key commands(HCI Facade typically)
**/
class MLinkKeyResponseHandler
	{
public:
	virtual TInt LinkKeyRequestReply(const TBTDevAddr& aBdaddr,const TDesC8& aLinkKey) const =0;
	virtual TInt LinkKeyRequestNegativeReply(const TBTDevAddr& aBdaddr) const =0;
	};

/**
Interface for a class that can handle passkey commands(HCI Facade typically)
**/
class MIOCapabilityResponseHandler
	{
public:
	virtual TInt IOCapabilityRequestReply(const TBTDevAddr& aBDADDR, TUint8 aIOCapability, TUint8 aOOBDataPresent, TUint8 aAuthenticationRequirements) const =0;
	virtual TInt IOCapabilityRequestNegativeReply(const TBTDevAddr& aBDADDR, TUint8 aReason) const =0;
	};

/**
Interface for a class that can handle passkey commands(HCI Facade typically)
**/
class MPasskeyResponseHandler
	{
public:
	virtual TInt UserPasskeyRequestReply(const TBTDevAddr& aBDADDR, TUint32 aNumericValue) const =0;
	virtual TInt UserPasskeyRequestNegativeReply(const TBTDevAddr& aBdaddr) const =0;
	};

/**
Interface for a class that can handle OOB data commands(HCI Facade typically)
**/
class MOOBDataResponseHandler
	{
public:
	virtual TInt RemoteOOBDataRequestReply(const TBTDevAddr& aBDADDR, const TBluetoothSimplePairingHash& aC, const TBluetoothSimplePairingRandomizer& aR) const =0;
	virtual TInt RemoteOOBDataRequestNegativeReply(const TBTDevAddr& aBDADDR) const =0;
	};

/**
Interface for a class that can handle numeric comparison confirmation commands(HCI Facade typically)
**/
class MNumericComparisonResponseHandler
	{
public:
	virtual TInt UserConfirmationRequestReply(const TBTDevAddr& aBDADDR) const =0;
	virtual TInt UserConfirmationRequestNegativeReply(const TBTDevAddr& aBDADDR) const =0;
	};

#endif //_SECMANNOTIFIERS_H
