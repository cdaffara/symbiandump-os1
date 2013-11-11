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
 @released
*/

#ifndef __IR_SOCK_H__
#define __IR_SOCK_H__

#include "es_sock.h"
#include <e32property.h>

//################## EXPORTED CONSTANTS ########################


/** IrDA protocol family. */
const TUint KIrdaAddrFamily=0x100;   // Address of Irda prot family

/** IrMUX protocol within the IrDA protocol family. */
const TUint KIrmux=88;               // Protocol number for Irmux

/** IrTinyTP protocol within the IrDA protocol family.. */
const TUint KIrTinyTP=89;            // Protocol number for IrTinyTP

/** An invalid LSAP number--binding to this will select the first unused LSAP. */
const TUint KAutoBindLSAP = 0xffff;


/** Category used when publishing IrDA status notifications. */
const TUid KIrdaPropertyCategory = {KUidSystemCategoryValue};

/** Key used when publishing IrDA status notifications. */
const TUint KIrdaStatus = 0x100052d1;


/** Names used for IrDA status notifications. */
namespace TIrdaStatusCodes
	{
	enum
		{
		EIrLoaded,
		EIrDiscoveredPeer,
		EIrLostPeer,
		EIrConnected,
		EIrBlocked,
		EIrDisconnected,
		EIrUnloaded,
		};
	}

/** PnP support - first service hint byte. */
const TUint8 KIrPnPMask=0x01;	   // PnP support - FIRST SERVICE HINT BYTE

/** PDA/Palmtop - first service hint byte. */
const TUint8 KPalmtopMask=0x02;    // PDA/Palmtop - FIRST SERVICE HINT BYTE

/** Computer - first service hint byte. */
const TUint8 KComputerMask=0x04;   // Computer - FIRST SERVICE HINT BYTE

/** Printer - first service hint byte. */
const TUint8 KPrinterMask=0x08;	   // Printer - FIRST SERVICE HINT BYTE

/** IrModem - first service hint byte. */
const TUint8 KModemMask=0x10;      // IrModem - FIRST SERVICE HINT BYTE

/** Fax - first service hint byte. */
const TUint8 KFaxMask=0x20;        // Fax - FIRST SERVICE HINT BYTE

/** LAN Access - first service hint byte. */
const TUint8 KLANAccessMask=0x40;  // LAN Access - FIRST SERVICE HINT BYTE

/** Extension bit - first service hint byte. */
const TUint8 KExtensionMask=0x80;  // Extension bit - FIRST SERVICE HINT BYTE

/** Telephony - second service hint byte. */
const TUint8 KTelephonyMask=0x01;  // Telephony - SECOND SERVICE HINT BYTE

/** File Server - second service hint byte. */
const TUint8 KFileServerMask=0x02; // File Server - SECOND SERVICE HINT BYTE

/** IrCOMM support - second service hint byte. */
const TUint8 KIrCommMask=0x04;     // IrCOMM support - SECOND SERVICE HINT BYTE

/** IrOBEX support - second service hint byte. */
const TUint8 KIrObexMask=0x20;     // IrObex support - SECOND SERVICE HINT BYTE

//
// IrDA Ioctls
//

/** Completes on receipt of a discovery indication. 

Returns log entry in a TNameEntry. 

Ioctl level : KIrdaAddrFamily */
const TUint KDiscoveryIndicationIoctl=0;

/** Completes on successfully making RSocket EXCLUSIVE else returns error. 

Completes successfully or returns KErrDisconnected if failed.

Ioctl level: KIrdaAddrFamily */
const TUint KExclusiveModeIoctl=1;

/** Completes on successfully making RSocket MULTIPLEXED else returns error.

Completes successfully or returns KErrDisconnected if failed.

Ioctl level: KIrdaAddrFamily */
const TUint KMultiplexModeIoctl=2;

/** Completes on doing a status request on IrMUX layer packets to send.

Returns TDes8* holding TUint indicating number of outstanding MUX data requests.

Ioctl level: KIrdaAddrFamily */
const TUint KIrmuxStatusRequestIoctl=3;

