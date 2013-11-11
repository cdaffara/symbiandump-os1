// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <mmf/server/mmfswcodecwrapper.h>

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <mmf/server/mmfdatabuffer.h>
#include <e32std.h>


#ifdef EABI  
const TUint KNumFramesInABuffer  = 50;
#else
const TUint KNumFramesInABuffer  = 6;	
#endif	
const TInt  KGsmFrameSize = 65;
const TUint	KGsmEncodedFrameSize = KNumFramesInABuffer * KGsmFrameSize ;   //50 * 65 encoded frames
const TUint	KPcmEncodedFrameSize = KNumFramesInABuffer * 320;  // 50 times 160*2
const TInt  KPcmDataForGsmFrame = 640;
const TUint	KPcmInputFrameSize   = KPcmEncodedFrameSize*2;	// cache 2 frames

/**
*
*class CMmfGsmToPcm16HwDevice 
*
*/
class CMmfGsm610ToPcm16HwDevice : public CMMFSwCodecWrapper
	{	
public:
	static  CMmfGsm610ToPcm16HwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMmfGsm610ToPcm16HwDevice(); 
private:
	void ConstructL();
	};

/**
*
* CMmfPcmHwDevicePcm16ToGsm610 
*
*/	  
class CMmfPcm16ToGsm610HwDevice : public CMMFSwCodecWrapper
	{	
public:
	static  CMmfPcm16ToGsm610HwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	~CMmfPcm16ToGsm610HwDevice(); 
private:
	void ConstructL();
	};

/**
*
* class CMMFGsm610ToPcm16Codec coverts Gsm610 to PCM16
*  
*/
class CGSM610FR_Decoder;

class CMMFGsm610ToPcm16Codec : public CMMFSwCodec
	{
public:
	CMMFGsm610ToPcm16Codec();
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KGsmEncodedFrameSize;};
	virtual TUint SinkBufferSize() {return KPcmInputFrameSize;};
	static  CMMFGsm610ToPcm16Codec* NewL();
	void ConstructL();
	~CMMFGsm610ToPcm16Codec();
private:
	TBool CheckInputBuffers( CMMFDataBuffer& aSrc, CMMFDataBuffer& aDest );
	TInt  NumBuffersToProcess( const CMMFDataBuffer& aSrc );
	TBool ProcessPostCondition( const CMMFSwCodec::TCodecProcessResult& result );
	// do not provide an implmentation for these two methods
	const CMMFGsm610ToPcm16Codec& operator=( const CMMFGsm610ToPcm16Codec& );
	CMMFGsm610ToPcm16Codec( const CMMFGsm610ToPcm16Codec& );
private:
	CGSM610FR_Decoder* iGsmDecoder;
	};

/**
*
* class CMMFPcm16ToGsm610Codec coverts PCM16 to GSM610
*  
*/  
class CGSM610FR_Encoder;
class CMMFPcm16ToGsm610Codec : public CMMFSwCodec
	{
public:
	CMMFPcm16ToGsm610Codec();
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize() {return KPcmInputFrameSize;};
	virtual TUint SinkBufferSize() {return  KGsmEncodedFrameSize;};	
	static  CMMFPcm16ToGsm610Codec* NewL();
	~CMMFPcm16ToGsm610Codec();
	void ConstructL();
private:
    TBool CheckInputBuffers( CMMFDataBuffer& aSrc, CMMFDataBuffer& aDest );
	TInt NumBuffersToProcess( const CMMFDataBuffer& aSrc );
	inline TBool ProcessPostCondition( const CMMFSwCodec::TCodecProcessResult& result );

	// do not supply implementations
	const CMMFPcm16ToGsm610Codec& operator=( const CMMFPcm16ToGsm610Codec& );
	CMMFPcm16ToGsm610Codec( const CMMFPcm16ToGsm610Codec &);
private:
	CGSM610FR_Encoder* iGsmEncoder;
	};

/**
*
* class TMmfAudioAudioCodecPanics provides a name space for audio codec
* panics
*
**/
class TMmfGsmCodecPanicsNameSpace
	{
public:
	/*
	A list of panic codes for Codecs
	@internalTechnology
	EBadArgument indicates a bad argument
	EBadState indicates a state viaolation
	EBadInvariant indicates an invariant violation
	EPostConditionViolation indicates a post condition violation
		*/
	enum TMmfGsmCodecPanics
		{
		EBadArgument,
		EBadState,
		EBadInvariant,
		EBadReset,
		EPostConditionViolation
		};
public:
	inline static void Panic(const TMmfGsmCodecPanicsNameSpace::TMmfGsmCodecPanics aPanic);		
	};

/**
*
* Panic
* @argument aPanic is the panic code for the codec
*
*/
inline void TMmfGsmCodecPanicsNameSpace::Panic(const TMmfGsmCodecPanicsNameSpace::TMmfGsmCodecPanics aPanic)
	{
	_LIT(KGsmCodecPanicCategory, "GsmCodec");
	User::Panic(KGsmCodecPanicCategory, aPanic);
	}

/**
*
* ProcessPostCondition
* @param aResult
* @result TBool Etrue if the post condition is satisfied
*
**/
inline TBool CMMFPcm16ToGsm610Codec::ProcessPostCondition( const CMMFSwCodec::TCodecProcessResult& aResult )
	{
     TBool status = ETrue;
	 if( (aResult.iSrcBytesProcessed / KPcmDataForGsmFrame ) != (aResult.iDstBytesAdded / KGsmFrameSize  ) )
		 {
		 status = EFalse;
		 }
	 return status;
	}
#endif  //__GSM610_H__
