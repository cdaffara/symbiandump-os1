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

#ifndef __SBCENCODER_H__
#define __SBCENCODER_H__

#include <e32std.h>
#include <mmf/server/mmfcodec.h>

#include "SBCFrameParameters.h"

const TUint8 KSbcMaxBlocks = 16;
const TUint8 KSbcMaxChannels = 2;
const TUint8 KSbcMaxSubbands = 8;

/**
This class is for manipulating (reading/writing) raw bit stream. 
It is used in sbc encoder for generating sbc frame bit stream.
@internalComponent
*/
class CBitStreamParser : public CBase
	{
public:
	static CBitStreamParser* NewLC(TDes8& aBitStream);
	void 	Reset();
	
	/** relative position */
	TUint8	ReadBits(TInt aBitsToRead);
	void	WriteBits(TInt aBitsToWrite, TUint8 aBitsValue);
	
	inline TUint8	ReadByte();
	inline void		WriteByte(TUint8 aByteValue);
	
	/** absolute position */
	inline void		SetPosition(TUint aByteOffset, TUint8 aBitOffset);
	inline void		Position(TUint& aByteOffset, TUint8& aBitOffset) const;

private:
	CBitStreamParser(TDes8& aBitStream);
	~CBitStreamParser();
	
	void	ConstructL();

private:
	TDes8&  iBitStream;
	TUint8*	iPtr;
	TUint	iByteOffset;
	TUint8	iBitOffset;
	};
	
/**
This class is for checking and generating SBC CRC code. 
It is used in sbc encoder for generating SBC CRC check number.
@internalComponent
*/
class CSbcCRCCalculator : public CBase
	{
public:
	CSbcCRCCalculator();
	
	void 	Reset();
	void 	InputBit(TUint8 aBit);
	void	InputBits(TUint8 aBits, TUint8 aValue);
	void 	InputByte(TUint8 aByte);
	
	TUint8 	ShiftRegister();
	
private:
	TUint8 	iShiftRegister;
	};

/**
This class is for encoding one SBC frame
@internalComponent
*/
class CSBCFrameEncoder : public CBase
	{
public:
	static CSBCFrameEncoder* NewL();
	~CSBCFrameEncoder();
	void EncodeFrameL(const TDesC8& aSrc, TDes8& aFrame);
	void Configure(const TSBCFrameParameters& aParameters);
	void Reset();
	
private:
	CSBCFrameEncoder();
	void 	ConstructL();
	
	/** encode frame */
	void 	Analyse(const TDesC8& aSrc);
	void 	CalcScaleFactors();
	void	JoinSubbands();
	void 	CalcBitAllocation();
	void 	Quantize();
	
	void 	AnalyseMono(const TInt16 aInputSamples[]);
	void 	AnalyseOneChannel(const TInt16 aInputSamples[], TUint8 aChannel);
	void 	Analyse4Subbands(const TInt16 aInputSamples[], TUint8 aBlock, TUint8 aChannel);
	void 	Analyse8Subbands(const TInt16 aInputSamples[], TUint8 aBlock, TUint8 aChannel);

	void	DoJoinSubbands();
	
	void 	CalcBitAllocIndependent();
	void 	CalcBitneedIndependent(TInt8 aBitneed[], const TUint8 aScaleFactors[]);
	TInt8 	MaxBitneedIndependent(const TInt8 aBitneed[]);
	TInt8 	CalcBitSlicesIndependent(const TInt8 aBitneed[], TInt& aBitCount);
	void 	DistributeBitsIndependent(const TInt8 aBitneed[], TUint8 aBits[]);

	void 	CalcBitAllocCombined();
	void 	CalcBitneedCombined(TInt8 aBitneed[][KSbcMaxSubbands]);
	TInt8 	MaxBitneedCombined(const TInt8 aBitneed[][KSbcMaxSubbands]);
	TInt8 	CalcBitSlicesCombined(const TInt8 aBitneed[][KSbcMaxSubbands], TInt& aBitCount);
	void 	DistributeBitsCombined(const TInt8 aBitneed[][KSbcMaxSubbands]);
	
	TUint8 	CalcCRC();
	
	/** writing result back */
	void	WriteFrameL(TDes8& aFrame);
	void 	WriteHeader(CBitStreamParser& aParser);
	void 	WriteScaleFactors(CBitStreamParser& aParser);
	void 	WriteData(CBitStreamParser& aParser);
	void 	WritePaddingL(CBitStreamParser& aParser);
	
private:
	/** input parameters, set before encode */
	TSBCFrameParameters	iParameters;
	
	TInt16 iAnalysisSamples[2][KSbcMaxSubbands * 10];
	TUint8 iScaleFactors[2][KSbcMaxSubbands];
	TUint8 iBits[2][KSbcMaxSubbands];
	TInt32 iOutputSamples[KSbcMaxBlocks][2][KSbcMaxSubbands];
	TUint8 iJoin[KSbcMaxSubbands];
	};
	
/**
This class is for encoding one SBC frame
@internalComponent
*/
class CSBCEncoder : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CSBCEncoder();
	void ConfigureL(TUid aConfigType, const TDesC8& aConfigData);
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);

private:
	CSBCEncoder();
	void 	ConstructL(TAny* aInitParams);
	void 	ResetL();
	
	TUint 	EncodeFrameL(const TDesC8& aSrc, TDes8& aDst);
	TUint 	CachePcmSamplesL(const CMMFDataBuffer& aSrc, TUint aSrcPos);
	TUint 	CachedSampleSize();
	
private:
	/** input parameters, set before encode */
	TSBCFrameParameters	iParameters;
	TUint iPcmFrameSize;
	
	/** CSBCFrameEncoder encodes each frame */
	CSBCFrameEncoder*	iSbcFrameEncoder;
	TUint iSbcFrameLength;
	
	/** pcm audio sample cach */
	HBufC8* iPcmSampleCach;
	};
	
#endif // __SBCENCODER_H__