/** Completes on doing a status request on IrLAP layer packets to send.

Returns TDes8* holding TUint indicating the number of outstanding LAP data 
requests.

Ioctl level: KIrdaAddrFamily */
const TUint KIrlapStatusRequestIoctl=4;

/** Completes on successfully putting RSocket into IDLE mode.

Completes successfully or returns KErrAbort if failed.

Ioctl level: KIrdaAddrFamily */
const TUint KIdleRequestIoctl=5;

/** Completes on successfully taking RSocket out of IDLE mode.

Completes successfully or returns KErrAbort if failed.

Ioctl level: KIrdaAddrFamily */
const TUint KIdleClearRequestIoctl=6;

/** Completes on receipt of an IrLAP disconnect indication.

Completes successfully on IrLAP or IrMUX disconnect on this connection.

Ioctl level: KIrdaAddrFamily */
const TUint KDisconnectIndicationIoctl=7;

/** Completes on doing a status request on IrLAP layer packets to send.

Returns TDes8* holding TUint indicating the number of outstanding LAP data 
requests.

Ioctl level: KIrdaAddrFamily */
const TUint KIrlapStatusIndicationIoctl=8;

/** Completes on receiving a status indication from IrLAP.

Returns TDes8* holding TUint indicating the number of outstanding MUX data 
requests.

Ioctl level: KIrdaAddrFamily */
const TUint KIrmuxStatusIndicationIoctl=9;

/** Completes on doing an IrLAP link reset.

Does an SNRMP-UAF link reset which can be initiated from either primary or 
secondary.

Ioctl level: KIrdaAddrFamily */
const TUint KIrlapResetRequestIoctl=10;

/** Completes on receipt of an IrLAP link reset indication.

Completes with error value KErrNone if link is successfully reset.

Ioctl level: KIrdaAddrFamily */
const TUint KIrlapResetIndicationIoctl=11;

/** Completes on doing an IrLAP link reset.

Does a DISCP-UAF link disconnect which can be initiated from either primary 
or secondary.

Ioctl level: KIrdaAddrFamily */
const TUint KIrlapDisconnectRequestIoctl=12;

const TUint KExclusiveMode=0;

const TUint KMultiplexMode=1;


//********************  Irda Set/GetOpts ******************************
// These two are done on Socket Write
//


/** KLevelIrlap.

Sets transfer mode to be unexpedited (the default).

Not recommended for use with SetOpt(). Preferred use is with RSocket::Send()

Returns KErrNone */
const TUint KUnexpeditedDataOpt		= 0;		// Default

/** KLevelIrlap

Sets transfer mode to be expedited (urgent).

Not recommended for use with SetOpt(). Preferred use is with RSocket::Send()

Returns KErrNone */
const TUint KExpeditedDataOpt		= 1;		// Urgent data transfer

// On KLevelIrlap

/** KLevelIrlap

Sets number of discovery slots.

Returns KErrNone */
const TUint KDiscoverySlotsOpt		= 2;

/** KLevelIrlap

Sets the requested maximum link baud rate.

Returns KErrInUse, if the link is already running.

Returns with the currently set maximum link baud supported. */
const TUint KUserBaudOpt			= 3;

/** KLevelIrlap

Sets the requested maximum data packet size that can be received by the host.

Returns KErrInUse, if the link is already running.

Returns with the currently set value for the maximum receivable data size 
of the host IrLAP layer. */
const TUint KHostMaxDataSizeOpt		= 4;

/** KLevelIrlap

Returns KErrNotSupported.

Cannot set this value for the remote station.

Returns with the currently set value for the maximum transmissible data size 
to remote IrLAP layer. */
const TUint KRemoteMaxDataSizeOpt	= 6;	// Remote packet size

/** KLevelIrlap

Set the maximum link turnaround time for the host IrLAP layer.

Returns KErrInUse, if the link is already running.

Returns with the currently set value for the host link turnaround time */
const TUint KHostMaxTATimeOpt		= 5;

/** KLevelIrlap

Disables IrLAP level reset_check/wait states */
const TUint KIrlapDisableResetOpt	= 9;	// This disables Irlap level reset_check/wait states.

/** KLevelIrlap

Allows the client to set local busy in IrLAP. */
const TUint KLocalBusyDetectedOpt	= 10;	// Client can set local busy in Irlap

