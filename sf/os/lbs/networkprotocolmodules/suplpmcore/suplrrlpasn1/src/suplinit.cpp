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

#include "suplmessagebase.h"
#include "ULP.h"
#include "suplinit.h"
#include "supldevloggermacros.h" 
#include <lbs/lbslocclasstypes.h>
#include <hash.h>

/**
Static factory constructor
*/
EXPORT_C CSuplInit* CSuplInit::NewL()
	{
	SUPLLOG(ELogP1, "CSuplInit::NewL() Begin\n");
	CSuplInit* self = new (ELeave) CSuplInit();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplInit::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}
	
/** 
Constructor 
*/
CSuplInit::CSuplInit()
 : CSuplMessageBase::CSuplMessageBase(ESuplInit, EFalse)
	{
	}

/** 
Second stage constructor
*/
void CSuplInit::ConstructL()
	{
	CSuplMessageBase::ConstructL();
	}

/**
Destructor
*/	
CSuplInit::~CSuplInit()
	{
	SUPLLOG(ELogP1, "CSuplInit::~CSuplInit() Begin\n");
	delete iMessage;
	SUPLLOG(ELogP1, "CSuplInit::~CSuplInit() End\n");
	}

/** 
GetPosMethod() 

Populates aMethod according to SUPL INIT positioning method parameter 

The Positioning Method parameter of the SUPL INIT message is the method
desired by the SLP for the SUPL POS session.

@param aMethod, on return populated as per the positioning method element
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplInit::GetPosMethod(TLbsNetPosRequestMethod& aMethod)
	{
	__ASSERT_DEBUG(iData->message.u.msSUPLINIT != NULL, User::Invariant());

	SUPLLOG(ELogP1, "CSuplInit::GetPosMethod() Begin\n");
	
	// retrieve the posMethod value
	ASN1T_PosMethod posMethod = iData->message.u.msSUPLINIT->posMethod;
	
	TLbsNetPosMethod posMethods[2];
	TInt numMethods = 1;
	
	switch (posMethod)
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
			SUPLLOG(ELogP1, "CSuplInit::GetPosMethod() Error - invalid argument\n");
			return KErrArgument;
		}

	// populate the return parameter	
	TInt err = aMethod.SetPosMethods(posMethods, numMethods);

	SUPLLOG(ELogP1, "CSuplInit::GetPosMethod() End\n");
	return err;
	}
	

/** 
NotificationPresent()

@return ETrue if optional notification parameter is present, EFalse otherwise
*/
EXPORT_C TBool CSuplInit::NotificationPresent()
	{
	SUPLLOG(ELogP1, "CSuplInit::NotificationPresent() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLINIT != NULL, User::Invariant());
	
	if (iData->message.u.msSUPLINIT->m.notificationPresent != 0)
		{
		SUPLLOG(ELogP1, "CSuplInit::NotificationPresent(ETrue) End\n");
		return ETrue;
		}
	SUPLLOG(ELogP1, "CSuplInit::NotificationPresent(EFalse) End\n");
	return EFalse;
	}
	
/** 
NotificationType()

Populates aPrivacy according to SUPL INIT notification element.

If the notification element is not present, this is interpreted as "no 
notification and no verification", as per the SUPL spec.

@param  aPrivacy, populated according to the notification element
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplInit::GetNotificationType(TLbsNetPosRequestPrivacy& aPrivacy)
	{
	__ASSERT_DEBUG(iData->message.u.msSUPLINIT != NULL, User::Invariant());
	SUPLLOG(ELogP1, "CSuplInit::NotificationType() Begin\n");

	if (iData->message.u.msSUPLINIT->m.notificationPresent == 0)
		{
		aPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent);
		aPrivacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
		}
	else
		{
		ASN1T_Notification& notification = iData->message.u.msSUPLINIT->notification;
		
		// populate privacy request according to notification type.
		switch (notification.notificationType)
			{
			case NotificationType::noNotificationNoVerification:
				{
				aPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceSilent);
				aPrivacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
				break;
				}
			case NotificationType::notificationOnly:
				{
				aPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceNotify);
				aPrivacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
				break;
				}
			case NotificationType::notificationAndVerficationAllowedNA:
				{
				aPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceVerify);
				aPrivacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
				break;
				}
			case NotificationType::notificationAndVerficationDeniedNA:
				{
				aPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceVerify);
				aPrivacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionReject);
				break;
				}
			case NotificationType::privacyOverride:
				{
				// this is option is not implemented by the LBS subsystem
				aPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceStealth);
				aPrivacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
				break;
				}
			default:
				{
				// error
				__ASSERT_DEBUG(EFalse, User::Invariant());
				SUPLLOG(ELogP1, "CSuplInit::NotificationType() Error - invalid notification type\n");
				return KErrNotFound;
				}
			}
		}

	SUPLLOG(ELogP1, "CSuplInit::NotificationType() End\n");
	return KErrNone;
	}
		
/** 
ExternalRequestInfoPresent()

@return ETrue if optional requestor id and/or client name subparameter of the 
		notification element is present 
		EFalse otherwise
*/
EXPORT_C TBool CSuplInit::ExternalRequestInfoPresent()
	{
	SUPLLOG(ELogP1, "CSuplInit::ExternalRequestInfoPresent() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLINIT != NULL, User::Invariant());
	
	if (iData->message.u.msSUPLINIT->m.notificationPresent != 0)
		{
		if (iData->message.u.msSUPLINIT->notification.m.requestorIdPresent != 0 ||
		    iData->message.u.msSUPLINIT->notification.m.clientNamePresent  != 0 )
			{
			SUPLLOG(ELogP1, "CSuplInit::ExternalRequestInfoPresent(ETrue) End\n");
			return ETrue;
			}
		}
	SUPLLOG(ELogP1, "CSuplInit::ExternalRequestInfoPresent(EFalse) End\n");
	return EFalse;
	}


/** 
ExternalRequestInfo()

Populates aRequestInfo according to SUPL INIT requestor id and/or client name 
sub-elements of the notification element.

@param  aRequestInfo, populated according to received notification element
@return KErrNotFound if the notification element is not present
		KErrNone     otherwise
*/
EXPORT_C TInt CSuplInit::GetExternalRequestInfo(TLbsExternalRequestInfo& aRequestInfo)
	{
	__ASSERT_DEBUG(iData->message.u.msSUPLINIT != NULL, User::Invariant());
	SUPLLOG(ELogP1, "CSuplInit::ExternalRequestInfo() Begin\n");

	if (iData->message.u.msSUPLINIT->m.notificationPresent != 0)
		{
		ASN1T_Notification& notification = iData->message.u.msSUPLINIT->notification;

		// process requestor ID if it is present
		if (notification.m.requestorIdPresent != 0 && notification.m.requestorIdTypePresent != 0)
			{
			// requestor type
			switch (notification.requestorIdType)
				{
				case FormatIndicator::logicalName:
					{
					aRequestInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatLogicalName);
					break;
					}
      			case FormatIndicator::e_mailAddress:
					{
					aRequestInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatEmailAddress);
					break;
					}
      			case FormatIndicator::msisdn:
					{
					aRequestInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatMSISDN);
					break;
					}
      			case FormatIndicator::url:
					{
					aRequestInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatURL);
					break;
					}
      			case FormatIndicator::sipUrl:
					{
					aRequestInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatSIPURL);
					break;
					}
      			case FormatIndicator::min:
					{
					aRequestInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatMIN);
					break;
					}
      			case FormatIndicator::mdn:
					{
					aRequestInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatMDN);
					break;
					}
				default:
					{
					// error
					__ASSERT_DEBUG(EFalse, User::Invariant());
					aRequestInfo.SetRequesterIdFormat(TLbsExternalRequestInfo::EFormatUnknown);
					break;
					}
				}
			
			// requestor id
			ASN1T_Notification_requestorId& requestorId = notification.requestorId;
			TPtrC8 ptr(requestorId.data, requestorId.numocts);
			aRequestInfo.SetRequesterId(ptr);
			
			// requestor encoding type
			switch (notification.encodingType)
				{
				case EncodingType::ucs2:
					{
					aRequestInfo.SetRequesterIdCodingScheme(TLbsExternalRequestInfo::ECodingSchemeUCS2);
					break;
					}
      			case EncodingType::gsmDefault:
      				{
      				aRequestInfo.SetRequesterIdCodingScheme(TLbsExternalRequestInfo::ECodingSchemeGSMDefault);
      				break;
      				}
      			case EncodingType::utf8:
      				{
            		aRequestInfo.SetRequesterIdCodingScheme(TLbsExternalRequestInfo::ECodingSchemeUTF8);
            		break;
            		}
      			default:
      				{
					// error
					__ASSERT_DEBUG(EFalse, User::Invariant());
      				aRequestInfo.SetRequesterIdCodingScheme(TLbsExternalRequestInfo::ECodingSchemeUnknown);
      				break;
      				}
				}
			}

		// process Client Name if it is present
		if (notification.m.clientNamePresent != 0 && notification.m.clientNameTypePresent != 0)
			{
			// client name type
			switch (notification.clientNameType)
				{
				case FormatIndicator::logicalName:
					{
					aRequestInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatLogicalName);
					break;
					}
      			case FormatIndicator::e_mailAddress:
					{
					aRequestInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatEmailAddress);
					break;
					}
      			case FormatIndicator::msisdn:
					{
					aRequestInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatMSISDN);
					break;
					}
      			case FormatIndicator::url:
					{
					aRequestInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatURL);
					break;
					}
      			case FormatIndicator::sipUrl:
					{
					aRequestInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatSIPURL);
					break;
					}
      			case FormatIndicator::min:
					{
					aRequestInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatMIN);
					break;
					}
      			case FormatIndicator::mdn:
					{
					aRequestInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatMDN);
					break;
					}
				default:
					{
					// error
					__ASSERT_DEBUG(EFalse, User::Invariant());
					aRequestInfo.SetClientNameFormat(TLbsExternalRequestInfo::EFormatUnknown);
					break;
					}
				}
			
			// client name
			ASN1T_Notification_clientName& clientName = notification.clientName;
			TPtrC8 ptr(clientName.data, clientName.numocts);
			aRequestInfo.SetClientName(ptr);
			
			// client name encoding type
			switch (notification.encodingType)
				{
				case EncodingType::ucs2:
					{
					aRequestInfo.SetClientNameCodingScheme(TLbsExternalRequestInfo::ECodingSchemeUCS2);
					break;
					}
      			case EncodingType::gsmDefault:
      				{
      				aRequestInfo.SetClientNameCodingScheme(TLbsExternalRequestInfo::ECodingSchemeGSMDefault);
      				break;
      				}
      			case EncodingType::utf8:
      				{
            		aRequestInfo.SetClientNameCodingScheme(TLbsExternalRequestInfo::ECodingSchemeUTF8);
            		break;
            		}
      			default:
      				{
					// error
					__ASSERT_DEBUG(EFalse, User::Invariant());
      				aRequestInfo.SetClientNameCodingScheme(TLbsExternalRequestInfo::ECodingSchemeUnknown);
      				break;
      				}
				}
			}
		
		SUPLLOG(ELogP1, "CSuplInit::ExternalRequestInfo() End\n");
		return KErrNone;
		}
	
	// notification element not present.	
	SUPLLOG(ELogP1, "CSuplInit::ExternalRequestInfo() End (Notification element not present)\n");
	return KErrNotFound;
	}


