// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// The base class for Bluetooth Saps
// 
//

#include <es_prot.h>
#include "BtSap.h"
#include "secman.h"
#include "codman.h"

#include "BTSec.h"

//Diagnostic string for security check failures, in builds without platsec
//diagnostics this will be NULL.
const char* const KBT_SAP_NAME_DIAG = __PLATSEC_DIAGNOSTIC_STRING("Bluetooth SAP");

CBluetoothSAP::CBluetoothSAP(CBTSecMan& aSecMan, CBTCodServiceMan& aCodMan)
: iSecMan(aSecMan), iCodMan(aCodMan), iNoSecurityRequired(EFalse)
	{
	}

void CBluetoothSAP::ConstructL()
	{
	TCallBack accessDenied(AccessDeniedCallBack, this);
	iAccessDeniedCallBack = new(ELeave) CAsyncCallBack(accessDenied, CActive::EPriorityStandard);
	}

TInt CBluetoothSAP::SetDeviceOverride(const TDesC8& aOption)
/**
	Provides for overriding of the general *service* security
	for given devices.

**/
	{
	if (aOption.Length() != sizeof(TBTServiceSecurityPerDevice))
		{
		return KErrArgument;
		}

	TBTServiceSecurityPerDevice override =
		*reinterpret_cast<const TBTServiceSecurityPerDevice*>(aOption.Ptr());

	// add this device into the array of overrides for this SAP
	// if device is in the list, replace the entry
	for (TInt i=0; i<=iDeviceOverrides.Count()-1; i++)
		{
		if (iDeviceOverrides[i].DeviceAddress() == override.DeviceAddress())
			{
			// update!
			iDeviceOverrides[i].SetDeviceSecurity(override.DeviceSecurity());
			return KErrNone;
			}
		}
	// if here, this is a new entry
	return iDeviceOverrides.Append(override);
	}
	
