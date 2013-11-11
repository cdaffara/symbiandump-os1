// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef HCIFRAME_H
#define HCIFRAME_H

#include <e32std.h>
#include <e32base.h>
#include <bttypes.h>
#include <bluetooth/hci/hcitypes.h>
#include <bluetooth/hci/aclpacketconsts.h>
#include <bttypespartner.h>

/**
Class representing an Hctl Frame as specified in the Bluetooth Core Specification, HCI Section
This is abstract and the different frame types (Command, ACL, SCO etc) derive from it
*/
NONSHARABLE_CLASS(CHctlFrameBase) : public CBase
	{
public:
	~CHctlFrameBase();
	
protected:
	CHctlFrameBase(TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize);
	void ConstructL(TUint32 aHctlFrameSize);
	
protected:
	const TUint8 iHctlHeaderSize;
	const TUint8 iHctlTrailerSize;

	TPtr8 iFramePtr;
	HBufC8*	iHctlFrame;
	};

/**
Class representing an Hctl Command Frame as specified in the Bluetooth Core Specification, HCI Section
*/
NONSHARABLE_CLASS(CHctlCommandFrame) : public CHctlFrameBase
	{
public:
	IMPORT_C static CHctlCommandFrame* NewL(TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize);

	IMPORT_C void FinaliseCommand();
	IMPORT_C void ResetCommand();

	IMPORT_C void SetOpcode(THCIOpcode aOpcode);
	IMPORT_C void PutByte(TUint8 aByte);
	IMPORT_C void PutBytes16(TUint16 aVal);
	IMPORT_C void PutBytes24(TUint32 aVal);
	IMPORT_C void PutBytes32(TUint32 aVal);
	IMPORT_C void PutBytes32(TUint32 aVal,TUint8 aNumOfBytes);
	IMPORT_C void PutString(const TDesC8& aString);
	IMPORT_C void PutPaddedString(const TDesC8& aString, TInt aRequiredLength);
	IMPORT_C void PutDevAddr(const TBTDevAddr& aBdaddr);
	IMPORT_C void PutAFHHostChannelClassification(const TDesC8& aClassification);
	IMPORT_C void PutLinkKey(const TBTLinkKey& aLinkKey);
	IMPORT_C void PutConnectionHandle(THCIConnectionHandle aConnectionHandle);
	IMPORT_C void PutSimplePairingHash(const TBluetoothSimplePairingHash& aHash);
	IMPORT_C void PutSimplePairingRandomizer(const TBluetoothSimplePairingRandomizer& aRandomizer);
	
	
	IMPORT_C const TDesC8& HCTLPayload() const;
	
private:
	CHctlCommandFrame(TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize);

public:
	// Hctl Command specific constants
	const static TUint8 KHCICommandLengthFieldOffset 	= 2;
	const static TUint8 KHCICommandLengthFieldLength 	= 1;

	const static TUint8 KHCICommandPacketHeaderLength 	= 3;
	const static TInt KHCIMaxCommandLength				= 255;
	};

/**
Class representing an Hctl Data Frame as specified in the Bluetooth Core Specification, HCI Section
This is abstract and the different data types (ACL, SCO etc) derive from it
*/
NONSHARABLE_CLASS(CHctlDataFrameBase) : public CHctlFrameBase
	{
public:
	IMPORT_C void SetConnectionHandle(THCIConnHandle aConnectionHandle);
	IMPORT_C static THCIConnHandle ConnectionHandle(const TDesC8& aHCIDataFrame);
	IMPORT_C const TDesC8& HCTLPayload() const;

	virtual void SetDataPayload(const TDesC8& aData)=0;
	
protected:
	CHctlDataFrameBase(TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize);

public:
	const static TUint8 KHCIDataPacketLengthFieldOffset = 2;
	};


/**
Class representing an Hctl ACL Data Frame as specified in the Bluetooth Core Specification, HCI Section
*/
NONSHARABLE_CLASS(CHctlAclDataFrame) : public CHctlDataFrameBase
	{
public:
	IMPORT_C static CHctlAclDataFrame* NewL(TUint16 aPayloadSize, TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize);

	IMPORT_C static TAclPacketBoundaryFlag PacketBoundaryFlag(const TDesC8& aData);
	IMPORT_C static TAclPacketBroadcastFlag PacketBroadcastFlag(const TDesC8& aData);
	
	IMPORT_C void SetFlags(TAclPacketBoundaryFlag aBoundaryFlag, TAclPacketBroadcastFlag aBroadcastFlag);
	IMPORT_C void SetDataPayload(const TDesC8& aData);
	IMPORT_C THCIConnHandle ConnectionHandle() const;

public:
	// Hctl ACL Data specific constants
	const static TUint8 KHCIACLDataPacketLengthFieldLength = 2;

	const static TUint8 KHCIACLDataPacketHeaderLength = 4; // 2 bytes ConnH, 2 bytes data length field
	const static TUint16 KHCTLMaxACLDataSize = 0xffff;

private:
	CHctlAclDataFrame(TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize);
	void Finalise();
	};

/**
Class representing an Hctl Synchronous Data Frame as specified in the Bluetooth Core Specification, HCI Section
*/
NONSHARABLE_CLASS(CHctlSynchronousDataFrame) : public CHctlDataFrameBase
	{
public:
	IMPORT_C static CHctlSynchronousDataFrame* NewL(TUint8 aBufSize, TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize);
	virtual void SetDataPayload(const TDesC8& aData);

public:
	// HCI SCO Data specific constants
	const static TUint8 KHCISCODataPacketLengthFieldLength = 1;

	const static TUint8 KHCISynchronousDataPacketHeaderLength = 3; // 2 bytes ConnH, 1 byte data length field 
	const static TUint8 KHCTLMaxSynchronousDataSize = 0xff;
private:
	CHctlSynchronousDataFrame(TUint8 aHctlHeaderSize, TUint8 aHctlTrailerSize);
	void Finalise();
	};

#endif // HCIFRAME_H
