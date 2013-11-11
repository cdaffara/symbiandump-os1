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

#ifndef __TSU_MMF_BASECL_TESTSRCSINK_H__
#define __TSU_MMF_BASECL_TESTSRCSINK_H__

#include <mmf/server/mmfdatasink.h>
#include <mmf/server/mmfdatasource.h>
#include "TSU_MMF_BASECL_TestSrcSinkImplementationUID.hrh"

const TUid  KMmfUidTSUBASECLTestSourceUid	= {KMmfUidTSUBASECLTestSource}; 
const TUid  KMmfUidTSUBASECLTestSinkUid		= {KMmfUidTSUBASECLTestSink}; 

const TUint KMmfTSUBASECLDefaultAsyncTime = 100000; //100mS
const TUint KMmfTSUBASECLDefaultFrameSize = 16;
const TUint KmmfTSUBASECLNumberFieldSize  = 3;
const TUint KmmfTSUBASECLTextFieldSize    = KMmfTSUBASECLDefaultFrameSize-KmmfTSUBASECLNumberFieldSize;

enum TSrcSinkState
	{
	ELoggedOff=0,
	ELoggedOn,
	EPrimed,
	EPlaying,
	EPaused,
	EStopped
	};

/**
 * Utility used by test data source to simulate an asynchronous call
 * Currently just BufferFilledL
 * Defaults to making after call 100mS 
 */
class CMMF_TSU_BASECL_AsyncSrcEvent : public CTimer
	{
public:
	CMMF_TSU_BASECL_AsyncSrcEvent();
	~CMMF_TSU_BASECL_AsyncSrcEvent();
	void CallBufferFilledLAsynchronously(MDataSink* aConsumer, CMMFBuffer* aBuffer);
private:
	virtual void RunL();
private:
	CMMFBuffer* iBuffer;
	MDataSink* iConsumer;
	};

/**
 * Utiltiy used by test data sink to simulate an asynchronous call
 * Currently just BufferEmptiedL
 * Defaults to making after call 100mS 
 */
class CMMF_TSU_BASECL_AsyncSinkEvent : public CTimer
	{
public:
	CMMF_TSU_BASECL_AsyncSinkEvent();
	~CMMF_TSU_BASECL_AsyncSinkEvent();
	void CallBufferEmptiedLAsynchronously(MDataSource* aSupplier, CMMFBuffer* aBuffer);
	void CallBufferEmptiedAfterPause(MDataSource* aSupplier, CMMFBuffer* aBuffer);
	inline CMMFBuffer* Buffer() { return iBuffer; }
	inline MDataSource* Supplier() { return iSupplier; }
private:
	virtual void RunL();
private:
	CMMFBuffer* iBuffer;					// BufferEmptied callback
	MDataSource* iSupplier;					// BufferEmptied callback
	TBool iCallBufferEmptied;				// True if we should call BufferEmptied
	TBool iCallBufferEmptiedAfterPause;		// True if we should call BufferEmptied again, after Pause is called
	};


class TMMF_BASECL_TestSrcSinkConfig
	{
public:
	TUint iSizeInFrames;
	TUint iCreatedBufferSize;
	TBool iOwnsBuffer;
	TBool iAsynchronous;
	};

typedef TPckgBuf<TMMF_BASECL_TestSrcSinkConfig> TMMF_BASECL_TestSrcSinkConfigPckg;

