// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ir_sock.h>

#include <irdaextensions.h>
#include <e32base.h>
#include <es_prot.h>
#include <c32comm.h>
#include "IR_CONST.H"

EXPORT_C TIrdaSockAddr::TIrdaSockAddr() : TSockAddr()
/** Default constructor. Sets the protocol family to the value of the constant 
KIrdaAddrFamily. */
	{
	SetFamily(KIrdaAddrFamily);
	SetUserLen(sizeof(SIrdaAddr));
	addrPtr()->iSpare = 0;
	}

//
// Copy constructor - takes a previously filled TSockAddr and 
// parses it for result fields.  Useful for client code only.
//
EXPORT_C TIrdaSockAddr::TIrdaSockAddr(const TSockAddr &aAddr)
/** Copy constructor. Assumes that the source address is an IrDA socket address.

@param aAddr A socket address assumed to be valid IrDA socket address. */
	{
	SetFamily(KIrdaAddrFamily);
	SetPort(aAddr.Port());
	SetRemoteDevAddr(TIrdaSockAddr::Cast(aAddr).GetRemoteDevAddr());
	SetHostDevAddr(TIrdaSockAddr::Cast(aAddr).GetHostDevAddr());
	SetSniffStatus(TIrdaSockAddr::Cast(aAddr).GetSniffStatus());
	SetSolicitedStatus(TIrdaSockAddr::Cast(aAddr).GetSolicitedStatus());
	SetIrlapVersion(TIrdaSockAddr::Cast(aAddr).GetIrlapVersion());
	SetCharacterSet(TIrdaSockAddr::Cast(aAddr).GetCharacterSet());
	SetFirstServiceHintByte(TIrdaSockAddr::Cast(aAddr).GetFirstServiceHintByte());
	SetSecondServiceHintByte(TIrdaSockAddr::Cast(aAddr).GetSecondServiceHintByte());
    SetServiceHintByteCount(TIrdaSockAddr::Cast(aAddr).GetServiceHintByteCount());
	SetHomePort(TIrdaSockAddr::Cast(aAddr).GetHomePort());
	SetRemotePort(TIrdaSockAddr::Cast(aAddr).GetRemotePort());
	
	SetUserLen(sizeof(SIrdaAddr));
	}

EXPORT_C TIrdaSockAddr &TIrdaSockAddr::Cast(const TSockAddr &aAddr)
	{
	return *((TIrdaSockAddr *)&aAddr);
	}

EXPORT_C TIrdaSockAddr &TIrdaSockAddr::Cast(const TSockAddr *aAddr)
	{
	return *((TIrdaSockAddr *)aAddr);
	}

EXPORT_C TUint TIrdaSockAddr::GetRemoteDevAddr() const
	{
	return addrPtr()->iRemoteDevAddr;
	}

EXPORT_C void TIrdaSockAddr::SetRemoteDevAddr(const TUint aRemote)
	{
	addrPtr()->iRemoteDevAddr=aRemote;
	}

EXPORT_C TUint TIrdaSockAddr::GetHostDevAddr() const
	{
	return addrPtr()->iHostDevAddr;
	}

EXPORT_C void TIrdaSockAddr::SetHostDevAddr(const TUint aHost)
	{
	addrPtr()->iHostDevAddr=aHost;
	}

EXPORT_C TBool TIrdaSockAddr::GetSniffStatus() const
	{
	return addrPtr()->iSniff;
	}

EXPORT_C void TIrdaSockAddr::SetSniffStatus(const TBool aSniffStatus)
	{
	addrPtr()->iSniff=aSniffStatus;
	}

EXPORT_C TBool TIrdaSockAddr::GetSolicitedStatus() const
	{
	return addrPtr()->iSolicited;
	}

EXPORT_C void TIrdaSockAddr::SetSolicitedStatus(const TBool aSolicitedStatus)
	{
	addrPtr()->iSolicited=aSolicitedStatus;
	}

EXPORT_C TUint8 TIrdaSockAddr::GetIrlapVersion() const
	{
	return addrPtr()->iIrlapVersion;
	}

EXPORT_C void TIrdaSockAddr::SetIrlapVersion(const TUint8 aIrlapVersion)
	{
	addrPtr()->iIrlapVersion=aIrlapVersion;
	}

