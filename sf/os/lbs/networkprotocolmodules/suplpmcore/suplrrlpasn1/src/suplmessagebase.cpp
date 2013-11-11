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
#include "suplmessagebase.h"
#include "suplpos.h"
#include "supldevloggermacros.h" 
#include "suplrrlpasn1common.h"
#include <lbssatellite.h>
#include <lbs/lbsgpsmeasurement.h>
#include <lbs/lbsnetcommon.h>
#include <lbspositioninfo.h>
#include <lbsposition.h>
#include <reent.h>

/** 
Default constructor
*/
CSuplMessageBase::CSuplMessageBase(TSuplMessageType aType, TBool aIsOutgoingMessage)
 : iSuplMessageType(aType), iIsOutgoingMessage(aIsOutgoingMessage)
	{
	}
	
/** 
Destructor
*/
CSuplMessageBase::~CSuplMessageBase()
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::~CSuplMessageBase() Begin\n");
	delete iControl;
	delete iData;
	delete iEncodeBuffer;
	// release the STDLIB resources associated with this thread
	CloseSTDLIB();
	SUPLLOG(ELogP1, "CSuplMessageBase::~CSuplMessageBase() End\n");
	}
	
/** 
Second stage constructor 

Outgoing messages: constructs the data encapsulation and control objects.
Incoming message: no action
*/
void CSuplMessageBase::ConstructL()
	{
	if (iIsOutgoingMessage)
		{
		iData    = new (ELeave) ASN1T_ULP_PDU();
		ASN1Context* context = new (ELeave) ASN1Context;
		CleanupDeletePushL(context);
		iControl = new (ELeave) ASN1C_ULP_PDU(*context, *iData);
		// construction of iControl successful, pop context off the cleanup stack
		CleanupStack::Pop(context);

		// set the version parameter of the outgoing message
		iData->version.maj = 1;
		iData->version.min = 0;
		iData->version.servind = 0;
		}
	}
	

/** 
EncodeToL()

Encode a populated outgoing message to the specified buffer.

@param  aBuf buffer pointer 
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplMessageBase::EncodeToL(TPtr8& aBuf)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::EncodeToL() Begin\n");
	__ASSERT_DEBUG(iIsOutgoingMessage, User::Invariant());

	TInt retval = KErrNone;
	
	// buffer pointer, fixed max length
	TUint8* msgBuf = (TUint8*)aBuf.Ptr();
	TInt maxLength = aBuf.MaxLength();
	
	// Log the un-encoded SUPL values
	SUPLLOG(ELogP9, "-> ENCODING SUPL MESSAGE FOR SENDING\n");
	//LogMessageContent();
	
	// if the message is a SUPL POS, encode the payload
	if (iData->message.t == T_UlpMessage_msSUPLPOS)
		{
		CSuplPos* suplPos = reinterpret_cast <CSuplPos*>(this);
		retval = suplPos->EncodePosPayloadL();
		if (retval != KErrNone)
			{
			SUPLLOG(ELogP1, "CSuplMessageBase::EncodeToL() Error encoding Positioning Payload\n");
			return retval;
			}
		}
		
	// construct the encode buffer control object
	iEncodeBuffer = new (ELeave) ASN1PEREncodeBuffer (msgBuf, (unsigned int)maxLength, FALSE, (OSRTContext*)iControl->getContext());
	
	// Encode the message to the buffer
	TInt stat = iControl->EncodeTo(*iEncodeBuffer);

	if (stat == 0)
		{
		// Set the length according to reported buffer length
		TInt len = iEncodeBuffer->getMsgLen ();
		iData->length = len;
		aBuf.SetLength(len);
		
		// clear the encoded length field
		msgBuf[0] = 0;
		msgBuf[1] = 0;

		// set the encoded length field
      	msgBuf[0] |= (TUint8)( len >> 8 );
		msgBuf[1] |= (TUint8)( len );
		}
	else
		{
		retval = ProcessAsn1Error(stat);
		}
	
	// finished with encode buffer object
	delete iEncodeBuffer;
	iEncodeBuffer = NULL;

	// Log the encoded ASN1	
	SUPLLOG(ELogP8, "-> ENCODED SUPL MESSAGE FOR SENDING (HEX)\n");
	SUPLLOGHEX(ELogP8, aBuf.Ptr(), aBuf.Length());

	SUPLLOG2(ELogP1, "CSuplMessageBase::EncodeToL() End (retval = %d)\n", retval);
	return retval;
	}

/** 
SetDecodedData()

Assign decoded ASN1 data, for received messages.
Takes ownership of passed objects.

@param aData    data structure containing decoded message parameters
@param aControl control structure associated with decoded data structure.
*/
void CSuplMessageBase::SetDecodedData(ASN1T_ULP_PDU* aData, ASN1C_ULP_PDU* aControl)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::SetDecodedData() Begin\n");
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	iData = aData;
	iControl = aControl;
	SUPLLOG(ELogP1, "CSuplMessageBase::SetDecodedData() End\n");
	}


/** 
SetVersion()

Set the SUPL version used 
@param aVersion SUPL version in use
*/
EXPORT_C void CSuplMessageBase::SetVersion(CSuplVersion& aVersion)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::SetVersion() Begin\n");
	__ASSERT_DEBUG(iIsOutgoingMessage, User::Invariant());
	
	SUPLLOG2(ELogP1, "    - TInt iMaj = %d\n", aVersion.iMaj);
	SUPLLOG2(ELogP1, "    - TInt iMin = %d\n", aVersion.iMin);
	SUPLLOG2(ELogP1, "    - TInt iServind = %d\n", aVersion.iServind);
	
	iData->version.maj = aVersion.iMaj;
	iData->version.min = aVersion.iMin;
	iData->version.servind =  aVersion.iServind;
	SUPLLOG(ELogP1, "CSuplMessageBase::SetVersion() End\n");
	}
	

