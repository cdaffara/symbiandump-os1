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
//

#ifndef L2CAPSIGPACKETDISCONNECTION_H
#define L2CAPSIGPACKETDISCONNECTION_H

#include "l2capCommand.h"

NONSHARABLE_CLASS(HDisconnectRequest) : public HL2CapCommand
	{
public:
	static HDisconnectRequest* New(TL2CAPPort aLocalPort, TL2CAPPort aRemotePort,
	                   			   TUint8 aRTXTimerDuration = KDefaultRTXTimerDuration,
	                   			   TUint16 aERTXTimerDuration = KDefaultERTXTimerDuration);
	static TInt NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand);
	
	~HDisconnectRequest();

	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler);
	
	inline TL2CAPPort SourceCID() const;			// Returns the source channel id
	inline TL2CAPPort DestinationCID() const;		// Returns the destination id.
	inline void SetSourceCID(TL2CAPPort aPort);
	inline void SetDestinationCID(TL2CAPPort aPort);

private:
	HDisconnectRequest(RMBufChain& aCommand,
	                   TUint8 aRTXTimerDuration = KDefaultRTXTimerDuration,
	                   TUint16 aERTXTimerDuration = KDefaultERTXTimerDuration);
	
private:
	const static TUint8 KDisconnectRequestLength	= 8;

	const static TUint8 KDestinationCIDOffset		= 4;
	const static TUint8 KSourceCIDOffset			= 6;
	};

inline TL2CAPPort HDisconnectRequest::SourceCID() const
	{
	return GetLittleEndian16(KSourceCIDOffset, iCommand);
	}	

inline TL2CAPPort HDisconnectRequest::DestinationCID() const
	{
	return GetLittleEndian16(KDestinationCIDOffset, iCommand);
	}

inline void HDisconnectRequest::SetSourceCID(TL2CAPPort aPort)
	{
	PutLittleEndian16(aPort, KSourceCIDOffset, iCommand);
	}
	
inline void HDisconnectRequest::SetDestinationCID(TL2CAPPort aPort)
	{
	PutLittleEndian16(aPort, KDestinationCIDOffset, iCommand);
	}


	
NONSHARABLE_CLASS(HDisconnectResponse) :public HL2CapCommand
	{
public:
	static HDisconnectResponse* New(TUint8 aId, TL2CAPPort aLocalPort, TL2CAPPort aRemotePort);
	static TInt NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand);

	~HDisconnectResponse();

	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler);
	
	inline TL2CAPPort SourceCID() const;			// Returns the source channel id
	inline TL2CAPPort DestinationCID() const;		// Returns the destination id.
	inline void SetSourceCID(TL2CAPPort aPort);
	inline void SetDestinationCID(TL2CAPPort aPort);

private:
	HDisconnectResponse(RMBufChain& aCommand);

private:
	const static TUint8 KDisconnectResponseLength	= 8;

	const static TUint8 KDestinationCIDOffset		= 4;
	const static TUint8 KSourceCIDOffset			= 6;
	};

inline TL2CAPPort HDisconnectResponse::SourceCID() const
	{
	return GetLittleEndian16(KSourceCIDOffset, iCommand);
	}	

inline TL2CAPPort HDisconnectResponse::DestinationCID() const
	{
	return GetLittleEndian16(KDestinationCIDOffset, iCommand);
	}

inline void HDisconnectResponse::SetSourceCID(TL2CAPPort aPort)
	{
	PutLittleEndian16(aPort, KSourceCIDOffset, iCommand);
	}
	
inline void HDisconnectResponse::SetDestinationCID(TL2CAPPort aPort)
	{
	PutLittleEndian16(aPort, KDestinationCIDOffset, iCommand);
	}

#endif