/** KLevelIrlap

Allows the client to clear local busy in IrLAP */
const TUint KLocalBusyClearedOpt	= 11;	// Client can clear local busy in Irlap

/** KLevelIrlap

Disables discovery response for a short period (typically 3 seconds). */
const TUint KDiscoveryResponseDisableOpt = 12;

/** KLevelIrlap

Sets the host's first service hint byte used in XID frames.

Retrieves the first hint byte. */
const TUint KFirstHintByteOpt		= 13;	// Hint Bytes

/** KLevelIrlap

Sets the host's second service hint byte used in XID frames.

Retrieves the second hint byte */
const TUint KSecondHintByteOpt		= 14;

// On KLevelIrmux

/** KLevelIrmux

Turns on transport layer segmentation with the segment size specified. This 
value is advertised to the remote machine as the maximum amount of data we 
can reassemble.

Returns KErrNone */
const TUint KTinyTPLocalSegSizeOpt=7;	// This value is advertised to the remote machine as the max amount of data we can reassemble

/** KLevelIrmux

Queries the remote machine's segment size. The remote machine is unable 
to assemble more data than this.

Returns the remote machine's segment size. */
const TUint KTinyTPRemoteSegSizeOpt=8;	// Remote machine is unable to reassemble more data than this

/**
Disables TinyTP reassembly of segmented packets. */
const TUint KTinyTPDisabledSegmentation=15;
/**
@deprecated
Disables TinyTP reassembly of segmented packets. */
const TUint KTinyTPDiasbledSegmentation=15;

/**
@internalComponent
*/
const TUint KIrdaInternalOption=0x4000000;

/**
@internalTechnology
*/
const TUint KTinyTPSetMaxBufferSpace=16 | KIrdaInternalOption;



/** The option relates to IrLAP. */
const TUint KLevelIrlap=4;

/** The option relates to IrMUX. */
const TUint KLevelIrmux=5;

/** The maximum length of a class name in an IAS entry. */
const TUint KIASClassNameMax=60;

/** The maximum length of an attribute in an IAS entry. */
const TUint KIASAttributeNameMax=60;

/** The maximum length of a TIASQuery descriptor. */
const TUint KMaxQueryStringLength=128;

//################# CLASS DEFINITIONS ########################