/** 
SetSessionId()

Set the SUPL Session ID
@param aSessionId session identifier
*/
EXPORT_C TInt CSuplMessageBase::SetSessionId(CSuplSessionId& aSessionId)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() Begin\n");
	__ASSERT_DEBUG(iIsOutgoingMessage, User::Invariant());
	
	// local reference to context object
	OSCTXT* pctxt = iControl->getCtxtPtr();
	
	// populate SET Session ID, if present
	// note this should always be present as this method is for building 
	// outgoing messages.
	if (aSessionId.iSetSessionIdPresent)
		{
		iData->sessionID.m.setSessionIDPresent = 1;
		
		// SET Session ID / Session ID
		iData->sessionID.setSessionID.sessionId = aSessionId.iSetSessionId->iSessionId;

		// SET Session ID / SET ID
		ASN1T_SETId& setId = iData->sessionID.setSessionID.setId;
		switch (aSessionId.iSetSessionId->iSetId->iSetIdType)
			{	
		  	case ESuplSetIdTypeIPAddress:
		  		{
		  		// ID is based on IP Address
	  			setId.t =  T_SETId_iPAddress;
				setId.u.iPAddress = (ASN1T_IPAddress*)rtxMemAllocZ(pctxt, sizeof(ASN1T_IPAddress));
				if (iControl->getStatus() != 0)
					{
					SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() Error, out of memory\n");
					return KErrNoMemory;
					}				

				if (aSessionId.iSetSessionId->iSetId->iIpAddress->iIpAddressType == ESuplIpAddressTypeV6)
					{
					setId.u.iPAddress->t =  T_IPAddress_ipv6Address;
					setId.u.iPAddress->u.ipv6Address = (ASN1T_IPAddress_ipv6Address*)rtxMemAllocZ(pctxt, sizeof(ASN1T_IPAddress_ipv6Address));
					if (iControl->getStatus() != 0)
						{
						SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() Error, out of memory\n");
						return KErrNoMemory;
						}					

					TInt len = aSessionId.iSetSessionId->iSetId->iIpAddress->iIpAddress.Length();
					void* data = (void*)aSessionId.iSetSessionId->iSetId->iIpAddress->iIpAddress.Ptr();
					memcpy ((void*)setId.u.iPAddress->u.ipv6Address->data, data, len);
					setId.u.iPAddress->u.ipv6Address->numocts = len;
					}
				else
					{
					setId.u.iPAddress->t =  T_IPAddress_ipv4Address;
					setId.u.iPAddress->u.ipv4Address = (ASN1T_IPAddress_ipv4Address*)rtxMemAllocZ(pctxt, sizeof(ASN1T_IPAddress_ipv4Address));
					if (iControl->getStatus() != 0)
						{
						SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() Error, out of memory\n");
						return KErrNoMemory;
						}					

					TInt len = aSessionId.iSetSessionId->iSetId->iIpAddress->iIpAddress.Length();
					void* data = (void*)aSessionId.iSetSessionId->iSetId->iIpAddress->iIpAddress.Ptr();
					memcpy ((void*)setId.u.iPAddress->u.ipv4Address->data, data, len);
					setId.u.iPAddress->u.ipv4Address->numocts = len;
					}
				break;
		  		}

			case ESuplSetIdTypeMsisdn:
				{
				setId.t = T_SETId_msisdn;
				setId.u.msisdn = (ASN1T_SETId_msisdn*)rtxMemAllocZ(pctxt, sizeof(ASN1T_SETId_msisdn));
				if (iControl->getStatus() != 0)
					{
					SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() Error, out of memory\n");
					return KErrNoMemory;
					}				
				TInt len = aSessionId.iSetSessionId->iSetId->iSetId.Length();
				void* data = (void*)aSessionId.iSetSessionId->iSetId->iSetId.Ptr();
				memcpy ((void*)setId.u.msisdn->data, data, len);
				setId.u.msisdn->numocts = len;
				break;
				}

		  	case ESuplSetIdTypeMdn:
		  		{
				setId.t = T_SETId_mdn;
				setId.u.mdn = (ASN1T_SETId_mdn*)rtxMemAllocZ(pctxt, sizeof(ASN1T_SETId_mdn));
				if (iControl->getStatus() != 0)
					{
					SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() Error, out of memory\n");
					return KErrNoMemory;
					}				
				TInt len = aSessionId.iSetSessionId->iSetId->iSetId.Length();
				void* data = (void*)aSessionId.iSetSessionId->iSetId->iSetId.Ptr();
				memcpy ((void*)setId.u.mdn->data, data, len);
				setId.u.mdn->numocts = len;
				break;
				}

		  	case ESuplSetIdTypeMin:
		  		{
				setId.t = T_SETId_min;
				setId.u.min = (ASN1T_SETId_min*)rtxMemAllocZ(pctxt, sizeof(ASN1T_SETId_min));
				if (iControl->getStatus() != 0)
					{
					SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() Error, out of memory\n");
					return KErrNoMemory;
					}				
				TInt len = aSessionId.iSetSessionId->iSetId->iSetId.Length();
				void* data = (void*)aSessionId.iSetSessionId->iSetId->iSetId.Ptr();
				memcpy ((void*)setId.u.min->data, data, len);
				setId.u.min->numbits = len*8;
				break;
				}

		  	case ESuplSetIdTypeImsi:
		  		{
				setId.t = T_SETId_imsi;
				setId.u.imsi = (ASN1T_SETId_imsi*)rtxMemAllocZ(pctxt, sizeof(ASN1T_SETId_imsi));
				if (iControl->getStatus() != 0)
					{
					SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() Error, out of memory\n");
					return KErrNoMemory;
					}				
				TInt len = aSessionId.iSetSessionId->iSetId->iSetId.Length();
				void* data = (void*)aSessionId.iSetSessionId->iSetId->iSetId.Ptr();
				memcpy ((void*)setId.u.imsi->data, data, len);
				setId.u.imsi->numocts = len;
				break;
				}

		  	case ESuplSetIdTypeNai:
		  	default:
		  		{
		  		__ASSERT_DEBUG(0, User::Invariant());
		  		return KErrNotSupported;
		  		}
			}
		}
	
	// populate SLP Session ID, if present
	// note this may not be present if building an outgoing SUPL START message,
	// as it is specified by the first incoming message from the SLP.
	if (aSessionId.iSlpSessionIdPresent)
		{
		iData->sessionID.m.slpSessionIDPresent = 1;

		// SLP Session ID / Session ID
		TInt len = aSessionId.iSlpSessionId->iSessionId.Length();
		void* data = (void*)aSessionId.iSlpSessionId->iSessionId.Ptr();
		memcpy ((void*)iData->sessionID.slpSessionID.sessionID.data, data, len);
		iData->sessionID.slpSessionID.sessionID.numocts = len;
		
		// SLP Session ID / SLP ID
		if (aSessionId.iSlpSessionId->iSlpAddress->iSlpAddressType == ESuplSlpAddressTypeFqdn)
			{
			iData->sessionID.slpSessionID.slpId.t = T_SLPAddress_fQDN;
			TInt len = aSessionId.iSlpSessionId->iSlpAddress->iFqdn->iFqdn.Length();
			
			// allocate memory for the FQDN string			
			char* tmpstr = (char*) rtxMemAlloc (pctxt, len+1);
			if (tmpstr == NULL)
				{
				SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() Error, out of memory\n");
				return KErrNoMemory;
				}
				
			void* source = (void*)aSessionId.iSlpSessionId->iSlpAddress->iFqdn->iFqdn.Ptr();
			memcpy ((void*)tmpstr, source, len);
			tmpstr[len] = '\0';  // add null-terminator
			iData->sessionID.slpSessionID.slpId.u.fQDN = tmpstr;
			}
		else
			{
			// SLP ID is an IP address
			iData->sessionID.slpSessionID.slpId.t = T_SLPAddress_iPAddress;
			iData->sessionID.slpSessionID.slpId.u.iPAddress = (ASN1T_IPAddress*)rtxMemAllocZ(pctxt, sizeof(ASN1T_IPAddress));
			if (iControl->getStatus() != 0)
				{
				SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() Error, out of memory\n");
				return KErrNoMemory;
				}
						
			if (aSessionId.iSlpSessionId->iSlpAddress->iIpAddress->iIpAddressType == ESuplIpAddressTypeV6)
				{ // IPv6
				iData->sessionID.slpSessionID.slpId.u.iPAddress->t =  T_IPAddress_ipv6Address;
				iData->sessionID.slpSessionID.slpId.u.iPAddress->u.ipv6Address = (ASN1T_IPAddress_ipv6Address*)rtxMemAllocZ(pctxt, sizeof(ASN1T_IPAddress_ipv6Address));
				if (iControl->getStatus() != 0)
					{
					SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() Error, out of memory\n");
					return KErrNoMemory;
					}
							
				TInt len = aSessionId.iSlpSessionId->iSlpAddress->iIpAddress->iIpAddress.Length();
				void* data = (void*)aSessionId.iSlpSessionId->iSlpAddress->iIpAddress->iIpAddress.Ptr();
				memcpy ((void*)iData->sessionID.slpSessionID.slpId.u.iPAddress->u.ipv6Address->data, data, len);
				iData->sessionID.slpSessionID.slpId.u.iPAddress->u.ipv6Address->numocts = len;
				}
			else
				{ // IPv4
				iData->sessionID.slpSessionID.slpId.u.iPAddress->t =  T_IPAddress_ipv4Address;
				iData->sessionID.slpSessionID.slpId.u.iPAddress->u.ipv4Address = (ASN1T_IPAddress_ipv4Address*)rtxMemAllocZ(pctxt, sizeof(ASN1T_IPAddress_ipv4Address));
				if (iControl->getStatus() != 0)
					{
					SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() Error, out of memory\n");
					return KErrNoMemory;
					}				

				TInt len = aSessionId.iSlpSessionId->iSlpAddress->iIpAddress->iIpAddress.Length();
				void* data = (void*)aSessionId.iSlpSessionId->iSlpAddress->iIpAddress->iIpAddress.Ptr();
				memcpy ((void*)iData->sessionID.slpSessionID.slpId.u.iPAddress->u.ipv4Address->data, data, len);
				iData->sessionID.slpSessionID.slpId.u.iPAddress->u.ipv4Address->numocts = len;
				}
			}
		}
	SUPLLOG(ELogP1, "CSuplMessageBase::SetSessionId() End\n");
	return KErrNone;
	}

