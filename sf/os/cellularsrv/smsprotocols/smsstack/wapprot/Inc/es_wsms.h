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

/**
 @file
 @publishedAll
*/

#if !defined(__ES_WSMS_H__)
#define __ES_WSMS_H__

#include <es_sock.h>

/** WAP SMS Protocol Module address family ID. */
const TUint KWAPSMSAddrFamily=0x011;
/** WAP SMS Protocol Module datagram protocol ID. */
const TUint KWAPSMSDatagramProtocol=0x01;
/** WAP SMS Protocol Module maximum number of sockets supported. */
const TInt  KWAPSMSNumberSockets=0x100;
/** The maximum datagram size the SMS sockets protocol supports. */
const TUint KWAPSMSMaxDatagramSize = 255*160; // Based on 7 bit encoding
/** WAP SMS Protocol Module service information flags. */
const TUint KWAPSMSDatagramServiceInfo = KSIConnectionLess | KSIMessageBased;
/** CDMA WAP SMS Protocol Module address family ID for legacy WDP application. */
const TUint KWAPCDMASMSAddrFamily=0x013;

/**
 *  @publishedAll
 */
_LIT(KWAPSMSProtocolId,"WAPSMS Datagram");
_LIT(KWAPCDMASMSProtocolId,"WAP CDMA SMS Datagram");

//
// Wap Address / Port Settings
//

/** WAP port setting. */
enum TWapPortNumber
	{
	/** Unspecified. */
	EWapPortUnspecified = -1,
	/** Connectionless session protocol. */
	EWapPortWsp         = 9200,
	/** Connection oriented session protocol. */
	EWapPortWspWtp      = 9201,
	/** Secure connectionless session protocol. */
	EWapPortWspWtls     = 9202,
	/** Secure connection oriented session protocol. */
	EWapPortWspWtpWtls  = 9203,
	/** vCard. */
	EWapPortVCard       = 9204,
	/** Secure vCard. */
	EWapPortVCardWtls   = 9206,
	/** vCal. */
	EWapPortVCal        = 9205,
	/** Secure vCal. */
	EWapPortVCalWtls    = 9207
	};


/**
 *  Socket address type used with the WAP SMS Protocol Module.
 *  @publishedAll
 *  @released
 */
class TWapAddr : public TSockAddr
	{
public:
	enum { EMaxWapAddressLength = 24 };
public:
	/**
	 *  Gets the WAP port number.
	 *  
	 *  	* @return	WAP port number 
	 */
	inline TWapPortNumber WapPort() const;

	/**
	 *  Sets the WAP port number.
	 *  
	 *  	* @param aPort	WAP port number 
	 */
	inline void SetWapPort(TWapPortNumber aPort);

	/**
	 *  Sets the WAP address.
	 *  
	 *  	* @param aTel	WAP address 
	 */
	inline void SetWapAddress(const TDesC8& aTel);

	/**
	 *  Gets the WAP address.
	 *  
	 *  	* @return	WAP address 
	 */
	inline TPtrC8 WapAddress() const;

	/**
	 *  Equality operator.
	 *  
	 *  @param aAddr	Object to compare
	 *  	* @return		ETrue if the object value's are equal 
	 */
	inline TBool operator==(const TWapAddr& aAddr) const;
	};

//
// Option Settings
//

/** WAP SMS Protocol Module option level, for RSocket::SetOpt() and RSocket::GetOpt() calls. */
const TInt KWapSmsOptionLevel = KSOLSocket + 1;

/**
 *  WAP SMS Protocol Module option for the data coding scheme.
 *  
 *  * anOption in RSocket::GetOpt() and RSocket::SetOpt() should be a TWapSmsDataCodingScheme value. 
 */
const TInt KWapSmsOptionNameDCS      = 0x01;

/** Unused. */
const TInt KWapSmsOptionSmartMessage = 0x02;

/** WAP SMS Protocol Module option to set the message type to WAP datagram. */
const TInt KWapSmsOptionWapDatagram  = 0x03;

/** WAP SMS Protocol Module option to set the status report of the last segment. */
const TInt KWapSmsStatusReportScheme = 0x06;	//	KWapSmsOptionNewStyleClient	= 0x04;
												// KWapSmsOptionOKToDeleteMessage = 0x05;
												// Defined in wap_sock.h


/**
 *  WAP Data Coding Scheme types.
 *  
 *  * These are used with KWapSmsOptionNameDCS. 
 */
enum TWapSmsDataCodingScheme
	{
	/** 7-bit. */
	EWapSms7BitDCS,
	/** 8-bit. */
	EWapSms8BitDCS
	};
	

/** WAP Status Report Scheme types.
*
* These are used with KWapSmsStatusReportScheme. */
enum TWapSmsStatusReportScheme
	{
	/** Default Scheme. */
	EWapSmsDefault,
	/** TPSRR Scheme. */
	EWapSmsTPSRR,
	/** Control Information Element Scheme. */
	EWapSmsCtrlInfoElement
	};

#include "es_wsms.inl"

#endif
