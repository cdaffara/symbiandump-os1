// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The base class for L2Cap Commands
// 
//

#ifndef L2CAPCOMMAND_H
#define L2CAPCOMMAND_H

#include <es_mbuf.h>
#include <bt_sock.h>

#include "l2util.h"
#include "l2constants.h"
#include "L2types.h"


class CL2CapSignalHandler;
class CL2CAPMux;

NONSHARABLE_CLASS(HL2CapCommand)
	{
public:
	// Message format lengths and offsets.
	const static TUint8 KCodeOffset					= 0;
	const static TUint8 KIdOffset					= 1;
	const static TUint8 KLengthOffset 				= 2;
	const static TUint8 KL2CapCommandHeaderLength 	= 4;		

	// RTX / ERTX Timer values.
	const static TUint8 KDefaultRTXTimerDuration	= 5;	// Second(s)
	const static TUint8 KDefaultERTXTimerDuration	= 120;	// Second(s)
	const static TUint8 KMinRTXTimerDuration		= 1;
	const static TUint8 KMinERTXTimerDuration		= 60;
	const static TUint8 KMaxRTXTimerDuration		= 60;
	const static TUint16 KMaxERTXTimerDuration		= 300;
	const static TUint8 KNumberOfRetransmissions	= 1;

	// Used to specify that the ID has not been set yet.
	const static TUint8 KUninitialisedID			= 0;

	enum TRTXTimerType
		{
		ERTXTimer,
		EERTXTimer,
		};
			
	static TInt DecodeCommand(RMBufChain& aCommandString, HL2CapCommand*& aCommand);

	virtual ~HL2CapCommand();
	HL2CapCommand(RMBufChain& aCommand, 
	              TUint8 aRTXTimerDuration = KDefaultRTXTimerDuration,
	              TUint16 aERTXTimerDuration = KDefaultERTXTimerDuration);

	// Pure virtual that is used to ensure that derived class have a mechanism to process themselves in
	// signal handler
	virtual	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler) = 0;

	// This is called to pull an outgoing L2CAP command.  This method will initialise
	// the CID if one has not yet been allocated. 
	TInt GetCommand(CL2CAPMux& aMuxer, RMBufChain& aBuffer);
	
	inline TBool IsRequest() const;

	inline TSignallingCommand Code() const;
	inline void SetCode(TSignallingCommand aCode);
	
	inline TUint8 ID() const;
	inline void SetID(TUint8 aID);

	inline TInt DataLength() const;
	inline void WriteDataLength();

	inline TInt CommandLength() const;

	void StartRTXTimer(TRTXTimerType aType, CL2CapSignalHandler& aSignalHandler);
	static TInt ResponseTimerExpired(TAny* aL2CapCommand);
	
	static void PutLittleEndian32(TUint32 aValue, TInt aOffset, RMBufChain& aCommand);
	static void PutLittleEndian16(TUint16 aValue,TInt aOffset, RMBufChain& aCommand);
	static void PutByte(TUint8 aByte, TInt aOffset, RMBufChain& aCommand);

	static TUint32 GetLittleEndian32(TInt aOffset, const RMBufChain& aCommand);
	static TUint16 GetLittleEndian16(TInt aOffset, const RMBufChain& aCommand);
	static TUint8 GetByte(TInt aOffset, const RMBufChain& aCommand);

	TBool StatefulResponse();
protected:
	// RTx / ERTx Timer methods.
	void CancelResponseTimer();
	void HandleResponseTimerExpiry();
	
//members	
public:
	//Link member so that CCFramePDU can hold a linked list of these commands
	TDblQueLink iLink;

protected:
	RMBufChain iCommand;
	TBool iResponseTimerRunning;
	TDeltaTimerEntry iResponseTimerEntry;
	CL2CapSignalHandler* iResponseSignalHandler;
	TUint8 iRetransmissionCounter;
	
	TUint8 iRTXTimerDuration;
	TUint16 iERTXTimerDuration;
	};


NONSHARABLE_CLASS(HInvalidCommand) : public HL2CapCommand
	{
public:
	HInvalidCommand(RMBufChain& aCommand);
	~HInvalidCommand();

	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler);
	};


inline TBool HL2CapCommand::IsRequest() const
	{
	// Every request command code has the LSB clear.
	return (!(Code() & 0x01));
	}

inline TSignallingCommand HL2CapCommand::Code() const
	{
	return TSignallingCommand(GetByte(KCodeOffset, iCommand));
	}
	
inline void HL2CapCommand::SetCode(TSignallingCommand aCode)
	{
	PutByte(static_cast<TUint8>(aCode), KCodeOffset, iCommand);
	}

inline TUint8 HL2CapCommand::ID() const
	{
	return GetByte(KIdOffset, iCommand);
	}
	
inline void HL2CapCommand::SetID(TUint8 aID)
	{
	PutByte(aID, KIdOffset, iCommand);
	}
	
inline TInt HL2CapCommand::DataLength() const
	{
	return GetLittleEndian16(KLengthOffset, iCommand);
	}

inline void HL2CapCommand::WriteDataLength()
	{
	PutLittleEndian16(static_cast<TUint16>(iCommand.Length() - KL2CapCommandHeaderLength), KLengthOffset, iCommand);
	}

inline TInt HL2CapCommand::CommandLength() const
	{
	return iCommand.Length();
	}
	

#endif
	