NONSHARABLE_CLASS(TIrdaSockAddr) : public TSockAddr
/** An IrDA socket address. 
	@publishedAll 
	@released */
	{
struct SIrdaAddr
	{	
	TUint iHostDevAddr;
	TUint iRemoteDevAddr;
	TBool iSniff;
	TBool iSolicited;
	TUint8 iIrlapVersion;
	TUint8 iFirstServiceHintByte;
	TUint8 iSecondServiceHintByte;
	TUint8 iCharacterSet;
	TUint8 iServiceHintByteCount;
	// Extra stuff for MUX
	TUint8 iHomePort;
	TUint8 iRemotePort;
	TUint8 iSpare;
    };
public:
	IMPORT_C TIrdaSockAddr();
	IMPORT_C TIrdaSockAddr(const TSockAddr &aAddr);
	/** Casts a reference to a general socket address into an IrDA socket address.
	
	@param aAddr A reference to a general socket address. 
	@return The socket address returned as a reference to an IrDA socket address. */
	
	/** Casts a pointer to a general socket address into an IrDA socket address.
	
	@param aAddr A pointer to a general socket address. 
	@return The socket address returned as a reference to an IrDA socket address. */
	IMPORT_C static TIrdaSockAddr &Cast(const TSockAddr &aAddr);

	/** Casts a reference to a general socket address into an IrDA socket address.
	
	@param aAddr A reference to a general socket address. 
	@return The socket address returned as a reference to an IrDA socket address. */
	
	/** Casts a pointer to a general socket address into an IrDA socket address.
	
	@param aAddr A pointer to a general socket address. 
	@return The socket address returned as a reference to an IrDA socket address. */

	IMPORT_C static TIrdaSockAddr &Cast(const TSockAddr *aAddr);
	/** Returns the remote device address.
	
	@return The remote device address as a 32 bit value. */

	IMPORT_C TUint GetRemoteDevAddr() const;
	/** Sets the remote device address.
	
	@param aRemote The remote device address as a 32-bit value. */

	IMPORT_C void SetRemoteDevAddr(const TUint aRemote);
	
	/** Returns the host device address.
	
	@return The host device address as a 32 bit value. */
	IMPORT_C TUint GetHostDevAddr() const;
	
	/** Sets the host device address.
	
	@param aHost The host device address as a 32-bit value. */
	IMPORT_C void SetHostDevAddr(const TUint aHost);
	
	/** Returns the remote device's sniff status, i.e. whether the device is capable 
	of sniffing for IrDA devices.
	
	@return True, if the device is capable of sniffing for IrDA devices; false, 
	otherwise. */
	IMPORT_C TBool GetSniffStatus() const;
	
	/** Sets the sniff status for the remote device, i.e. whether the device is capable 
	of sniffing for IrDA devices.
	
	@param aSniff True, if the device is capable of sniffing for IrDA devices; 
	false, otherwise. */
	IMPORT_C void SetSniffStatus(const TBool aSniff);
	
	/** Returns the solicited status.
	
	The solicited status indicates whether a discovery was initiated by the host 
	device or a remote device.
	
	@return True, if the discovery was initiated by the host device; false if 
	the discovery was initiated by the remote device. */
	IMPORT_C TBool GetSolicitedStatus() const;
	
	/** Sets the solicited status. The solicited status indcates whether a discovery 
	was initiated by the host device or a remote device.
	
	@param aSolicited ETrue, if the discovery was initiated by the host device; 
	EFalse, if discovery was initiated by the remote device. */
	IMPORT_C void SetSolicitedStatus(const TBool aSolicited);
	
	/** Returns the remote device's IrLAP version number.
	
	This should be 0x00 indicating that the device conforms to the IrDA IrLAP 
	1.00 specification.
	
	@return The device's IrLAP version number. */
	IMPORT_C TUint8 GetIrlapVersion() const;
	
	/** Sets the IrLAP version number for the remote device.
	
	This should be 0x00 indicating that the device conforms to the IrDA IrLAP 
	1.00 specification.
	
	@param aIrlapVersion The device's IrLAP version number. */
	IMPORT_C void SetIrlapVersion(const TUint8 aIrlapVersion);
	
	/** Returns the character set supported by the remote device.
	
	@return One of the TIASCharSet enumerator values defining the character set 
	supported by the device. */
	IMPORT_C TUint8 GetCharacterSet() const;
	
	/** Sets the character set supported by the remote device.
	
	@param aCharacterSet One of the TIASCharSet enumerator values defining the 
	character set supported by the device */
	IMPORT_C void SetCharacterSet(const TUint8 aCharacterSet);
	
	/** Returns the remote device's first service hint byte. Service hint bytes indicate 
	the level of IrLMP support provided by the device.
	
	@return The first service hint byte. The individual bits have the following 
	meaning: Bit 0 (0x01) - PnP Compatible Bit 1 (0x02) - PDA/Palmtop Bit 2 (0x04) 
	- Computer Bit 3 (0x08) - Printer Bit 4 (0x10) - Modem Bit 5 (0x20) - Fax 
	Bit 6 (0x40) - LAN Access Bit 7 (0x80) - Extension */
	IMPORT_C TUint8 GetFirstServiceHintByte() const;
	
	/** Sets the first service hint byte for the remote device. The service hint bytes 
	indicate the level of IrLMP support provided by the device.
	
	@param aFirstServiceHintByte The first service hint byte. The individual bits 
	have the following meaning: Bit 0 (0x01) - PnP Compatible Bit 1 (0x02) - PDA/Palmtop 
	Bit 2 (0x04) - Computer Bit 3 (0x08) - Printer Bit 4 (0x10) - Modem Bit 5 
	(0x20) - Fax Bit 6 (0x40) - LAN Access Bit 7 (0x80) - Extension */
	IMPORT_C void SetFirstServiceHintByte(const TUint8 aFirstServiceHintByte);
	
	/** Returns the remote device's first service hint byte. Service hint bytes indicate 
	the level of IrLMP support provided by the device.
	
	@return The second service hint byte. The individual bits have the following 
	meaning: Bit 0 (0x01) - Telephony Bit 1 (0x02) - File Server Bit 2 (0x04) 
	- IrCOMM Bit 3 (0x08) - reserved Bit 4 (0x10) - reserved Bit 5 (0x20) - IrOBEX 
	Bit 6 (0x40) - reserved Bit 7 (0x80) - Extension */
	IMPORT_C TUint8 GetSecondServiceHintByte() const;
	
	/** Sets the second service hint byte for the remote device. The service hint bytes 
	indicate the level of IrLMP support provided by the device.
	
	@param aSecondServiceHintByte The second service hint byte. The individual 
	bits have the following meaning: Bit 0 (0x01) - Telephony Bit 1 (0x02) - File 
	Server Bit 2 (0x04) - IrCOMM Bit 3 (0x08) - reserved Bit 4 (0x10) - reserved 
	Bit 5 (0x20) - IrOBEX Bit 6 (0x40) - reserved Bit 7 (0x80) - Extension */
	IMPORT_C void SetSecondServiceHintByte(const TUint8 aSecondServiceHintByte);
	
	/** Returns the number of service hint bytes for the remote device.
	
	@return The number of service hint bytes. */
	IMPORT_C TUint8 GetServiceHintByteCount() const;
	
	/** Sets the number of service hint bytes for the remote device.
	
	@param aServiceHintByteCount The number of service hint bytes. */
	IMPORT_C void SetServiceHintByteCount(const TUint8 aServiceHintByteCount);
	IMPORT_C TUint8 GetHomePort() const;
	IMPORT_C void SetHomePort(const TUint8 aHomePort);
	IMPORT_C TUint8 GetRemotePort() const;
	IMPORT_C void SetRemotePort(const TUint8 aRemotePort);
private:
	SIrdaAddr* addrPtr() const;
	};