/** 
MessageType()

Returns the message type 
@return message type (set at construction)
*/
EXPORT_C CSuplMessageBase::TSuplMessageType CSuplMessageBase::MessageType()
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::MessageType() Begin\n");
	SUPLLOG2(ELogP1, "CSuplMessageBase::MessageType() End (message type = %d)\n", iSuplMessageType);
	return iSuplMessageType;
	}

/** 
GetVersion()

Populates aVersion with the SUPL version
@param aVersion on return, populated with version of received SUPL message
*/
EXPORT_C TInt CSuplMessageBase::GetVersion(CSuplVersion& aVersion)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::Version() Begin\n");
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	aVersion.iMaj = iData->version.maj;
	aVersion.iMin = iData->version.min;
	aVersion.iServind = iData->version.servind;
	
	SUPLLOG(ELogP1, "CSuplMessageBase::Version() End\n");
	return KErrNone;
	}

/** 
GetSessionId()

Populates aSessionId with the SUPL Session ID

@param aSessionId on return, populated with session ID of received SUPL message
*/
EXPORT_C TInt CSuplMessageBase::GetSessionId(CSuplSessionId& aSessionId)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::SessionId() Begin\n");
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	
	// populate SET Session ID, if present
	// note in the case of a received SUPL INIT message, the 
	// SET Session ID is not present.
	if (iData->sessionID.m.setSessionIDPresent != 0)
		{
		aSessionId.iSetSessionIdPresent = ETrue;
		aSessionId.iSetSessionId->iSessionId = iData->sessionID.setSessionID.sessionId;
		ASN1T_SETId& setId = iData->sessionID.setSessionID.setId;
		
		switch (setId.t)
			{
			case T_SETId_iPAddress:
				{
				aSessionId.iSetSessionId->iSetId->iSetIdType = ESuplSetIdTypeIPAddress;
				// Pointer to the address data buffer
				TBuf8<16>& ipAddress = aSessionId.iSetSessionId->iSetId->iIpAddress->iIpAddress;
				
				// IPv4 or IPv6 address?
				if (setId.u.iPAddress->t == T_IPAddress_ipv6Address)
					{
					aSessionId.iSetSessionId->iSetId->iIpAddress->iIpAddressType = ESuplIpAddressTypeV6;
					TInt len = setId.u.iPAddress->u.ipv6Address->numocts;
					TUint8* data = setId.u.iPAddress->u.ipv6Address->data;
					ipAddress.Copy(data, len);
					}
				else
					{
					aSessionId.iSetSessionId->iSetId->iIpAddress->iIpAddressType = ESuplIpAddressTypeV4;
					TInt len = setId.u.iPAddress->u.ipv4Address->numocts;
					TUint8* data = setId.u.iPAddress->u.ipv4Address->data;
					ipAddress.Copy(data, len);
					}
				break;
				}
			case T_SETId_msisdn:
				{
				aSessionId.iSetSessionId->iSetId->iSetIdType = ESuplSetIdTypeMsisdn;
				TBuf8<16>& targetSetId = aSessionId.iSetSessionId->iSetId->iSetId;
				TInt len = setId.u.msisdn->numocts;
				TUint8* data = setId.u.msisdn->data;
				targetSetId.Copy(data, len);
				break;
				}
			case T_SETId_mdn:
				{
				aSessionId.iSetSessionId->iSetId->iSetIdType = ESuplSetIdTypeMdn;
				TBuf8<16>& targetSetId = aSessionId.iSetSessionId->iSetId->iSetId;
				TInt len = setId.u.mdn->numocts;
				TUint8* data = setId.u.mdn->data;
				targetSetId.Copy(data, len);
				break;
				}
			case T_SETId_min:
				{
				aSessionId.iSetSessionId->iSetId->iSetIdType = ESuplSetIdTypeMin;
				TBuf8<16>& targetSetId = aSessionId.iSetSessionId->iSetId->iSetId;
				TInt len = 5; // min is limited to 5 bytes (34 bits). Copy it all.
				TUint8* data = setId.u.min->data;
				targetSetId.Copy(data, len);
				break;
				}
			case T_SETId_imsi:
				{
				aSessionId.iSetSessionId->iSetId->iSetIdType = ESuplSetIdTypeImsi;
				TBuf8<16>& targetSetId = aSessionId.iSetSessionId->iSetId->iSetId;
				TInt len = setId.u.imsi->numocts;
				TUint8* data = setId.u.imsi->data;
				targetSetId.Copy(data, len);
				break;
				}
			case T_SETId_nai:
				{
				// we should not receive SET IDs of these types, as we will never 
				// have set them in the first outgoing message.
				__ASSERT_DEBUG(0, User::Invariant());
				return KErrNotSupported;
				}
			case T_SETId_extElem1:
			default:
				{
				__ASSERT_DEBUG(0, User::Invariant());
				return KErrCorrupt;
				}
			}
		} // end of SET Session ID handling

	// populate SLP Session ID, if present
	// note this should always be present as this method is intended for the 
	// decoding of received messages.
	if (iData->sessionID.m.slpSessionIDPresent != 0)
		{
		aSessionId.iSlpSessionIdPresent = ETrue;
		TUint8* dataSource = iData->sessionID.slpSessionID.sessionID.data;
		TInt len = iData->sessionID.slpSessionID.sessionID.numocts;
		aSessionId.iSlpSessionId->iSessionId.Copy(dataSource, len);
		
		if (iData->sessionID.slpSessionID.slpId.t == T_SLPAddress_fQDN)
			{
			// FQDN is a NULL terminated string, length 1..255
			aSessionId.iSlpSessionId->iSlpAddress->iSlpAddressType = ESuplSlpAddressTypeFqdn;
						
			// find the length of the FQDN (NULL terminated)
			const TUint8* tmp = (const TUint8*)iData->sessionID.slpSessionID.slpId.u.fQDN;
			TPtrC8 source = TPtrC8(tmp, 256);
			_LIT8(KNull,"\0");
			TInt fqdnLength = source.Find(KNull);
			
			if (fqdnLength > 0 && fqdnLength <256)
				{
				// copy to the container
				source.Set(tmp, fqdnLength);
				TBuf8<256>& fqdn = aSessionId.iSlpSessionId->iSlpAddress->iFqdn->iFqdn;
				fqdn.Copy(source);
				fqdn.SetLength(fqdnLength);
				}
			else
				{
				// fqdn length is corrupt
				__ASSERT_DEBUG(0, User::Invariant());
				return KErrCorrupt;
				}
			}
		else if (iData->sessionID.slpSessionID.slpId.t == T_SLPAddress_iPAddress)
			{
			// SLP ID is an IP Address
			aSessionId.iSlpSessionId->iSlpAddress->iSlpAddressType = ESuplSlpAddressTypeIp;

			// Pointer to the address data buffer
			TBuf8<16>& ipAddress = aSessionId.iSlpSessionId->iSlpAddress->iIpAddress->iIpAddress;
							
			// IPv4 or IPv6 address?
			if (iData->sessionID.slpSessionID.slpId.u.iPAddress->t == T_IPAddress_ipv6Address)
				{
				aSessionId.iSlpSessionId->iSlpAddress->iIpAddress->iIpAddressType = ESuplIpAddressTypeV6;
				TInt len = iData->sessionID.slpSessionID.slpId.u.iPAddress->u.ipv6Address->numocts;
				TUint8* data = iData->sessionID.slpSessionID.slpId.u.iPAddress->u.ipv6Address->data;
				ipAddress.Copy(data, len);
				}
			else
				{
				aSessionId.iSlpSessionId->iSlpAddress->iIpAddress->iIpAddressType = ESuplIpAddressTypeV4;
				TInt len = iData->sessionID.slpSessionID.slpId.u.iPAddress->u.ipv4Address->numocts;
				TUint8* data = iData->sessionID.slpSessionID.slpId.u.iPAddress->u.ipv4Address->data;
				ipAddress.Copy(data, len);
				}
			}
		else
			{
			// SLP ID Type is corrupt
			__ASSERT_DEBUG(0, User::Invariant());
			SUPLLOG(ELogP1, "CSuplMessageBase::SessionId() End (unexpected SLP ID type)\n");
			return KErrCorrupt;
			}
		} // end of SLP Session ID handling
		
	SUPLLOG(ELogP1, "CSuplMessageBase::SessionId() End\n");
	return KErrNone;
	}