EXPORT_C TUint8 TIrdaSockAddr::GetCharacterSet() const
	{
	return addrPtr()->iCharacterSet;
	}

EXPORT_C void TIrdaSockAddr::SetCharacterSet(const TUint8 aCharacterSet)
	{
	addrPtr()->iCharacterSet=aCharacterSet;
	}

EXPORT_C TUint8 TIrdaSockAddr::GetFirstServiceHintByte() const
	{
	return addrPtr()->iFirstServiceHintByte;
	}

EXPORT_C void TIrdaSockAddr::SetFirstServiceHintByte(const TUint8 aFirstServiceHintByte)
	{
	addrPtr()->iFirstServiceHintByte=aFirstServiceHintByte;
	}

EXPORT_C TUint8 TIrdaSockAddr::GetSecondServiceHintByte() const
	{
	return addrPtr()->iSecondServiceHintByte;
	}

EXPORT_C void TIrdaSockAddr::SetSecondServiceHintByte(const TUint8 aSecondServiceHintByte)
	{
	addrPtr()->iSecondServiceHintByte=aSecondServiceHintByte;
	}

EXPORT_C TUint8 TIrdaSockAddr::GetServiceHintByteCount() const
	{
	return addrPtr()->iServiceHintByteCount;
	}

EXPORT_C void TIrdaSockAddr::SetServiceHintByteCount(const TUint8 aServiceHintByteCount)
	{
	addrPtr()->iServiceHintByteCount=aServiceHintByteCount;
	}	

EXPORT_C TUint8 TIrdaSockAddr::GetHomePort() const 
	{
	return addrPtr()->iHomePort;
	}	

EXPORT_C void TIrdaSockAddr::SetHomePort(const TUint8 aHomePort)
	{
	addrPtr()->iHomePort=aHomePort;
	}	

EXPORT_C TUint8 TIrdaSockAddr::GetRemotePort() const
	{
	return addrPtr()->iRemotePort;
	}	

EXPORT_C void TIrdaSockAddr::SetRemotePort(const TUint8 aRemotePort)
	{
	addrPtr()->iRemotePort=aRemotePort;
	}	

TIrdaSockAddr::SIrdaAddr* TIrdaSockAddr::addrPtr() const
	{
	return (SIrdaAddr *)UserPtr();
	}
	
/**
Stops the IrDA stack responding to discovery requests from
another device.

@return KErrNone, if successful.
        KErrPermissionDenied, if the method is called without the
        required capabilities.

@capability WriteDeviceData
@publishedAll
@released
*/
EXPORT_C TInt RIrdaDiscoveryResponseSuppressor::Open()
	{
	return DefineAndSetKey(ETrue);
	}
	
/**
Stops the suppressor from blocking the sending of discovery responses
to remote IrDA devices.

@publishedAll
@released
*/
EXPORT_C void RIrdaDiscoveryResponseSuppressor::Close()
	{
	(void) DefineAndSetKey(EFalse); 
	}

/**
Private helper function that defines the IrDA discovery responses
suppression P&S key and sets the specified suppression status.

@param aSuppressDiscoveryResponses A boolean indicating if responses should be 
       suppressed or allowed.
       
@return KErrNone, if successful.
        KErrPermissionDenied, if the method is called without the
        required capabilities.

@internalComponent
@released
*/
TInt RIrdaDiscoveryResponseSuppressor::DefineAndSetKey(TBool aSuppressDiscoveryResponses)
	{
	// Define the discovery response suppression property.
	TSecurityPolicy policy = TSecurityPolicy(ECapabilityWriteDeviceData);
	TInt rerr = RProperty::Define(KIrdaPropertyCategory, 
	                              KIrdaDiscoveryResponseSuppressor, 
	                              RProperty::EInt, 
	                              policy, 
	                              policy);

	if(rerr == KErrAlreadyExists || rerr == KErrNone)
		{
		TDiscoveryResponseSuppressorStatus status = aSuppressDiscoveryResponses ? 
		                                            ESuppressDiscoveryResponses : 
		                                            EAllowDiscoveryResponses;

		rerr = RProperty::Set(KIrdaPropertyCategory, 
		                      KIrdaDiscoveryResponseSuppressor, 
		                      status);
		}
		
	// Forward the error if it is not KErrAlreadyExists.
	return rerr;
	}
	
// EOF
