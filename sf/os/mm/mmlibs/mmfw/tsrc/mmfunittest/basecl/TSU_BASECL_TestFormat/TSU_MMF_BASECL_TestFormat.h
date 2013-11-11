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

#ifndef __TSU_MMF_BASECL_TESTFORMAT_H__
#define __TSU_MMF_BASECL_TESTFORMAT_H__

#include <mmf/server/mmfformat.h>
#include "TSU_MMF_BASECL_TestFormatImplementationUID.hrh"
#include "../TSU_BASECL_TestSrcSink/TSU_MMF_BASECL_TestSrcSink.h"

const TUid  KMmfUidTSUBASECLTestFormatDecodeUid	= {KMmfUidTSUBASECLTestFormatDecode}; 
const TUid  KMmfUidTSUBASECLTestFormatEncodeUid	= {KMmfUidTSUBASECLTestFormatEncode}; 
const TUint KMmfFormatFrameTimeInterval = 100000; // default it to 100ms

/**
*
* CMMF_TSU_BASECL_TestFormatDecode
*
* Test format decode used by the TSU_MMF_BASECL unit test harness
* Uses the CMMF_TSU_BASECL_TestSource
*
*/
class CMMF_TSU_BASECL_TestFormatDecode : public CMMFFormatDecode
	{
public: 
	static CMMFFormatDecode* NewL(MDataSource* aSource);
	~CMMF_TSU_BASECL_TestFormatDecode();

	virtual TUint Streams(TUid aMediaType) const;
	virtual TTimeIntervalMicroSeconds FrameTimeInterval(TMediaId aMediaType) const;
	virtual TTimeIntervalMicroSeconds Duration(TMediaId aMediaType) const;

	//from MDataSource	
	virtual TFourCC SourceDataTypeCode(TMediaId /*aMediaId*/);
	virtual TInt SetSourceDataTypeCode(TFourCC aSinkFourCC, TMediaId /*aMediaId*/);
	virtual void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId /*aMediaId*/);
	virtual TBool CanCreateSourceBuffer();
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &aReference);
	virtual TInt SourceThreadLogon(MAsyncEventHandler& /*aEventHandler*/);
	virtual void SourceThreadLogoff();
	virtual void SourcePrimeL();
	virtual void SourcePauseL();
	virtual void SourceStopL();
	virtual void SourcePlayL();

	void SendEventToClient(const TMMFEvent& aEvent);

protected:
	CMMF_TSU_BASECL_TestFormatDecode();

private:
	virtual void ConstructL(MDataSource* aSource);

private:
	MDataSource* iSource;
	TSrcSinkState iState;
	TBool iBufferCreated;
	TUint iSizeInFrames;
	TUint iCreatedBufferSize;
	TBool iOwnsBuffer;
	TBool iAsynchronous;
	TFourCC iFourCC;
	CMMFBuffer* iBuffer;
	HBufC8* iTestSourceData;
	TUint iSourceDataPosition;
	};

/**
*
* CMMF_TSU_BASECL_TestFormatEncode
*
* Test format decode used by the TSU_MMF_BASECL unit test harness
* Uses the CMMF_TSU_BASECL_TestSink
*
*/
class CMMF_TSU_BASECL_TestFormatEncode : public CMMFFormatEncode
	{
public: 
	static CMMFFormatEncode* NewL(MDataSink* aSink);
	~CMMF_TSU_BASECL_TestFormatEncode();

	virtual TTimeIntervalMicroSeconds FrameTimeInterval(TMediaId aMediaType) const;
	virtual TTimeIntervalMicroSeconds Duration(TMediaId aMediaType) const;

	//from MDataSink
	virtual TFourCC SinkDataTypeCode(TMediaId /*aMediaId*/);
	virtual TInt SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId /*aMediaId*/);
	virtual void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId /*aMediaId*/);
	virtual TBool CanCreateSinkBuffer();
	virtual CMMFBuffer* CreateSinkBufferL(TMediaId /*aMediaId*/, TBool &aReference);
	virtual TInt SinkThreadLogon(MAsyncEventHandler& /*aEventHandler*/);
	virtual void SinkThreadLogoff();
	virtual void SinkPrimeL();
	virtual void SinkPauseL();
	virtual void SinkPlayL();
	virtual void SinkStopL();

	void SendEventToClient(const TMMFEvent& aEvent);
protected:
	CMMF_TSU_BASECL_TestFormatEncode();

private:
	virtual void ConstructL(MDataSink* aSink);

private:
	MDataSink* iSink;
	TSrcSinkState iState;
	TBool iBufferCreated;
	TUint iSizeInFrames;
	TUint iCreatedBufferSize;
	TBool iOwnsBuffer;
	TBool iAsynchronous;
	TFourCC iFourCC;
	CMMFBuffer* iBuffer;
	};

#endif

