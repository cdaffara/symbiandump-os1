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

#ifndef MMFWAVFORMAT_H
#define MMFWAVFORMAT_H

#include <mmf/server/mmfformat.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/server/mmfclip.h>
#include <ecom/implementationproxy.h>
#include <mmf/common/mmfutilities.h>

#include "MMFFormatBaseClasses.h"
#include <mmf/server/mmfformatstandardcustominterfaces.h>

#define RiffChunkName(A,B,C,D) ((D<<24)+(C<<16)+(B<<8)+A)
const TInt KRiffChunkNameRiff = RiffChunkName('R','I','F','F');
const TInt KRiffChunkNameList = RiffChunkName('L','I','S','T');
const TInt KRiffChunkNameData = RiffChunkName('d','a','t','a');
const TInt KRiffChunkTypeWave = RiffChunkName('W','A','V','E');
const TInt KRiffChunkNameFact = RiffChunkName('f','a','c','t');
const TInt KRiffChunkNameFmt_ = RiffChunkName('f','m','t',' ');

const TUint KMdaWavMaxHeaderSize = 38;
const TUint KMdaWavFactChunkSize = 4;

const TUint KRiffChunkHeaderLength = 8;
const TUint KRiffChunkDataLength = 4;
const TUint KRiffContainerChunkHeaderLength = KRiffChunkHeaderLength+KRiffChunkDataLength;
const TUint KWavFileUncompressedDataHeaderSize = 44; // Most formats
const TUint KWavFileCompressedDataHeaderSize = 48; // IMA ADPCM format (format ID 17)
const TUint KDefaultFormatChunkLength = 16; //format chunk length is normally (always??) 16 bytes
const TUint KAlawFormatChunkLength = 18; //alaw chunk length is always 18 bytes
const TUint KGSMFormatChunkLength = 20; //gsm610 chunk length seems to be 20 bytes
const TUint KImaAdpcmFormatChunkLength = 20; //ADPCM chunk is always 20 bytes

const TUint KGSMFactChunkSize = 12;
const TUint KGSMFactChunkLength = 4;
const TUint KGSMExtraHeaderDataLength = 2;
const TUint KGSMAvgBytesPerSecond = 1625;
const TUint KGSMAvgBytesPerSecondSampleRate = 8000;
const TUint KGSMBlockAlign = 0x41;  //65 decimal
const TUint KGSMSamplesPerBlock = 0x140; //320decimal

const TUint KImaAdpcmFactChunkSize = 12;
const TUint KImaAdpcmFactChunkLength = 4;
const TUint KImaAdpcmExtraHeaderDataLength = 2;
const TUint KImaAdpcmAvgBytesPerSecond = 4055;
const TUint KImaAdpcmBlockAlign = 0x100;

const TUint KMMFWavFormatTypePcm	= 1;
const TUint KMMFWavFormatTypeAlaw = 6;
const TUint KMMFWavFormatTypeMulaw =	7;
const TUint KMMFWavFormatTypeImaAdpcm = 17;
const TUint KMMFWavFormatTypeGSM610 = 49;


class TMdaRiffChunk
	{
public:
	TInt32 iName;
	TUint32 iLength;
	TInt32 iType;
	TInt iPosition;
	TBool iFound;
	};



/**
 * @internalComponent
 * This provides a mechanism for sharing code between the Read and Write Formats 
 * without resorting to multiple inheritence.
 *
 */
template <class T> 
class TMMFWavConsolidationMethods
	{
public:
	TMMFWavConsolidationMethods(T& aThat) : iThat(aThat){};

	void ProcessFormatHeaderL();
	void FindRiffChunksL();

	TTimeIntervalMicroSeconds Duration(TMediaId aMediaId) const;
	void CalculateFrameTimeInterval();
	TTimeIntervalMicroSeconds PositionL();
	void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);

	void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);

private:
	void AssignChunkTo(TMdaRiffChunk* aAssignedChunk);
	void ReadChunk(TMdaRiffChunk* aChunk);

private:
	T& iThat;
	const TUint8* iStartPtr;
	TUint iLastReadPosition;
	TMdaRiffChunk iCurrent;
	};





class CMMFWavFormatRead : public CMMFFormatRead, public MMMFDecodeCustomInterfaceBlockLength
	{
	friend class TMMFWavConsolidationMethods<CMMFWavFormatRead>;
public:

	static CMMFFormatDecode* NewL(MDataSource* aSource);
	~CMMFWavFormatRead();

    virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
	virtual TTimeIntervalMicroSeconds PositionL();

	virtual TTimeIntervalMicroSeconds Duration(TMediaId aMediaType) const;
	virtual void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);

	//From CMMFFormatDecode2
	TAny* CustomInterface(TUid aInterfaceId);

	//From MMMFDecodeCustomInterfaceBlockLength
	TUint FileBlockLength();
	
protected:
	//From CMMFFormatRead
	virtual void ConstructL(MDataSource* aSource);
	void ProcessFormatHeaderL();
	void CalculateFrameTimeInterval();

private:
	CMMFWavFormatRead(){};

private:
	TUint iCodecId;

	TMdaRiffChunk iFormatChunk;
	TMdaRiffChunk iFactChunk;
	TMdaRiffChunk iDataChunk;
	TUint iRiffChunkLength;

	TUint iAverageBytesPerSecond;	//needed for correct IMA
	TUint iBlockAlign;				//needed for correct IMA
	TUint iSamples; 				//needed for IMA
	TUint iSamplesPerBlock;

	TMMFWavConsolidationMethods<CMMFWavFormatRead>* iWavFormatConsolidator;
	};



class CMMFWavFormatWrite : public CMMFFormatWrite
	{
	friend class TMMFWavConsolidationMethods<CMMFWavFormatWrite>;

public:
	static CMMFFormatEncode* NewL(MDataSink* aSink);
	~CMMFWavFormatWrite();

	virtual TTimeIntervalMicroSeconds Duration(TMediaId aMediaType) const;

	virtual TInt SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId); 
	virtual void NegotiateL(MDataSource& aSource);

	virtual void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId);

	virtual TInt64 BytesPerSecond();

	virtual void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);

	virtual void CropL(TTimeIntervalMicroSeconds aPosition, TBool aToEnd );

	virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
	virtual TTimeIntervalMicroSeconds PositionL();


protected:
	//From CMMFFormatWrite
	void ConstructL(MDataSink* aSink);
	TInt DetermineIfValidClip();
	void SetDefaultHeaderParametersL();
	void ProcessFormatHeaderL();
	void WriteHeaderL();
	void CalculateFrameTimeInterval();	
	TInt OKToRecord();

private:
	CMMFWavFormatWrite() {};


private:
	TUint iCodecId;

	TBool                     iHasFactChunk;

	TMdaRiffChunk             iFormatChunk;
	TMdaRiffChunk             iFactChunk;
	TMdaRiffChunk             iDataChunk;
	TUint                     iRiffChunkLength;

	TUint                     iAverageBytesPerSecond;	//needed for correct IMA
	TUint                     iBlockAlign;				//needed for correct IMA
	TUint 					  iSamples; 				//needed for IMA
	TBool					  iHasChunksAfterData;	
	TUint                     iSamplesPerBlock;
	TBool                     iGSM610FrameSizeSet;

	TMMFWavConsolidationMethods<CMMFWavFormatWrite>* iWavFormatConsolidator;	
	};
#endif