TInt CBluetoothSAP::GetDeviceOverride(TDes8& aOption) const
/**
	Allows forgetful apps to give us an address, and we'll tell
	them the overrides for this service for that device.
*/
	{
	if (aOption.Length() != sizeof(TBTServiceSecurityPerDevice))
		{
		return KErrArgument;
		}

	TBTServiceSecurityPerDevice overrideBuf =
		*reinterpret_cast<const TBTServiceSecurityPerDevice*>(aOption.Ptr());

	const TBTServiceSecurityPerDevice* override = Override(overrideBuf.DeviceAddress());	
	if (override)
		{
		overrideBuf.SetDeviceSecurity(override->DeviceSecurity());
		aOption = TPtrC8(reinterpret_cast<TUint8*>(&overrideBuf), sizeof(TBTServiceSecurityPerDevice));
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}
	

CBluetoothSAP::~CBluetoothSAP()
	{
	__ASSERT_DEBUG(!(iCodServiceBits & KBTCodBitsRegdFlag), User::Panic(KBTCodPanic, EBTCodBadDeregister));
	iDeviceOverrides.Close();
	delete iAccessDeniedCallBack;
	}


const TBTServiceSecurity& CBluetoothSAP::Security() const
	{
	return iSecurity;
	}

const TBTServiceSecurityPerDevice* CBluetoothSAP::Override(const TBTDevAddr& aAddress) const
	{
	//look through overrides for a device override
	for (TInt i=0; i<iDeviceOverrides.Count(); i++)
		{
		if (iDeviceOverrides[i].DeviceAddress() == aAddress)
			{
			return &iDeviceOverrides[i];
			}
		}
	return NULL;
	}

void CBluetoothSAP::IoctlComplete(TInt /*aErr*/, TUint /*aLevel*/, TUint /*aName*/, TDesC8* aBuf)
	{
	iSocket->IoctlComplete(aBuf);
	}


TInt CBluetoothSAP::SecurityCheck(MProvdSecurityChecker *aSecurityChecker)
	{
	__ASSERT_ALWAYS(aSecurityChecker, User::Panic(KSECURITY_PANIC, EBTPanicNullSecurityChecker));
	
	iSecurityChecker = aSecurityChecker;
	return iSecurityChecker->CheckPolicy(KLOCAL_SERVICES, KBT_SAP_NAME_DIAG);
	}

void CBluetoothSAP::StartAccessRequest(const CBluetoothSAP& aSAPWithSecuritySettings, TBool aSecurityModeFourOutgoing)
/**
	Ask secman to do security
	@param aSAPWithSecuritySettings contains the security details
	@param aSecurityModeFourOutgoing indicates whether this is a security mode 4 request that comes before
	a security mode 2 request.  This is to be used for establishing a suitable 

	this SAP has the remote address connecting
**/
	{
	const MAccessRequestResponseHandler& handler = *this;
	
	TBTServiceSecurity secReqs(aSAPWithSecuritySettings.Security());
	if(aSecurityModeFourOutgoing)
		{
		// For security mode 4 requests we don't consider the authorisation and any pre-v2.1
		// security settings.  Note this is only applicable for outgoing access requirements.
		//So we set a concrete MITM protection value...
		secReqs.SetAuthentication(secReqs.MitmProtection());
		// ...then we remove any authorisation, authentication or encryption.
		secReqs.SetAuthorisation(EFalse);
		secReqs.SetAuthentication(EFalse);
		secReqs.SetEncryption(EFalse);
		}
	// As a note, using the override as-is is infact safe as none of the override values 
	// can really affect the security mode 4 aspects.

	TRAPD(err, SecMan().AccessRequestL(secReqs,
						aSAPWithSecuritySettings.Override(RemoteAddress()),
						RemoteAddress(),
						aSecurityModeFourOutgoing ? EGeneralBondingSecurityMode4Outgoing : EGeneralBonding, // We are doing general bonding
						aSAPWithSecuritySettings.iNoSecurityRequired,
						const_cast<MAccessRequestResponseHandler&>(handler)));

	if (err != KErrNone)
		{
		// complete request now...
		iAccessDeniedCallBack->CallBack();
		}
	}

void CBluetoothSAP::CancelAccessRequest()
	{
	SecMan().CancelRequest(*this);
	}

void CBluetoothSAP::AccessRequestComplete(TInt /*aResult*/)
/**
	The SAP wasnt expecting security responses
	Derivers normally would override (and forward to state)
**/
	{
	_LIT(KBTSAPPanic, "BTSAP");
	User::Panic(KBTSAPPanic, EBTSecUnexpectedSecurityResponse);
	}

/*static*/ TInt CBluetoothSAP::AccessDeniedCallBack(TAny* aBtSap)
	{
	CBluetoothSAP* btSap = static_cast<CBluetoothSAP*>(aBtSap);
	btSap->AccessRequestComplete(EBTSecManAccessDenied);
	return KErrNone;
	}


TInt CBluetoothSAP::SetCodServiceBits(TUint16 aNewCodServiceBits)
/**
	Allow SetOpts to set Service bits. These are written to the CoD
	when the SAP becomes active.
**/
	{
	TInt err = KErrNone;
	if (aNewCodServiceBits & (~KBTCodValidServiceBits))
		{
		err = KErrArgument;		// SetOpt is attempting to set illegal bits
		}
	else
		{
		if (iCodServiceBits & KBTCodBitsRegdFlag)
			{
			err = KErrAlreadyExists; // This SAP has Service bits already registered
			}
		else
			{
			iCodServiceBits = aNewCodServiceBits; // These can be overwritten at this stage
			}
		}
	return err;
	}

	
void CBluetoothSAP::RegisterCodService()
/**
	Allows SAPs to register their Service(s)
**/
	{
	if (iCodServiceBits && !(iCodServiceBits & KBTCodBitsRegdFlag))
		{
		// Only send to CodMan once
		CodMan().RegisterCodService(iCodServiceBits);
		}
// Set the regd flag, even if no service bits, this will ensure that late SetOpts are errored
	iCodServiceBits |= KBTCodBitsRegdFlag;
	}


void CBluetoothSAP::DeregisterCodService()
/**
	Allows SAPs to clear their Service bits
**/
	{
	if (iCodServiceBits && (iCodServiceBits & KBTCodBitsRegdFlag))
		{
		iCodServiceBits &= (~KBTCodBitsRegdFlag);	// reset the registered flag, don't want to confuse CodMan
		CodMan().RemoveCodService(iCodServiceBits);
		}
	iCodServiceBits = 0;	// tidy up these service bits + regd flag
	}


TInt CBluetoothSAP::SetOption(TUint aLevel,TUint aName,const TDesC8 &aOption)
/**
	This is now the default SetOpt handler for SAP protocols.
	Common attributes are processed here and the protocol dependent SetOpts are passed on.
**/
	{
	TInt rerr = KErrNone;
	if(aLevel == KSolBtSAPBase)
		{
		switch (aName)
			{
			case KBTRegisterCodService:
				{
				if (aOption.Length() != sizeof(TDesC8))
					return KErrArgument;
				
		        TUint16 newServiceBits = *reinterpret_cast<const TUint16*>(aOption.Ptr());
				rerr = SetCodServiceBits(newServiceBits);	// The service bits are saved and then registered when SAP becomes live
				}
				break;	
			case KBTSetNoSecurityRequired:
				{
				__ASSERT_DEBUG(iSecurityChecker, User::Panic(KSECURITY_PANIC, EBTPanicNullSecurityChecker));
				rerr = iSecurityChecker->CheckPolicy(KSDP_SID_PROT_SERV, KBT_SAP_NAME_DIAG);
				if (rerr == KErrNone)
					{
					iNoSecurityRequired = *reinterpret_cast<const TBool*>(aOption.Ptr());
					}
				}
				break;
			case KBTSecurityDeviceOverride:
				{
				rerr = SetDeviceOverride(aOption); 
				}
				break;
			default:
				// Unhandled SetOpt name
				rerr = KErrNotSupported;
				break;
			}
		}
	else
		{
		// Process SAP SetOpts
		rerr = SAPSetOption(aLevel, aName, aOption);
		}
		
	return rerr;
	}
