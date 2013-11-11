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

#ifndef L2CAPSIGPACKETINFORMATION_H
#define L2CAPSIGPACKETINFORMATION_H

#include "l2capCommand.h"

NONSHARABLE_CLASS(HInformationRequest) : public HL2CapCommand
	{
public:
	static HInformationRequest* New(TInfoType aInfoType,
					                TUint8 aRTXTimerDuration = KDefaultRTXTimerDuration,
	             					TUint16 aERTXTimerDuration = KDefaultERTXTimerDuration);
	             					
	static TInt NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand);
	
	~HInformationRequest();

	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler);

	inline TInfoType InfoType() const;
	inline void SetInfoType(TInfoType aInfoType);

private:
	HInformationRequest(RMBufChain& aCommand,
						TUint8 aRTXTimerDuration = KDefaultRTXTimerDuration,
						TUint16 aERTXTimerDuration = KDefaultERTXTimerDuration);	
private:
	const static TUint8 KInformationRequestLength	= 6;

	const static TUint8 KInfoTypeOffset				= 4;
	};

inline TInfoType HInformationRequest::InfoType() const
	{
	return static_cast<TInfoType>(GetLittleEndian16(KInfoTypeOffset, iCommand));
	}

inline void HInformationRequest::SetInfoType(TInfoType aInfoType)
	{
	PutLittleEndian16(static_cast<TUint16>(aInfoType), KInfoTypeOffset, iCommand);
	}


	
NONSHARABLE_CLASS(HInformationResponse) :public HL2CapCommand
	{
public:
	static HInformationResponse* New(TInfoType aInfoType, TInfoReqResult aResult, TUint8 aId, TUint32 aExtendedFeatures);
	static HInformationResponse* New(TInfoType aInfoType, TInfoReqResult aResult, TUint8 aId);
	static TInt NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand);
	~HInformationResponse();

	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler);

	inline TInfoType InfoType() const;
	inline TInfoReqResult Result() const;
	inline void SetInfoType(TInfoType aInfoType);
	inline void SetResult(TInfoReqResult aResult);
	inline void SetExtendedFeatureMask(TUint32 aFeatureMask);
	
	TL2CapEntityInfo RemoteExtendedFeatureMask() const;

private:
	HInformationResponse(RMBufChain& aCommand);
	static TBool VerifyStructure(const RMBufChain& aCommand);

private:
	const static TUint8 KInfoTypeOffset						= 4;
	const static TUint8 KResultOffset						= 6;
	const static TUint8 KDataOffset							= 8;
	
	const static TUint8 KConnectionlessMTULength			= 2;
	const static TUint8 KExtendedFeatureMaskLength			= 4;

	const static TUint8 KInformationResponseMinLength		= 8;
	const static TUint8 KInformationResponseMaxLength		= KInformationResponseMinLength + KExtendedFeatureMaskLength;
	};

inline TInfoType HInformationResponse::InfoType() const
	{
	return static_cast<TInfoType>(GetLittleEndian16(KInfoTypeOffset, iCommand));
	}

inline TInfoReqResult HInformationResponse::Result() const
	{
	return static_cast<TInfoReqResult>(GetLittleEndian16(KResultOffset, iCommand));
	}

inline void HInformationResponse::SetInfoType(TInfoType aInfoType)
	{
	PutLittleEndian16(static_cast<TUint16>(aInfoType), KInfoTypeOffset, iCommand);
	}
	
inline void HInformationResponse::SetResult(TInfoReqResult aResult)
	{
	PutLittleEndian16(static_cast<TUint16>(aResult), KResultOffset, iCommand);
	}

inline void HInformationResponse::SetExtendedFeatureMask(TUint32 aFeatureMask)
	{
	PutLittleEndian32(aFeatureMask, KDataOffset, iCommand);
	}
	

#endif