/** 
SlpAddressPresent()

The SLP address is mandatory for non-Proxy mode operation, for proxy mode
operation the paramater is optional.

@return ETrue if optional SLP address parameter is present, EFalse otherwise
*/
EXPORT_C TBool CSuplInit::SlpAddressPresent()
	{
	SUPLLOG(ELogP1, "CSuplInit::SlpAddressPresent() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLINIT != NULL, User::Invariant());
	
	if (iData->message.u.msSUPLINIT->m.sLPAddressPresent != 0)
		{
		SUPLLOG(ELogP1, "CSuplInit::SlpAddressPresent(ETrue) End\n");
		return ETrue;
		}
	
	SUPLLOG(ELogP1, "CSuplInit::SlpAddressPresent(EFalse) End\n");
	return EFalse;
	}


/** 
SlpAddress()

Populates aAddress according to SUPL INIT SLP address parameter 

The SLP address is mandatory for non-Proxy mode operation, for proxy mode
operation the paramater is optional.

@param aAddress, populated according to the SLP Address Parameter
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplInit::GetSlpAddress(CSuplSlpAddress& aAddress)
	{
	SUPLLOG(ELogP1, "CSuplInit::SlpAddress() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLINIT != NULL, User::Invariant());
	
	if (iData->message.u.msSUPLINIT->m.sLPAddressPresent == 0)
		{
		SUPLLOG(ELogP1, "CSuplInit::SlpAddress() End (SLP Address Not Present)\n");
		return KErrNotFound;
		}
	
	ASN1T_SLPAddress& slpAddress = iData->message.u.msSUPLINIT->sLPAddress;
	
	switch (slpAddress.t)
		{
		case T_SLPAddress_iPAddress:
			{
			// SLP ID is an IP Address
			aAddress.iSlpAddressType = ESuplSlpAddressTypeIp;

			// Pointer to the address data buffer
			TBuf8<16>& ipAddress = aAddress.iIpAddress->iIpAddress;
							
			// IPv4 or IPv6 address?
			if (slpAddress.u.iPAddress->t == T_IPAddress_ipv6Address)
				{
				aAddress.iIpAddress->iIpAddressType = ESuplIpAddressTypeV6;
				TInt len = slpAddress.u.iPAddress->u.ipv6Address->numocts;
				TUint8* data = slpAddress.u.iPAddress->u.ipv6Address->data;
				ipAddress.Copy(data, len);
				}
			else
				{
				aAddress.iIpAddress->iIpAddressType = ESuplIpAddressTypeV4;
				TInt len = slpAddress.u.iPAddress->u.ipv4Address->numocts;
				TUint8* data = slpAddress.u.iPAddress->u.ipv4Address->data;
				ipAddress.Copy(data, len);
				}
			break;
			}
		case T_SLPAddress_fQDN:
			{
			// FQDN is a NULL terminated string, length 1..255
			aAddress.iSlpAddressType = ESuplSlpAddressTypeFqdn;
						
			// find the length of the FQDN (NULL terminated)
			const TUint8* tmp = (const TUint8*)slpAddress.u.fQDN;
			TPtrC8 source = TPtrC8(tmp, 256);
			_LIT8(KNull,"\0");
			TInt fqdnLength = source.Find(KNull);
			
			if (fqdnLength > 0 && fqdnLength <256)
				{
				// copy to the container
				source.Set(tmp, fqdnLength);
				TBuf8<256>& fqdn = aAddress.iFqdn->iFqdn;
				fqdn.Copy(source);
				fqdn.SetLength(fqdnLength);
				}
			else
				{
				// fqdn length is corrupt
				__ASSERT_DEBUG(0, User::Invariant());
				return KErrCorrupt;
				}
			break;
			}
		case T_SLPAddress_extElem1:
		default:
			{
			// error
			__ASSERT_DEBUG(EFalse, User::Invariant());
			return KErrUnknown;
			}
		}
		
	SUPLLOG(ELogP1, "CSuplInit::SlpAddress() End\n");
	return KErrNone;
	}

/** 
QopPresent()

@return ETrue if optional QoP parameter is present, EFalse otherwise
*/
EXPORT_C TBool CSuplInit::QopPresent()
	{
	SUPLLOG(ELogP1, "CSuplInit::QopPresent() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLINIT != NULL, User::Invariant());
	
	if (iData->message.u.msSUPLINIT->m.qoPPresent != 0)
		{
		SUPLLOG(ELogP1, "CSuplInit::QopPresent(ETrue) End\n");
		return ETrue;
		}

	SUPLLOG(ELogP1, "CSuplInit::QopPresent(EFalse) End\n");
	return EFalse;
	}

	
/**
Qop()

Populates aAddress according to SUPL INIT QoP parameter

@param aQuality, populated according to QoP parameter
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplInit::GetQop (TLbsNetPosRequestQuality& aQuality)
	{
	SUPLLOG(ELogP1, "CSuplInit::Qop() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLINIT != NULL, User::Invariant());
	
	if (iData->message.u.msSUPLINIT->m.qoPPresent == 0)
		{
		SUPLLOG(ELogP1, "CSuplInit::Qop() End (QoP element not present)\n");
		return KErrNotFound;
		}
	
	ASN1T_QoP& qop = iData->message.u.msSUPLINIT->qoP;
	
	// horizontal accuracy
	// convert to meters according to 3GPP TS 23.032
	TReal temp ;
	Math::Pow(temp, 1.1, qop.horacc);
	TReal accuracy = 10 * (temp - 1);
	aQuality.SetMinHorizontalAccuracy(accuracy);
	
	// vertical accuracy
	if (qop.m.veraccPresent)
		{
		// convert to meters according to 3GPP TS 23.032
		Math::Pow(temp, 1.025, qop.veracc);
		TReal verAccuracy = 45 * (temp - 1);
		aQuality.SetMinVerticalAccuracy(verAccuracy);
		}
	else
		{
		// Set vertical accuracy to NaN
		TRealX nan;
		nan.SetNaN();
		aQuality.SetMinVerticalAccuracy(nan);	
		}
	
	// max location age (max fix age)
	if (qop.m.maxLocAgePresent)
		{
		// convert from seconds to microseconds
		TTimeIntervalMicroSeconds maxLocAge(qop.maxLocAge * 1000 * 1000);
		aQuality.SetMaxFixAge(maxLocAge);	
		}

	// delay (max fix time)
	if (qop.m.delayPresent)
		{
		Math::Pow(temp, 2, qop.delay);
		TTimeIntervalMicroSeconds maxFixTime(temp * 1000 * 1000);
		aQuality.SetMaxFixTime(maxFixTime);
		}

	SUPLLOG(ELogP1, "CSuplInit::Qop() End\n");
	return KErrNone;
	}

/** 
SlpMode()

Returns the SLP mode specified in the received SUPL INIT message. 
Note that this implementation of the SUPL ASN1 does not include full support
of non-proxy operation.

@return the mode of operation supported by the SLP, either proxy or non-proxy.
*/
EXPORT_C CSuplInit::TSuplSlpMode CSuplInit::SlpMode()
	{
	SUPLLOG(ELogP1, "CSuplInit::SlpMode() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLINIT != NULL, User::Invariant());
	
	if (iData->message.u.msSUPLINIT->sLPMode == SLPMode::proxy)
		{
		SUPLLOG(ELogP1, "CSuplInit::SlpMode(EProxyMode) End\n");
		return CSuplInit::ESuplSlpModeProxy;
		}
	
	SUPLLOG(ELogP1, "CSuplInit::SlpMode(ENonProxyMode) End\n");
	return CSuplInit::ESuplSlpModeNonProxy;
	}

/**
SetReceivedMessageL()

Stores a copy of the unencoded received message data. This is required in the
calculation of the hash key for SUPL INIT authorisation.

@param  aData - descriptor containing the received SUPL INIT message
@leave  KErrNoMemory or other error codes
*/
void CSuplInit::SetReceivedMessageL(const TDesC8& aData)
	{
	SUPLLOG(ELogP1, "CSuplInit::SetReceivedMessageL() Begin\n");
	delete iMessage;
	iMessage = NULL;
	iMessage = HBufC8::NewL(aData.Length());
	*iMessage = aData;
	SUPLLOG(ELogP1, "CSuplInit::SetReceivedMessageL() End\n");
	}


/** 
GetVerL()

Returns the encoded hash of the received SUPL INIT message.
VER=H(H-SLP XOR opad, H(H-SLP XOR ipad, SUPL INIT))

@param  aHSlp - the provisioned H-SLP address
@return descriptor pointer to the encoded hash of the received message
*/
EXPORT_C TPtrC8 CSuplInit::GetVerL(const TDesC8 &aHSlp)
	{
	SUPLLOG(ELogP1, "CSuplInit::GetVerL() Begin\n");
	CMessageDigest *sha1  = CSHA1::NewL();
	CleanupStack::PushL(sha1);
	CHMAC *hmac = CHMAC::NewL(aHSlp, sha1);
	// If construction works, the CHMAC takes ownership of the sha1 object
	CleanupStack::Pop(sha1);
	CleanupStack::PushL(hmac);

	TPtrC8 hmac_value = hmac->Final(iMessage->Des());

	// store the hmac.
	iVer.Copy(hmac_value.Left(8));
	iVer.SetLength(8);	
	
	CleanupStack::PopAndDestroy(hmac);

	// return a pointer to it.
	TPtrC8 ver(iVer.Ptr(), 8);

	SUPLLOG(ELogP1, "CSuplInit::GetVerL() End\n");
	return ver;
	}


