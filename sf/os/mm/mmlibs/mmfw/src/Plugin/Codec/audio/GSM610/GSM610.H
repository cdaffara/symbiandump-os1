// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __GSM610_H__
#define __GSM610_H__


//*******************************************************************
//* INCLUDE FILES:
//*******************************************************************

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <mmf/server/mmfcodec.h>
#include <mmf/server/mmfdatabuffer.h>
#include <e32std.h>

#include "gsm610fr.h"

const TUint	KGsmEncodedFrameSize = 65;

const TUint	KPcmEncodedFrameSize = 160*2;
const TUint	KPcmInputFrameSize   = KPcmEncodedFrameSize*2;	// cache 2 frames


// Internal to Symbian
// Used to be TMMFPtr8 - useful aid to porting
class TMMFPtr8 : public TPtr8
	{
public:
	TMMFPtr8()
		: TPtr8(0,0,0) {};
	inline void Set(const TDes8& aDes)
		{ TPtr8::Set((TUint8*)(aDes.Ptr()),aDes.Length(),aDes.MaxLength()); };
	inline void SetLengthOnly(const TDes8& aDes)
		{ TPtr8::Set((TUint8*)(aDes.Ptr()),aDes.Length(),aDes.Length()); };
	inline void Set(const TPtrC8& aDes)
		{ TPtr8::Set((TUint8*)(aDes.Ptr()),aDes.Length(),aDes.Length()); };
	inline void Shift(TInt aOffset)
		{ SetLength(Length()-aOffset); iMaxLength-=aOffset; iPtr+=aOffset; };
	};

//*******************************************************************
//* GSM to 16 bit PCM Codec Class:
//*******************************************************************

class CGsmTo16PcmWavCodec : public CBase
	{
public:
	CGsmTo16PcmWavCodec();
	~CGsmTo16PcmWavCodec();

	void ConstructL();

	void Reset();
	void ResetAllL();
	void ProcessL(TMMFPtr8* aSrc, TMMFPtr8* aDst);

private:
	TUint    iOutBufferCount;
	TUint8* iOutBufferPtr;
	TUint8  iOutBuffer [KPcmInputFrameSize];
	TUint    iInBufferCount;
	TUint8* iInBufferPtr;
	TUint8  iInBuffer [KGsmEncodedFrameSize];


	CGSM610FR_Decoder* iGsmDecoder;
	};


//*******************************************************************
//* 16 bit PCM to GSM Codec Class:
//*******************************************************************

class C16PcmToGsmWavCodec : public CBase
	{
public:
	C16PcmToGsmWavCodec();
	~C16PcmToGsmWavCodec();
	void ConstructL();

	void Reset();
	void ResetAllL();
	void ProcessL(TMMFPtr8* aSrc, TMMFPtr8* aDst);
	
private:
	TUint    iOutBufferCount;
	TUint8* iOutBufferPtr;
	TUint8  iOutBuffer [KGsmEncodedFrameSize];
	TUint    iInBufferCount;
	TUint8* iInBufferPtr;
	TUint8  iInBuffer [KPcmInputFrameSize];

	CGSM610FR_Encoder* iGsmEncoder;

	};

class CMMFGsmTo16PcmCodec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFGsmTo16PcmCodec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);

private:
	CMMFGsmTo16PcmCodec();
	void ConstructL(TAny* aInitParams);
private:
	//buffers owned by the datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;
	TUint iLastFrameNumber;

	//the real codec, CMMFGsmTo16PcmCodec is just a wrapper
	CGsmTo16PcmWavCodec* iCodecPtr;
	};


class CMMF16PcmToGsmCodec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMF16PcmToGsmCodec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);

private:
	CMMF16PcmToGsmCodec();
	void ConstructL(TAny* aInitParams);
private:
	//buffers owned by the datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;
	TUint iLastFrameNumber;

	//the real codec, is CMMF16PcmToGsmCodec just a wrapper
	C16PcmToGsmWavCodec* iCodecPtr;
	};



#endif