/**
*
* CMMF_TSU_BASECL_TestSource
*
* Test data source used by the TSU_MMF_BASECL unit test harness
* Fills source buffer with the ascii text "FRAME NUMBER nnn" nnn = frame number
*
*/
class CMMF_TSU_BASECL_TestSource : public CBase, public MDataSource
	{
public: 
	static MDataSource* NewSourceL();
	~CMMF_TSU_BASECL_TestSource();

	//from MDataSource	
	virtual TFourCC SourceDataTypeCode(TMediaId /*aMediaId*/);
	virtual TInt SetSourceDataTypeCode(TFourCC aSinkFourCC, TMediaId /*aMediaId*/);
	virtual void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId /*aMediaId*/);
	virtual void BufferEmptiedL(CMMFBuffer* aBuffer);
	virtual TBool CanCreateSourceBuffer();
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId /*aMediaId*/);
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &aReference);
	virtual TInt SourceThreadLogon(MAsyncEventHandler& /*aEventHandler*/);
	virtual void SourceThreadLogoff();
	virtual void SourcePrimeL();
	virtual void SourcePauseL();
	virtual void SourceStopL();
	virtual void SourcePlayL();

	void SendEventToClient(const TMMFEvent& aEvent);

	//CMMF_TSU_BASECL_TestSource specific functions
	virtual TSrcSinkState State();
	virtual TBool BufferCreated();
	virtual void SetOwnsBuffer(TBool aOwnsBuffer);
	virtual TUint FirstFrame();
	virtual TUint CurrentFrame();
	virtual TUint Frames();
protected:
	virtual void ConstructSourceL( const TDesC8& aInitData ) ;

private:
	CMMF_TSU_BASECL_TestSource();

private:
	TSrcSinkState iState;
	TBool iBufferCreated;
	TUint iSizeInFrames;
	TUint iCreatedBufferSize;
	TBool iOwnsBuffer;
	TBool iAsynchronous;
	TFourCC iFourCC;
	CMMF_TSU_BASECL_AsyncSrcEvent* iAsyncSrcEvent;
	CMMFBuffer* iBuffer;
	HBufC8* iTestSourceData;
	TUint iSourceDataPosition;
	TUint iFirstFrame;
	TUint iCurrentFrame;
	};


/**
*
* CMMF_TSU_BASECL_TestSink
*
* Test data source used by the TSU_MMF_BASECL unit test harness
* Expects buffer to contain the ascii text "FRAME NUMBER" or  "frame number"
* depending on whether its 4CC code has been set to "ABCD" or "abcd"
*
*/
class CMMF_TSU_BASECL_TestSink : public CBase, public MDataSink
	{
public: 
	static MDataSink* NewSinkL();
	~CMMF_TSU_BASECL_TestSink();

	//from MDataSink
	virtual TFourCC SinkDataTypeCode(TMediaId /*aMediaId*/);
	virtual TInt SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId /*aMediaId*/);
	virtual void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId /*aMediaId*/);
	virtual void BufferFilledL(CMMFBuffer* aBuffer);
	virtual TBool CanCreateSinkBuffer();
	virtual CMMFBuffer* CreateSinkBufferL(TMediaId /*aMediaId*/);
	virtual CMMFBuffer* CreateSinkBufferL(TMediaId /*aMediaId*/, TBool &aReference);
	virtual TInt SinkThreadLogon(MAsyncEventHandler& aEventHandler);
	virtual void SinkThreadLogoff();
	virtual void SinkPrimeL();
	virtual void SinkPauseL();
	virtual void SinkPlayL();
	virtual void SinkStopL();

	void SendEventToClient(const TMMFEvent& aEvent);

	//CMMF_TSU_BASECL_TestSink specific functions
	virtual TSrcSinkState State();
	virtual TBool BufferCreated();
	virtual void SetOwnsBuffer(TBool aOwnsBuffer);
	virtual TUint Frames();
protected:
	virtual void ConstructSinkL( const TDesC8& aInitData ) ;

private:
	CMMF_TSU_BASECL_TestSink();

private:
	TSrcSinkState iState;
	TBool iBufferCreated;
	TUint iSizeInFrames;
	TUint iCreatedBufferSize;
	TBool iOwnsBuffer;
	TBool iAsynchronous;
	TFourCC iFourCC;
	CMMF_TSU_BASECL_AsyncSinkEvent* iAsyncSinkEvent;
	CMMFBuffer* iBuffer;
	MAsyncEventHandler* iEventHandler;
	};

#endif

