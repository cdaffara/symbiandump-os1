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
// Defines types for SMS adressing and the class TSmsAddr
// 
//



/**
 @file
 @publishedAll
 @released
*/

#if !defined(SMSUADDR_H__)
#define SMSUADDR_H__

#include <e32property.h>
#include <es_sock.h>

/** SMS sockets family identifier.*/
const TUint KSMSAddrFamily = 0x010;
/** SMS sockets protocol ID. */
const TUint KSMSDatagramProtocol = 0x02;
/** The maximum number of SAPs the SMS sockets protocol supports. */
const TInt KSMSNumberSockets = 0x100;
/** The maximum datagram size the SMS sockets protocol supports. */
const TUint KSMSMaxDatagramSize = 255*160; // Based on 7 bit encoding
/** SMS sockets service flags. */
const TUint KSMSDatagramServiceInfo = KSIConnectionLess | KSIMessageBased ;

_LIT(KSmsDatagram,"SMS Datagram");

/** Specifies SMS sockets protocol level for the provider in the aLevel argument of RSocket::Ioctl(). */
const TUint KSolSmsProv = 0x100;

/**
 *  Ioctl command for deleting SMS messages.
 *  
 *  For this command, the aDesc argument of RSocket::Ioctl() should be NULL.
 *  
 *  @capability WriteUserData
 *  @see RSocket::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
 */
const TUint KIoctlDeleteSmsMessage        = 0x0300;

/**
 *  Ioctl command for counting stored SMS messages.
 *  
 *  For this command, the aDesc argument of RSocket::Ioctl() should be a packaged
 *  TUint. On completion, this holds the count of messages enumerated.
 *  
 *  @capability ReadUserData
 *  @see RSocket::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
 */
const TUint KIoctlEnumerateSmsMessages    = 0x0301;

/**
 *  Ioctl command to indicate to the protocol that the client received messages
 *  successfully.
 *  
 *  For this command, the aDesc argument of RSocket::Ioctl() should be NULL.
 *  
 *  @capability ReadUserData
 *  @see RSocket::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
 */
const TUint KIoctlReadMessageSucceeded    = 0x0304;

/**
 *  Ioctl command to indicate to the protocol that the client failed to receive
 *  messages successfully.
 *  
 *  For this command, the aDesc argument of RSocket::Ioctl() should be NULL.
 *  
 *  @capability ReadUserData
 *  @see RSocket::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
 */
const TUint KIoctlReadMessageFailed       = 0x0305;

/**
 *  Ioctl command for sending SMS messages.
 *  
 *  For this command, the aDesc argument of RSocket::Ioctl() should be a packaged
 *  TUint.
 *  
 *  @capability NetworkServices
 *  @see RSocket::Ioctl()
 */
const TUint KIoctlSendSmsMessage          = 0x0306;

/**
 *  Ioctl command for writing SMS messages to SIM.
 *  
 *  For this command, the aDesc argument of RSocket::Ioctl() may be NULL or may hold a packaged
 *  descriptor large enough to store information on slots for the message. This descriptor will
 *  be populated upon completion of the request and shall be used by the client to update the
 *  CSmsMessage object that has been written, by invoking its UpdateSlotsL() method. This has
 *  to be done if the client wants to be able to delete this message from the store at some
 *  point, unless it opts to enumerate messages before deletion takes place.
 *  
 *  @capability WriteUserData
 *  @see RSocket::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
 */
const TUint KIoctlWriteSmsMessage         = 0x0307;

/**
 *  Ioctl command for enumerating SMS parameter sets.
 *  
 *  For this command, the aDesc argument of RSocket::Ioctl() should be NULL.
 *  
 *  @capability ReadDeviceData
 *  @see RSocket::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
 */
const TUint KIoctlReadSmsParams           = 0x0308;

/**
 *  Ioctl command for completing SMSP list read request.
 *  
 *  For this command, the aDesc argument of RSocket::Ioctl() should be NULL.
 *  
 *  @capability ReadDeviceData
 *  @see RSocket::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
 */
const TUint KIoctlCompleteReadSmsParams   = 0x0309;

/**
 *  Ioctl command ofr writing SMS parameter sets.
 *  
 *  For this command, the aDesc argument of RSocket::Ioctl() should be NULL.
 *  
 *  @capability WriteDeviceData
 *  @see RSocket::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
 */
const TUint KIoctlWriteSmsParams          = 0x0310;