enum TIASDataType
/** An enumeration whose enumerators define the type of response received from 
an Information Access Service (IAS) query.

@see TIASResponse */
{
	/** No response type defined. */
	EIASDataMissing=0,
	/** The response type is an integer. */
	EIASDataInteger=1,
	/** The response type is binary data. */
	EIASDataOctetSequence=2,
	/** The response type is a string. */
	EIASDataUserString=3
};

enum TIASCharSet
/** The character set encoding of the character string response from an IAS query.

@see TIASResponse::GetCharString()
@see TIrdaSockAddr::GetCharacterSet() */
	{ 
	/** String is standard ASCII. */
	EIASCharSetUserStringASCII =0x00,
	/** String is ISO Latin-1. */
	EIASCharSetUserStringISO_8859_1=0x01,
	/** String is ISO Latin-2. */
	EIASCharSetUserStringISO_8859_2=0x02,
	/** String is ISO Latin-3. */
	EIASCharSetUserStringISO_8859_3=0x03,
	/** String is ISO Latin-4. */
	EIASCharSetUserStringISO_8859_4=0x04,
	/** String is ISO-8859-5 (Cyrillic). */
	EIASCharSetUserStringISO_8859_5=0x05,
	/** String is ISO-8859-6 (Arabic). */
	EIASCharSetUserStringISO_8859_6=0x06,
	/** String is ISO-8859-7 (Greek). */
	EIASCharSetUserStringISO_8859_7=0x07,
	/** String is ISO-8859-8 (Hebrew). */
	EIASCharSetUserStringISO_8859_8=0x08,
	/** String is ISO Latin-5. */
	EIASCharSetUserStringISO_8859_9=0x09,
	/** String is UNICODE. */
	EIASCharSetUserStringUnicode=0xFF,
	};

NONSHARABLE_CLASS(TIASQuery) : public TBuf8<KMaxQueryStringLength>
/** An Information Access Service (IAS) query to another device's IAS server.
@publishedAll
@released
@see TIASResponse */
	{
public:
	IMPORT_C TIASQuery(const TDesC8& aClass,const TDesC8& aAttribute,TUint aRemoteDevAddr);
	IMPORT_C TIASQuery();
	IMPORT_C void Set(const TDesC8& aClass,const TDesC8& aAttribute,TUint aRemoteDevAddr);
	IMPORT_C void Get(TDes8& aClass,TDes8& aAttribute,TUint& aRemoteDevAddr);
private:
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 
	};