/**
PopulateSetCapabilities()

Populates the data container specifying the content of the SET CAPABILITIES 
componenet according to the passed LBS capabilities.

Note that the Preferred Method parameter is set to the method identified
LAST in the array of capable methods. It is acceptable if this method is 
repeated elsewhere in the passed array.

@param  aCapsSource Capabilities Source
@param  aCapsTarget outgoing capabilities data object
@return error indication, KErrNone otherwise
*/
TInt CSuplMessageBase::PopulateSetCapabilities(const TLbsNetPosCapabilities& aCapsSource, ASN1T_SETCapabilities& aCapsTarget)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::PopulateSetCapabilities() Begin\n");
	
	// Specify supported positioning protocols. This implementation supports only RRLP
	aCapsTarget.posProtocol.rrlp = ETrue;
	
	// the preferred method is the last identified in the array.
	TInt prefMethod = PrefMethod::noPreference;
	
	// specify supported positioning methods
	TLbsNetPosMethod posMethod;
	for (TInt i=0 ; i<aCapsSource.NumPosMethods() ; ++i)
		{
		if (aCapsSource.GetPosMethod(i, posMethod) == KErrNone)
			{
			TUid means = posMethod.PosMeans();
			if (means == KLbsPositioningMeansGps)
				{
				// GPS positioning means
				if (posMethod.PosMode() == TPositionModuleInfo::ETechnologyTerminal ||
					posMethod.PosMode() == TPositionModuleInfo::ETechnologyUnknown)
					{
					aCapsTarget.posTechnology.autonomousGPS = ETrue;
					}
				else if (posMethod.PosMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted))
					{
					aCapsTarget.posTechnology.agpsSETBased = ETrue;
					prefMethod = PrefMethod::agpsSETBasedPreferred;
					}
				else if (posMethod.PosMode() == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted))
					{
					aCapsTarget.posTechnology.agpsSETassisted = ETrue;
					prefMethod = PrefMethod::agpsSETassistedPreferred;
					}
				}
			else if (means == KLbsPositioningMeansCell)
				{
				// Cell positioning means
				aCapsTarget.posTechnology.eCID = ETrue;
				}
			else if (means == KLbsPositioningMeansEotd)
				{
				// EOTD positioning means
				aCapsTarget.posTechnology.eOTD = ETrue;
				}
			else if (means == KLbsPositioningMeansOtdoa)
				{
				// OTDOA positioning means
				aCapsTarget.posTechnology.oTDOA = ETrue;
				}
			else if (means == KLbsPositioningMeansAflt)
				{
				// AFLT positioning means
				aCapsTarget.posTechnology.aFLT = ETrue;
				}
			else 
				{
				// unknown/corrupt positioning means
				__ASSERT_DEBUG(0, User::Invariant());
				return KErrCorrupt;
				}
			}
		}
	
	// specify the preferred GPS mode
	aCapsTarget.prefMethod = prefMethod;
	
	SUPLLOG(ELogP1, "CSuplMessageBase::PopulateSetCapabilities() End\n");
	return KErrNone;
	}


