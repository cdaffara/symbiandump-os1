// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
 
*/

#include "ULP.h"
#include "suplresponse.h"
#include "supldevloggermacros.h" 

/** 
Static factory constructor 
*/
EXPORT_C CSuplResponse* CSuplResponse::NewL()
	{
	SUPLLOG(ELogP1, "CSuplResponse::NewL() Begin\n");
	CSuplResponse* self = new (ELeave) CSuplResponse();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplResponse::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}
		
/** 
Constructor 
*/
CSuplResponse::CSuplResponse()
 : CSuplMessageBase::CSuplMessageBase(ESuplResponse, EFalse)
	{
	}

/** 
Second stage constructor 
*/
void CSuplResponse::ConstructL()
	{
	// call the base class ConstructL() to create common data structures
	CSuplMessageBase::ConstructL();
	}


/** 
Destructor
*/
CSuplResponse::~CSuplResponse()
	{
	SUPLLOG(ELogP1, "CSuplResponse::~CSuplResponse() Begin\n");
	SUPLLOG(ELogP1, "CSuplResponse::~CSuplResponse() End\n");
	}
	
	
/**
GetPosMethod()

The Positioning Method parameter of the SUPL RESPONSE message is the method
that SHALL be used for the SUPL POS session.

@param  aMethod on return, populated according to Pos Method parameter
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplResponse::GetPosMethod(TLbsNetPosRequestMethod& aMethod)
	{
	SUPLLOG(ELogP1, "CSuplResponse::GetPosMethod() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLRESPONSE != NULL, User::Invariant());
	
	// pointer to message body
	ASN1T_SUPLRESPONSE* msgBody = iData->message.u.msSUPLRESPONSE;
	
	TLbsNetPosMethod posMethods[2];
	TInt numMethods = 1;
	
	switch (msgBody->posMethod)
		{
		case PosMethod::agpsSETassisted:
			posMethods[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
			break;

		case PosMethod::agpsSETbased:
			posMethods[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
			break;

		case PosMethod::agpsSETassistedpref:
			posMethods[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
			posMethods[1].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);		
			++numMethods;
			break;

		case PosMethod::PosMethod::agpsSETbasedpref:
			posMethods[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
			posMethods[1].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
			++numMethods;
			break;

		case PosMethod::autonomousGPS:
			posMethods[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal);
			break;

		case PosMethod::aFLT:
			posMethods[0].SetPosMethod(KLbsPositioningMeansAflt, TPositionModuleInfo::ETechnologyTerminal);
			break;

		case PosMethod::eCID:
			posMethods[0].SetPosMethod(KLbsPositioningMeansCell, TPositionModuleInfo::ETechnologyTerminal);
			break;

		case PosMethod::PosMethod::eOTD:
			posMethods[0].SetPosMethod(KLbsPositioningMeansEotd, TPositionModuleInfo::ETechnologyTerminal);
			break;

		case PosMethod::oTDOA:
			posMethods[0].SetPosMethod(KLbsPositioningMeansOtdoa, TPositionModuleInfo::ETechnologyTerminal);
			break;

		case PosMethod::noPosition:
			posMethods[0].SetPosMethod(KLbsPositioningMeansNone, TPositionModuleInfo::ETechnologyTerminal);
			break;

		default:
			// error
			__ASSERT_DEBUG(EFalse, User::Invariant());
			return KErrArgument;
		}

	// populate the return parameter	
	TInt err = aMethod.SetPosMethods(posMethods, numMethods);

	SUPLLOG(ELogP1, "CSuplResponse::GetPosMethod() End\n");
	return err;
	}


/** 
SlpAddressPresent()

The SLP Address is conditionally present - it is required if the SUPL Session 
is operating in non-Proxy mode, otherwise it is not required.

@return ETrue if the SlpAddress parameter is present 
*/
EXPORT_C TBool CSuplResponse::SlpAddressPresent()
	{
	SUPLLOG(ELogP1, "CSuplResponse::SlpAddressPresent() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLRESPONSE != NULL, User::Invariant());
	
	if (iData->message.u.msSUPLRESPONSE->m.sLPAddressPresent == 0)
		{
		SUPLLOG(ELogP1, "CSuplResponse::SlpAddressPresent(EFalse) End\n");
		return EFalse;
		}

	SUPLLOG(ELogP1, "CSuplResponse::SlpAddressPresent(ETrue) End\n");
	return ETrue;
	}


/** 
SlpAddress()

@param  aAddress, populated according to the SLP address parameter
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplResponse::GetSlpAddress(CSuplSlpAddress& aAddress)
	{
	SUPLLOG(ELogP1, "CSuplResponse::SlpAddress() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLRESPONSE != NULL, User::Invariant());
	__ASSERT_DEBUG(iData->message.u.msSUPLRESPONSE->m.sLPAddressPresent != 0, User::Invariant());

	// pointer to message body
	ASN1T_SUPLRESPONSE* msgBody = iData->message.u.msSUPLRESPONSE;
		
	if (msgBody->m.sLPAddressPresent == 0)
		{
		SUPLLOG(ELogP1, "CSuplResponse::SlpAddress() End (SLP Address element not present)\n");
		return KErrNotFound;
		}
	
	if (msgBody->sLPAddress.t == T_SLPAddress_fQDN)
		{
		// FQDN is a NULL terminated string, length 1..255
		aAddress.iSlpAddressType = ESuplSlpAddressTypeFqdn;
		
		// find the length of the FQDN (NULL terminated)
		const TUint8* tmp = (const TUint8*)msgBody->sLPAddress.u.fQDN;
		TPtrC8 source = TPtrC8(tmp, 256);
		_LIT8(KNull,"\0");
		TInt fqdnLength = source.Find(KNull);
		source.Set(tmp, fqdnLength);

		// copy to the container
		TBuf8<256>& fqdn = aAddress.iFqdn->iFqdn;
		fqdn.Copy(source);
		fqdn.SetLength(fqdnLength);
		}

	else if (msgBody->sLPAddress.t == T_SLPAddress_iPAddress)
		{
		// SLP ID is an IP Address
		aAddress.iSlpAddressType = ESuplSlpAddressTypeIp;

		// Reference to the address data buffer
		TBuf8<16>& ipAddress = aAddress.iIpAddress->iIpAddress;
						
		// IPv4 or IPv6 address?
		if (msgBody->sLPAddress.u.iPAddress->t == T_IPAddress_ipv6Address)
			{
			aAddress.iIpAddress->iIpAddressType = ESuplIpAddressTypeV6;
			TInt len = msgBody->sLPAddress.u.iPAddress->u.ipv6Address->numocts;
			TUint8* data = msgBody->sLPAddress.u.iPAddress->u.ipv6Address->data;
			ipAddress.Copy(data, len);
			}
		else
			{
			aAddress.iIpAddress->iIpAddressType = ESuplIpAddressTypeV4;
			TInt len = msgBody->sLPAddress.u.iPAddress->u.ipv4Address->numocts;
			TUint8* data = msgBody->sLPAddress.u.iPAddress->u.ipv4Address->data;
			ipAddress.Copy(data, len);
			}
		}
	else
		{
		// error
		__ASSERT_DEBUG(0, User::Invariant());
		return KErrArgument;
		}
	
	SUPLLOG(ELogP1, "CSuplResponse::SlpAddress() End\n");
	return KErrNone;
	}