NONSHARABLE_CLASS(TIASResponse) : public TBuf8<KMaxQueryStringLength>
/** Response from an Information Access Service (IAS) query to another device's 
IAS server.
@publishedAll
@released
@see TIASQuery */
	{
public:		// But not exported :-)
	void SetToInteger(TUint anInteger);
	void SetToCharString(const TDesC8& aCharString);
	void SetToOctetSeq(const TDesC8& aData);
#ifdef _UNICODE
	void SetToCharString(const TDesC16& aWideString);
#endif

public:
	IMPORT_C TIASResponse();
/** Indicates the response contains a list. */
	IMPORT_C TBool IsList() const;
/** Indicates the number of items in the response. */
	IMPORT_C TInt NumItems() const;

/** Returns the type of the response.
	
	@return An enumeration identifying the type of response. */
	IMPORT_C TIASDataType Type() const;

	IMPORT_C TInt GetInteger(TInt &aResult,TInt anIndex=0) const;
	IMPORT_C TInt GetOctetSeq(TDes8 &aResult,TInt anIndex=0) const;
	IMPORT_C TInt GetCharString(TDes8 &aResult,TInt anIndex=0) const;
	IMPORT_C const TPtrC8 GetCharString8(TInt anIndex=0) const;
	
	/** Gets the response string.
	
	This is called if the response type indicates a string.
	
	@param aResult On return, an 8 bit modifiable descriptor containing the response 
	string. The length of the response string can never be greater than the value 
	of (KMaxQueryStringLength - 3). 
	@param anIndex Reserved for future use. This argument must be allowed to default 
	to 0 and must not be overriden. 
	@return If successful, one of the TIASCharSet enumerator values defining the 
	character set encoding of the response string. KErrNotSupported, if a non-zero 
	value has been specified for anIndex. KErrCorrupt, if the response type is 
	not a string. 
	@see TIASResponse::Type() */
	IMPORT_C TInt GetCharString(TDes16 &aResult,TInt anIndex=0) const;

	IMPORT_C const TPtrC16 GetCharString16(TInt anIndex=0) const;
	
private:
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1;
	TUint32     iPadding2;	
	};

NONSHARABLE_CLASS(TIASDatabaseEntryV001)
/** The data for an Information Access Service (IAS) record in the network database.

An object of this type is contained in a packaged modifiable buffer descriptor.
@publishedAll
@released
@see TIASDatabaseEntry */
	{
public:
	IMPORT_C TIASDatabaseEntryV001();
	
	/** The class name for the IAS entry
	
	This is a template specialisation of a TBuf8<TInt> */
	TBuf8<KIASClassNameMax> iClassName;
	
	/** The attribute for the IAS entry.
	
	This is a template specialisation of a TBuf8<TInt> */
	TBuf8<KIASAttributeNameMax> iAttributeName;
	
	/** The response data. */
	TIASResponse iData;
	
private:
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};
	
NONSHARABLE_CLASS(TIASDatabaseEntry) : public TPckgBuf<TIASDatabaseEntryV001>
/** An Information Access Service (IAS) record in the network database.

The record data is contained in an object of type TIASDatabaseEntryV001 packaged 
in a modifiable buffer descriptor.
@publishedAll
@released
@see RNetDatabase */
	{
public:
	IMPORT_C void SetClassName(const TDesC8& aClassName);
	IMPORT_C void SetAttributeName(const TDesC8& anAttributeName);
	IMPORT_C void SetToInteger(const TUint anInteger);
	IMPORT_C void SetToCharString(const TDesC8& aCharString);
	
	/** Sets the response type corresponding to the class and attribute defined in 
	this record, as binary data.
	
	@param aData The response binary data. The length of this data can never be 
	greater than (KMaxQueryStringLength - 3). */
	IMPORT_C void SetToOctetSeq(const TDesC8& aData);

#ifdef _UNICODE	//need to be able to enter wide entries in unicode builds
	IMPORT_C void SetToCharString(const TDesC16& aWideString);//new export
#endif
	};

#endif // __IR_SOCK_H__