/**
PopulateLocationId()

Populates the data container specifying the content of the LOCATION ID
componenet according to the passed LBS location details.


@param  aLocSource LocationID Source
@param  aLocTarget outgoing LocationID data object
@return error indication, KErrNone otherwise
*/
TInt CSuplMessageBase::PopulateLocationId(const CSuplLocationId& aLocSource, ASN1T_LocationId& aLocTarget)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::PopulateLocationId() Begin\n");
	__ASSERT_DEBUG(aLocSource.iType<ESuplLocationTypeLimit, User::Invariant());
	
	// pointer to context object
	OSCTXT* pctxt = iControl->getCtxtPtr();

	// Cell information status
	aLocTarget.status = aLocSource.iStatus;

	// Cell information is carrier type dependant
	if (aLocSource.iType == ESuplLocationTypeGsm)
		{
		aLocTarget.cellInfo.t = T_CellInfo_gsmCell;
		aLocTarget.cellInfo.u.gsmCell = (ASN1T_GsmCellInformation*)rtxMemAllocZ(pctxt, sizeof(ASN1T_GsmCellInformation));
		if (iControl->getStatus() != 0)
			{
			SUPLLOG(ELogP1, "CSuplMessageBase::PopulateLocationId() Error, out of memory\n");
			return KErrNoMemory;
			}
		
		// populate the cell information
		CSuplGsmCellInfo* gsmInfo = aLocSource.iGsmCellInfo;
		aLocTarget.cellInfo.u.gsmCell->refMCC = gsmInfo->iRefMCC;
		aLocTarget.cellInfo.u.gsmCell->refMNC = gsmInfo->iRefMNC;
		aLocTarget.cellInfo.u.gsmCell->refLAC = gsmInfo->iRefLAC;
		aLocTarget.cellInfo.u.gsmCell->refCI  = gsmInfo->iRefCI;
		
		// NMR is optional
		if (gsmInfo->iNMR > 0)
			{
			
			aLocTarget.cellInfo.u.gsmCell->m.nMRPresent = 1;

			// initialise the NMR list
			ASN1C_NMR list (*iControl, aLocTarget.cellInfo.u.gsmCell->nMR);
			list.init();
	
			// populate the array
			for (TInt i = 0; i < gsmInfo->iNMR; ++i)
				{
				ASN1T_NMRelement* nmrElement;
				nmrElement = list.NewElement();
				if (nmrElement == NULL)
					{
					SUPLLOG(ELogP1, "CSuplMessageBase::PopulateLocationId() Error, out of memory\n");
					return KErrNoMemory;
					}
				nmrElement->aRFCN = gsmInfo->iNmrElements[i].iARFCN;
				nmrElement->bSIC  = gsmInfo->iNmrElements[i].iBSIC;
				nmrElement->rxLev = gsmInfo->iNmrElements[i].iRxLev;
				
				list.Append(nmrElement);
				}			
			} // NMR

		// TA is optional
		if (gsmInfo->iTA >= 0)
			{
			aLocTarget.cellInfo.u.gsmCell->m.tAPresent = 1;
			aLocTarget.cellInfo.u.gsmCell->tA = gsmInfo->iTA;
			}
		}
	else if (aLocSource.iType == ESuplLocationTypeCdma)
		{
		iData->message.u.msSUPLSTART->locationId.cellInfo.t = T_CellInfo_cdmaCell;
		aLocTarget.cellInfo.u.cdmaCell = (ASN1T_CdmaCellInformation*)rtxMemAllocZ(pctxt, sizeof(ASN1T_CdmaCellInformation));
		if (iControl->getStatus() != 0)
			{
			SUPLLOG(ELogP1, "CSuplMessageBase::PopulateLocationId() Error, out of memory\n");
			return KErrNoMemory;
			}
		
		// populate the cell information
		CSuplCdmaCellInfo* cdmaInfo = aLocSource.iCdmaCellInfo;
	   	aLocTarget.cellInfo.u.cdmaCell->refNID        = cdmaInfo->iRefNID;
		aLocTarget.cellInfo.u.cdmaCell->refSID        = cdmaInfo->iRefSID;
   		aLocTarget.cellInfo.u.cdmaCell->refBASEID     = cdmaInfo->iRefBASEID;
   		aLocTarget.cellInfo.u.cdmaCell->refBASELAT    = cdmaInfo->iRefBASELAT;
   		aLocTarget.cellInfo.u.cdmaCell->reBASELONG    = cdmaInfo->iReBASELONG;
   		aLocTarget.cellInfo.u.cdmaCell->refREFPN      = cdmaInfo->iRefREFPN;
   		aLocTarget.cellInfo.u.cdmaCell->refWeekNumber = cdmaInfo->iRefWeekNumber;
   		aLocTarget.cellInfo.u.cdmaCell->refSeconds    = cdmaInfo->iRefSeconds;
		}
	else if (aLocSource.iType == ESuplLocationTypeWcdma)
		{
		aLocTarget.cellInfo.t = T_CellInfo_wcdmaCell;
		aLocTarget.cellInfo.u.wcdmaCell = (ASN1T_WcdmaCellInformation*)rtxMemAllocZ(pctxt, sizeof(ASN1T_WcdmaCellInformation));
		if (iControl->getStatus() != 0)
			{
			SUPLLOG(ELogP1, "CSuplMessageBase::PopulateLocationId() Error, out of memory\n");
			return KErrNoMemory;
			}
		
		// populate the cell information
		CSuplWcdmaCellInfo* wcdmaInfo = aLocSource.iWcdmaCellInfo;
		aLocTarget.cellInfo.u.wcdmaCell->refMCC = wcdmaInfo->iRefMCC;
		aLocTarget.cellInfo.u.wcdmaCell->refMNC = wcdmaInfo->iRefMNC;
		aLocTarget.cellInfo.u.wcdmaCell->refUC  = wcdmaInfo->iRefUC;
		}
	else
		{
		SUPLLOG(ELogP1, "CSuplMessageBase::PopulateLocationId() Error, unexpected location source type\n");
		return KErrArgument;
		}
	
	SUPLLOG(ELogP1, "CSuplMessageBase::PopulateLocationId() End\n");
	return KErrNone;
	}


