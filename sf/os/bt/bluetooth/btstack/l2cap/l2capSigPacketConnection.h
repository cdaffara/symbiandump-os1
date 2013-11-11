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

#ifndef L2CAPSIGPACKETCONNECTION_H
#define L2CAPSIGPACKETCONNECTION_H

#include "l2capCommand.h"


NONSHARABLE_CLASS(HConnectionRequest) : public HL2CapCommand
	{
public:
	static HConnectionRequest* New(TL2CAPPort aLocalPort, TL2CAPPort aRemotePort,
                                   TUint8 aRTXTimerDuration = KDefaultRTXTimerDuration,
	                               TUint16 aERTXTimerDuration = KDefaultERTXTimerDuration);
    static TInt NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand);
	
	~HConnectionRequest();

	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler);
	
	inline TL2CAPPort SourceCID() const;	// Returns the source channel id
	inline TL2CAPPort PSM() const;			// Returns the requested PSM.
	inline void SetSourceCID(TL2CAPPort aPort);
	inline void SetPSM(TL2CAPPort aPSM);

private:
	HConnectionRequest(RMBufChain& aCommand,
                       TUint8 aRTXTimerDuration = KDefaultRTXTimerDuration,
	                   TUint16 aERTXTimerDuration = KDefaultERTXTimerDuration);

private:
	const static TUint8 KConnectRequestLength		= 8;
	const static TUint8 KPSMOffset					= 4;
	const static TUint8 KSourceCIDOffset			= 6;
	};

inline TL2CAPPort HConnectionRequest::SourceCID() const
	{
	return GetLittleEndian16(KSourceCIDOffset, iCommand);
	}
	
inline TL2CAPPort HConnectionRequest::PSM() const
	{
	return GetLittleEndian16(KPSMOffset, iCommand);
	}

inline void HConnectionRequest::SetSourceCID(TL2CAPPort aPort)
	{
	PutLittleEndian16(aPort, KSourceCIDOffset, iCommand);
	}
	
inline void HConnectionRequest::SetPSM(TL2CAPPort aPSM)
	{
	PutLittleEndian16(aPSM, KPSMOffset, iCommand);
	}

	
	

NONSHARABLE_CLASS(HConnectionResponse) :public HL2CapCommand
	{
public:
	static HConnectionResponse* New(TUint8 aId, TL2CAPPort aLocalPort, TL2CAPPort aRemotePort, TConnectResponseResult aResult, TConnectResponseStatus aStatus = EConnectPendNoFurtherInfo);
	static TInt NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand);
	~HConnectionResponse();

	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler);
	
	inline TL2CAPPort SourceCID() const;			// Returns the source channel id
	inline TL2CAPPort DestinationCID() const;		// Returns the destination id.
	inline TConnectResponseResult Result() const;	// Returns the result field	
	inline TConnectResponseStatus Status() const;	// Returns the status field	

	inline void SetSourceCID(TL2CAPPort aPort);
	inline void SetDestinationCID(TL2CAPPort aPort);
	inline void SetResult(TConnectResponseResult aResult);
	inline void SetStatus(TConnectResponseStatus aStatus);

private:
	HConnectionResponse(RMBufChain& aCommand);

private:
	const static TUint8 KConnectResponseLength		= 12;

	const static TUint8 KDestinationCIDOffset		= 4;
	const static TUint8 KSourceCIDOffset			= 6;
	const static TUint8 KResultOffset				= 8;
	const static TUint8 KStatusOffset				= 10;
	};

inline TL2CAPPort HConnectionResponse::SourceCID() const
	{
	return GetLittleEndian16(KSourceCIDOffset, iCommand);
	}	

inline TL2CAPPort HConnectionResponse::DestinationCID() const
	{
	return GetLittleEndian16(KDestinationCIDOffset, iCommand);
	}

inline TConnectResponseResult HConnectionResponse::Result() const
	{
	return TConnectResponseResult(GetLittleEndian16(KResultOffset, iCommand));
	}

inline TConnectResponseStatus HConnectionResponse::Status() const
	{
	return TConnectResponseStatus(GetLittleEndian16(KStatusOffset, iCommand));
	}

inline void HConnectionResponse::SetSourceCID(TL2CAPPort aPort)
	{
	PutLittleEndian16(aPort, KSourceCIDOffset, iCommand);
	}
	
inline void HConnectionResponse::SetDestinationCID(TL2CAPPort aPort)
	{
	PutLittleEndian16(aPort, KDestinationCIDOffset, iCommand);
	}
	
inline void HConnectionResponse::SetResult(TConnectResponseResult aResult)
	{
	PutLittleEndian16(static_cast<TUint16>(aResult), KResultOffset, iCommand);
	}

inline void HConnectionResponse::SetStatus(TConnectResponseStatus aStatus)
	{
	PutLittleEndian16(static_cast<TUint16>(aStatus), KStatusOffset, iCommand);
	}
		
#endif