/**
 *  Ioctl command for finding out whether SMS stack is configured for handling 
 *	of class 0 messages in out-of-disk condition or not.
 *  
 *  For this command, the aDesc argument of RSocket::Ioctl() should be NULL.
 *  
 *  @capability WriteDeviceData
 *  @see RSocket::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
 */
const TUint KIoctlSupportOODClass0SmsMessages	= 0x0311;

/**
 *  KIOctlSelect completion status if a modem is present.
 *  
 *  @capability WriteDeviceData
 *  @see RSocket::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
 */
const TUint KIoctlSelectModemPresent    = 0x400;

/**
 *  KIOctlSelect completion status if a modem is not present.
 *  
 *  @capability WriteDeviceData
 *  @see RSocket::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
 */
const TUint KIoctlSelectModemNotPresent = 0x401;


/**
 *  Category for SMS Stack P&S variables.
 */
const TUid KUidPSSMSStackCategory = {0x101F7989}; // RootServer's KUidCommsProcess


/**
 *  Key for SMS Stack P&S Disk Space Monitor 
 */
const TInt KUidPSSMSStackDiskSpaceMonitorKey = 0x10282FAF;


/**
 *  Category for SMS Stack P&S variables.
 */
const RProperty::TType KUidPSSMSStackDiskSpaceMonitorKeyType = RProperty::EInt;


/**
 *  State of Disk Space Monitor.
 */
enum TSmsDiskSpaceMonitorStatus
	{
	/** The Disk Space status is unknown. */
	ESmsDiskSpaceUnknown   = 0,

	/** The Disk Space status is believed to be available. */
	ESmsDiskSpaceAvailable = 1,

	/** The Disk Space status was not present during the last receive operation
	    and some PDU have been negatively acknowledged. */
	ESmsDiskSpaceFull      = 2
	};


const TInt KMaxServiceCenterAddressLength = 14;
/** Buffer to hold Service Center address. */
typedef TBuf<KMaxServiceCenterAddressLength> TSmsServiceCenterAddress;
/** Package buffer for TSmsServiceCenterAddress objects. */
typedef TPckgBuf<TSmsServiceCenterAddress> TSmsServiceCenterAddressBuf;

//note: Maintainers, please do *not* change the order of these enums!
/**
 *  SMS address family for a socket.
 *  @publishedAll
 *  @released 
 */
enum TSmsAddrFamily
	{
	ESmsAddrUnbound                = 0, //< Not bound yet.
	ESmsAddrSendOnly               = 1, //< Only for sending, no reception.
	ESmsAddrMessageIndication      = 2, //< Matches on IEI 0x01 and DCS 0x1100xxxx, 0x1101xxxx and 0x1110xxxx.
	ESmsAddrMatchIEI               = 3, //< For matching Information Element Identifiers (IEI)
	ESmsAddrMatchText              = 4, //< For matching any text patterns
	ESmsAddrRecvAny                = 5, //< Receive all messages. Only one client can use this.
	ESmsAddrStatusReport           = 6, //< For receiving Status Reports.
	ESmsAddrLocalOperation         = 7, //< For local SIM operations.
	ESmsAddrApplication8BitPort    = 8, //< For sock port identification.
	ESmsAddrApplication16BitPort   = 9, //< For sock port identification.
	ESmsAddrEmail			  	   = 10 //< for matching of email messages.
	};



/**
 *  SMS address for a socket.
 *  @publishedAll
 *  @released
 */
class TSmsAddr : public TSockAddr
	{
public:
	enum { EMaxTextMatchLength = 24 };
public:
	IMPORT_C TSmsAddr();
	IMPORT_C TSmsAddrFamily SmsAddrFamily() const;
	IMPORT_C void SetSmsAddrFamily(TSmsAddrFamily aFamily);
	IMPORT_C TInt IdentifierMatch() const;
	IMPORT_C void SetIdentifierMatch(TInt aIdentifier);
	IMPORT_C TPtrC8 TextMatch() const;
	
	/**
	 *  Sets the match text. The matching is case-insensitive comparison 
	 *  done against the leading text in the message. Wild card matching
	 *  is not supported.
	 *  
	 *  @param aText The match text
	 *  @capability None
	 */
	IMPORT_C void SetTextMatch(const TDesC8& aText);
	IMPORT_C TBool operator==(const TSmsAddr& aAddr) const;
	};


#endif // SMSUADDR_H__