/**
PopulatePosition()

Populates the outgoing position element, used in POS INIT and END messages.

Timestamp is encoded in UTC format YYMMDDhhmmssZ

Latitude is encoded as an integer N (0..2^23-1) from the actual latitude X 
in degrees, where   N <= 2^23 * X/90 < N+1
	
Longitude is encoded as an integer N (-2^23..2^23-1) from the actual 
longitude X in degrees, where   N <= 2^24 * X/360 < N+1
	
Horizontal Uncertainty is encoded as per 3GPP GAD, ie describing an 
ellipse with semi-major and semi-minor axis measurements and orientation.
With only one value for horizontal accuracy available, the circle 
described is encoded as an ellipse with semi-major = semi-minor axis and
0 degree orientation. The uncertainty is encoded to 7 bits, thus:
	r = C( (1+x)^k - 1 )
where r = distance in meters, C = 10, x = 0.1 and K is the encoded constant.

Confidence information is not available in this implementation and is omitted

Altitude, if data is available, is encoded as a 15 bit binary encoded number.

Altitude uncertainty is encoded as a distance above or below the WGS84
ellipsoid, using the same formula as for horizontal uncertainty, but where
C = 45, x = 0.025. Encoded value K is limited to 7 bits.

If the passed TPositionInfoBase object is a TPositionCourseInfo, then the
optional velocity element is populated with data from the course info object.
Velocity information is encoded as per 3GPP TS 23.032, and the "horizontal
velocity with uncertainty" format is used.

@param  aPosSource Position source data from LBS
@param  aPosTarget outgoing Position message data object
@return error indication, KErrNone otherwise
*/
TInt CSuplMessageBase::PopulatePosition(const TPositionInfoBase& aPosSource, ASN1T_Position& aPosTarget)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::PopulatePosition() Begin\n");
	
	// access to source position data
	TPosition position;
	if ((aPosSource.PositionClassType() & EPositionInfoClass) != 0)
		{
		TPositionInfo posSource = reinterpret_cast <const TPositionInfo&> (aPosSource);
		posSource.GetPosition(position);

		// TIMESTAMP (mandatory)
		// Convert timestamp to UTC format: YYMMDDhhmmssZ
		TDateTime dateTime = position.Time().DateTime();
		iUtcTime.SetLength(iUtcTime.MaxLength());

		iUtcTime[0]=(((dateTime.Year() % 1000) % 100) / 10) + 48; // second last number in the year
		iUtcTime[1]=(((dateTime.Year() % 1000) % 100) % 10) + 48; // last number in the year
		iUtcTime[2]=((dateTime.Month() + 1) / 10) + 48;
		iUtcTime[3]=((dateTime.Month() + 1) % 10) + 48;
		iUtcTime[4]=((dateTime.Day()) / 10) + 48;
		iUtcTime[5]=((dateTime.Day()) % 10) + 48;
		iUtcTime[6]=(dateTime.Hour() / 10) + 48;
		iUtcTime[7]=(dateTime.Hour() % 10) + 48;
		iUtcTime[8]=(dateTime.Minute() / 10) + 48;
		iUtcTime[9]=(dateTime.Minute() % 10) + 48;
		iUtcTime[10]=(dateTime.Second() / 10) + 48;
		iUtcTime[11]=(dateTime.Second() % 10) + 48;
		iUtcTime[12]= 0x5A; // ASCII for "Z"
		
		aPosTarget.timestamp = (const char*) &iUtcTime[0];
		
		// POSITION ESTIMATE (mandatory)
		// -- latitude/longitude (mandatory)
		aPosTarget.positionEstimate.latitudeSign = PositionEstimate_latitudeSign::north;
		TReal64 latitude  = position.Latitude();
		TReal64 longitude = position.Longitude();
		if (latitude < 0)
			{
			aPosTarget.positionEstimate.latitudeSign = PositionEstimate_latitudeSign::south;
			latitude *= -1;
			}
		aPosTarget.positionEstimate.latitude  = latitude  * KLbsLatitudeConst; 
		aPosTarget.positionEstimate.longitude = longitude * KLbsLongitudeConst;

		// -- uncertainty (optional)
		if (position.HorizontalAccuracy() != 0)
			{
			TInt uncert = Uncertainty(position.HorizontalAccuracy());
			aPosTarget.positionEstimate.m.uncertaintyPresent = 1;
			aPosTarget.positionEstimate.uncertainty.uncertaintySemiMajor = uncert;
			aPosTarget.positionEstimate.uncertainty.uncertaintySemiMinor = uncert;
			aPosTarget.positionEstimate.uncertainty.orientationMajorAxis = 0;
			}

		// -- confidence (optional)
		// this information is not available, omitted.
		aPosTarget.positionEstimate.m.confidencePresent = 0;
		aPosTarget.positionEstimate.confidence = 0;
		
		// -- altitude information (optional)
		if (position.Altitude() != 0)
			{
			aPosTarget.positionEstimate.m.altitudeInfoPresent = 1;
			TReal32 altitude = position.Altitude();
			aPosTarget.positionEstimate.altitudeInfo.altitudeDirection = AltitudeInfo_altitudeDirection::height;
			if (altitude < 0)
				{
				aPosTarget.positionEstimate.altitudeInfo.altitudeDirection = AltitudeInfo_altitudeDirection::depth;
				altitude *= -1;
				}
			aPosTarget.positionEstimate.altitudeInfo.altitude = EncodeAltitude(altitude);
			aPosTarget.positionEstimate.altitudeInfo.altUncertainty = UncertaintyAltitude(position.VerticalAccuracy());
			}
		}
	else
		{
		SUPLLOG(ELogP1, "CSuplMessageBase::PopulatePosition() Error, position class is not EPositionInfoClass type\n");
		return KErrNotSupported;
		}
	
	// handle velocity (TCourse) information
	if ((aPosSource.PositionClassType() & EPositionCourseInfoClass) != 0)
		{
		TPositionCourseInfo posSource = reinterpret_cast <const TPositionCourseInfo&> (aPosSource);
		TCourse course;
		posSource.GetCourse(course);
		
		// -- velocity
		aPosTarget.m.velocityPresent = 1;
		TInt velErr = PopulateVelocity(course, aPosTarget.velocity);
		
		if (velErr != KErrNone)
			{
			return velErr;
			}
		}

	SUPLLOG(ELogP1, "CSuplMessageBase::PopulatePosition() End\n");
	return KErrNone;
	}

