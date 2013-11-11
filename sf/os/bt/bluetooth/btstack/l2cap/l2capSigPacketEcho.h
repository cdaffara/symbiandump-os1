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


#ifndef L2CAPSIGPACKETECHO_H
#define L2CAPSIGPACKETECHO_H

#include "l2capCommand.h"

// Interface class used when discovering the originator of the
// Echo Request Command.
class MEchoResponseHandler
	{
public:
	virtual void EchoResponseReceived(const TDesC8* aData) = 0;
	};
	
	
NONSHARABLE_CLASS(HEchoRequest) : public HL2CapCommand
	{
public:
	static HEchoRequest* New(RMBufChain& aData,
		  	                 TUint8 aRTXTimerDuration = KDefaultRTXTimerDuration,
	                         TUint16 aERTXTimerDuration = KDefaultERTXTimerDuration);
	
	static HEchoRequest* New(TUint8 aRTXTimerDuration = KDefaultRTXTimerDuration,
	                         TUint16 aERTXTimerDuration = KDefaultERTXTimerDuration);
	                         
	static TInt NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand);

	~HEchoRequest();

	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler);

	TInt GetData(RMBufChain& aData) const;

	inline void SetEchoResponseHandler(MEchoResponseHandler& aHandler);
	inline void RemoveEchoResponseHandler();
	inline MEchoResponseHandler* EchoResponseHandler() const;

public:
	const static TUint8 KEchoRequestEmptyLength		= 4;

private:
	HEchoRequest(RMBufChain& aCommand,
                 TUint8 aRTXTimerDuration = KDefaultRTXTimerDuration,
	             TUint16 aERTXTimerDuration = KDefaultERTXTimerDuration);

private:
	const static TUint8 KDataOffset					= 4;

private:		
	MEchoResponseHandler* iEchoResponseHandler;
	};


inline void HEchoRequest::SetEchoResponseHandler(MEchoResponseHandler& aHandler)
	{
	iEchoResponseHandler = &aHandler;
	}
	
inline void HEchoRequest::RemoveEchoResponseHandler()
	{
	// The echo response handler no longer wishes to be informed
	// when the response arrives.
	iEchoResponseHandler = NULL;
	}

inline MEchoResponseHandler* HEchoRequest::EchoResponseHandler() const
	{
	return iEchoResponseHandler;
	}


NONSHARABLE_CLASS(HEchoResponse) : public HL2CapCommand
	{
public:
	static HEchoResponse* New(TUint8 aId);
	static HEchoResponse* New(RMBufChain& aData, TUint8 aId);
	static TInt NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand);

	~HEchoResponse();

	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler);

	TInt GetData(RMBufChain& aData) const;

private:
	HEchoResponse(RMBufChain& aCommand);

private:
	const static TUint8 KEchoResponseEmptyLength	= 4;
	const static TUint8 KDataOffset					= 4;
	};

#endif