/*
PopulateVelocity()

Populates the outgoing velocity sub-element with velocity information from 
an LBS TCourse object.

@param  aVelSource course/velocity source data from LBS
@param  aVelTarget outgoing velocity message data object
@return error indication, KErrNone otherwise
*/
TInt CSuplMessageBase::PopulateVelocity(const TCourse& aCourse, ASN1T_Velocity& aVelTarget)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::PopulateVelocity() Begin\n");
	aVelTarget.t = T_Velocity_horveluncert;
	aVelTarget.u.horveluncert = (ASN1T_Horveluncert*)rtxMemAllocZ(iControl->getCtxtPtr(), sizeof(ASN1T_Horveluncert));
	if (iControl->getStatus() != 0)
		{
		SUPLLOG(ELogP1, "CSuplMessageBase::PopulateVelocity() Error, out of memory\n");
		return KErrNoMemory;
		}

	// ---- bearing
	TUint bearing  = (TUint)aCourse.Heading();
	aVelTarget.u.horveluncert->bearing.numbits = 9;
	aVelTarget.u.horveluncert->bearing.data[0] = bearing >> 1; // the first 8 of 9 bits
	aVelTarget.u.horveluncert->bearing.data[1] = bearing << 7; // just the 9th bit, in msb

	// ---- horizontal speed. Convert meters per second -> kilomteres per hour
	TReal32 horSpeed = aCourse.Speed() * KLbsMpsKmphConstant;
	//      adjust for GAD encoding and lose decimal precision
	horSpeed += 0.5;
	TUint horSpeedInt = (TUint)horSpeed;
	//      limit to 2^16-1
	if (horSpeedInt > 65535)
		{
		horSpeedInt = 65535;
		}
	aVelTarget.u.horveluncert->horspeed.numbits = 16;
	aVelTarget.u.horveluncert->horspeed.data[0] = horSpeedInt >> 8;
	aVelTarget.u.horveluncert->horspeed.data[1] = horSpeedInt;

	// ---- horizontal speed. Convert meters per second -> kilomteres per hour
	TUint uncertSpeed  = (TUint)(aCourse.SpeedAccuracy() * KLbsMpsKmphConstant);
	if (uncertSpeed > 255)
		{
		uncertSpeed = 255;
		}
	aVelTarget.u.horveluncert->uncertspeed.numbits = 8;
	aVelTarget.u.horveluncert->uncertspeed.data[0] = uncertSpeed;
	
	SUPLLOG(ELogP1, "CSuplMessageBase::PopulateVelocity() End\n");
	return KErrNone;
	}
	
		


/**
LeaveIfAllocErrorL()

Calls User::Leave(<error code>) if a memory allocation has failed.
*/
void CSuplMessageBase::LeaveIfAllocErrorL()
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::LeaveIfAllocErrorL() Begin\n");
	if (iControl->getStatus() == RTERR_NOMEM)
		{
		SUPLLOG(ELogP1, "CSuplMessageBase::LeaveIfAllocErrorL() End (out of memory error)\n");
		User::Leave(KErrNoMemory);
		}
	else if (iControl->getStatus() != RT_OK)
		{
		SUPLLOG2(ELogP1, "CSuplMessageBase::LeaveIfAllocErrorL() End (ASN1 runtime error, %d)\n", iControl->getStatus());
		User::Leave(KErrGeneral);
		}
	SUPLLOG(ELogP1, "CSuplMessageBase::LeaveIfAllocErrorL() End\n");
	}

/**
Uncertainty()

Converts a minumum accuracy value in meters to an uncertainty value K as 
described in 3GPP 23.032 (Universal Geographical Area Description) section 6.2.

r = C((1+x)^K - 1)

where r = distance in meters
      C = 10
      x = 0.1
      K = uncertainty value
      
hence K = ln(r/C + 1) / ln(1.1)

@param aDistance - distance measurement in meters
@return uncertainty value K
*/
TInt CSuplMessageBase::Uncertainty(const TReal32& aDistance)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::Uncertainty() Begin\n");
	TReal uncert;
	Math::Ln(uncert,  (aDistance/10) + 1 );
	uncert /= KLbsLogOnePointOne;
	if (uncert>KLbsMaxUncert)
		{
		uncert = KLbsMaxUncert;
		}

	// round to nearest whole number
	TReal uncertRounded;
	Math::Round(uncertRounded, uncert, 0);
	
	SUPLLOG(ELogP1, "CSuplMessageBase::Uncertainty() End\n");
	return (TInt)uncertRounded;
	}
	
/**
UncertaintyAltitude()

Converts a minumum accuracy value in meters to an uncertainty altitude value K as 
described in 3GPP 23.032 (Universal Geographical Area Description) section 6.4.

r = C((1+x)^K - 1)

where r = distance in meters
      C = 45
      x = 0.1
      K = uncertainty value
      
hence K = ln(r/C + 1) / ln(1.1)

@param aDistance - altitude accuracy in meters
@return uncertainty altitude value K
*/
TInt CSuplMessageBase::UncertaintyAltitude(const TReal32& aDistance)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::UncertaintyAltitude() Begin\n");
	TReal uncert;
	Math::Ln(uncert,  (aDistance/45) + 1 );
	uncert /= KLbsLogOnePointZeroTwoFive;
	if (uncert>KLbsMaxUncert)
		{
		uncert = KLbsMaxUncert;
		}

	// round to nearest whole number
	TReal uncertRounded;
	Math::Round(uncertRounded, uncert, 0);
	
	SUPLLOG(ELogP1, "CSuplMessageBase::UncertaintyAltitude() End\n");
	return (TInt)uncertRounded;
	}


/**
EncodeAltitude()

Converts an value for altiutude to an 15 bit binary coded number N

@param aAltitude - altitude in meters
@return uncertainty altitude value K
*/
TInt CSuplMessageBase::EncodeAltitude(const TReal32& aAltitude)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::EncodeAltitude() Begin\n");
	TInt altEncoded = (TInt)aAltitude;
	if (altEncoded>KLbsMaxAltitude)
		{
		altEncoded = KLbsMaxAltitude;
		}

	SUPLLOG(ELogP1, "CSuplMessageBase::EncodeAltitude() End\n");
	return altEncoded;
	}

/**
Translates error codes returned by the ASN1 runtime library to distinguish
from Symbian global error codes.

Errors are simply translated to positive error codes. They maintain their
meaning as described in rtxErrCodes.h and asn1ErrCodes.h.

Exceptions:
  RTERR_NOMEM is translated to global error code KErrNoMemory

@see rtxErrCodes.h
@see asn1ErrCodes.h
*/
TInt CSuplMessageBase::ProcessAsn1Error(TInt aError)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::ProcessAsn1Error() Begin\n");
	if (aError == RTERR_NOMEM)
		{
		SUPLLOG(ELogP1, "CSuplMessageBase::ProcessAsn1Error() End (Out Of Memory)\n");
		return KErrNoMemory;
		}
	else
		{
		SUPLLOG2(ELogP1, "CSuplMessageBase::ProcessAsn1Error() End (ASN1 Runtime Error %d)\n", aError);
		return aError * -1;
		}
	}

/** 
Prints the content of the data structure to the logger 
*/
EXPORT_C void CSuplMessageBase::LogMessageContent()
	{
	//SUPLLOG_PDU(iControl);
	if (MessageType() == ESuplPos)
		{
		CSuplPos* suplPos = reinterpret_cast <CSuplPos*>(this);
		suplPos->LogPayload();
		}
	}
	

