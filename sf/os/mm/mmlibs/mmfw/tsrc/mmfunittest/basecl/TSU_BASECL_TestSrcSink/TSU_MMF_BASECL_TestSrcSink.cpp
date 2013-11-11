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

#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmfdatabuffer.h>
#include "TSU_MMF_BASECL_TestSrcSink.h"


//active timer object to create an asynchronous source event
CMMF_TSU_BASECL_AsyncSrcEvent::CMMF_TSU_BASECL_AsyncSrcEvent() : CTimer(EPriorityStandard)
	{
	CTimer::ConstructL();
	}

CMMF_TSU_BASECL_AsyncSrcEvent::~CMMF_TSU_BASECL_AsyncSrcEvent()
	{
	Cancel();
	}

void CMMF_TSU_BASECL_AsyncSrcEvent::CallBufferFilledLAsynchronously(MDataSink* aConsumer, CMMFBuffer* aBuffer)
	{
	iConsumer = aConsumer;
	iBuffer = aBuffer;
	if (!IsAdded())
		CActiveScheduler::Add(this);
	After(TTimeIntervalMicroSeconds32(KMmfTSUBASECLDefaultAsyncTime));	
	}

void CMMF_TSU_BASECL_AsyncSrcEvent::RunL()
	{
	iConsumer->BufferFilledL(iBuffer);
	}



MDataSource* CMMF_TSU_BASECL_TestSource::NewSourceL()
	{
	CMMF_TSU_BASECL_TestSource* s = new(ELeave) CMMF_TSU_BASECL_TestSource();
	return STATIC_CAST( MDataSource*, s );
	}

CMMF_TSU_BASECL_TestSource::CMMF_TSU_BASECL_TestSource() :
	MDataSource(KMmfUidTSUBASECLTestSourceUid), iState(ELoggedOff), 
	iBufferCreated(EFalse), iOwnsBuffer(EFalse), iFourCC('A','B','C','D')
	{
	}

/**
 * Constructs a source using the aInitData initialisation data
 * specifying the size in frames - effectivly to simulate the source data size
 * the created buffer size
 * whether the source owns the buffer ie is a ref buffer
 * whether the source is synchronous or asynchronous
 */
void CMMF_TSU_BASECL_TestSource::ConstructSourceL( const TDesC8& aInitData) 
	{
	TMMF_BASECL_TestSrcSinkConfig srcInitDataStruct;
	TPckgC<TMMF_BASECL_TestSrcSinkConfig> pckg(srcInitDataStruct);
	pckg.Set(aInitData);
	srcInitDataStruct = pckg();
	iSizeInFrames = srcInitDataStruct.iSizeInFrames;
	iCreatedBufferSize = srcInitDataStruct.iCreatedBufferSize;
	iOwnsBuffer = srcInitDataStruct.iOwnsBuffer;
	iAsynchronous = srcInitDataStruct.iAsynchronous;

	//create default source data
	iTestSourceData = HBufC8::NewL(KmmfTSUBASECLTextFieldSize);
	_LIT8(KBufStuff, "FRAME NUMBER ");
	iTestSourceData->Des().Zero();
	iTestSourceData->Des().Append(KBufStuff);
	}
	

CMMF_TSU_BASECL_TestSource::~CMMF_TSU_BASECL_TestSource()
	{
	if (iOwnsBuffer)
		delete iBuffer;
	delete iAsyncSrcEvent;
	delete iTestSourceData;
	}

CMMFBuffer* CMMF_TSU_BASECL_TestSource::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &aReference)
	{
	CMMFDataBuffer* buf = CMMFDataBuffer::NewL(iCreatedBufferSize);
	aReference = iOwnsBuffer;
	iBufferCreated = ETrue;
	if (iOwnsBuffer)
		{
		delete iBuffer;
		iBuffer = buf; //store buffer pointer for later deletion
		}
	return buf;
	}

TFourCC CMMF_TSU_BASECL_TestSource::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	return iFourCC;
	}

TInt CMMF_TSU_BASECL_TestSource::SetSourceDataTypeCode(TFourCC aSinkFourCC, TMediaId /*aMediaId*/)
	{
	iFourCC = aSinkFourCC;
	return KErrNone;
	}

/**
 * Fills buffer with source test data
 * This consists of the text "FRAME NUMBER " followed by the frame number
 * of a Buffer - for frames < 16 bytes the buffer is filled up to the buffer size
 * for frames > 16 bytes the text is repeated
 * frames must be greater than 3 bytes
 */
void CMMF_TSU_BASECL_TestSource::FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId /*aMediaId*/)
	{
	CMMFDataBuffer* buf = STATIC_CAST(CMMFDataBuffer*, aBuffer);
	if (aBuffer->FrameNumber()<= iSizeInFrames)
		{ //check have not reached end of data
		if (!iFirstFrame)
			iFirstFrame = aBuffer->FrameNumber();
		iCurrentFrame = aBuffer->FrameNumber();
				
		TBool appendFrameNumber;
		TDes8& ptr8 = buf->Data();
		TUint lengthToAppend = ptr8.MaxLength();
		if (lengthToAppend >= KMmfTSUBASECLDefaultFrameSize) 
			{ //append text
			lengthToAppend = KmmfTSUBASECLTextFieldSize;
			appendFrameNumber = ETrue; //append frame number later
			}
		else if ((lengthToAppend < KMmfTSUBASECLDefaultFrameSize) &&
			(iSourceDataPosition+lengthToAppend >= KMmfTSUBASECLDefaultFrameSize))
			{ //we are continuing adding text from a previuos append
			lengthToAppend -=KmmfTSUBASECLNumberFieldSize;
			appendFrameNumber = ETrue; //append frame number later
			}
		else //not at end of text so don't append frame number
			appendFrameNumber = EFalse;
		
		ptr8.Zero();
		ptr8.Append(iTestSourceData->Ptr()+iSourceDataPosition, lengthToAppend); //append text
		iSourceDataPosition += lengthToAppend; //update append position
		if (iSourceDataPosition >= KmmfTSUBASECLTextFieldSize)
			iSourceDataPosition = 0; //completed a frame so reset pos
		if (appendFrameNumber)
			ptr8.AppendNumFixedWidth(aBuffer->FrameNumber(), EDecimal, KmmfTSUBASECLNumberFieldSize);		
		}

	if (!iAsynchronous)
		{
		aConsumer->BufferFilledL(aBuffer);
		}
	else
		{
		//source is asynchronous so return buffer later using timer
		iAsyncSrcEvent->CallBufferFilledLAsynchronously(aConsumer, aBuffer);
		}
	}


void CMMF_TSU_BASECL_TestSource::BufferEmptiedL(CMMFBuffer* /*aBuffer */)
	{
	}


TBool CMMF_TSU_BASECL_TestSource::CanCreateSourceBuffer()
	{
	return ETrue;
	}

//This API no longer used - DataPath now uses CreateSourceBufferL taking aRef
CMMFBuffer* CMMF_TSU_BASECL_TestSource::CreateSourceBufferL(TMediaId /*aMediaId*/)
	{
	iBufferCreated = ETrue;
	return NULL;
	}

/**
 * CMMF_TSU_BASECL_AsyncSrcEvent needs to be created in the same thread as 
 * the datapath execution so create this in the SourceThreadLogon rather
 * than the constructor
 */
TInt CMMF_TSU_BASECL_TestSource::SourceThreadLogon(MAsyncEventHandler& /*aEventHandler*/)
	{
	TInt err = KErrNone;
	if (iAsynchronous) 
		{
		TRAP(err, iAsyncSrcEvent = new(ELeave)CMMF_TSU_BASECL_AsyncSrcEvent()); // misuse TRAPD here, change to TRAP
		}
	if ((!err) && (iState == ELoggedOff)) iState = ELoggedOn;
	return err;
	}

void CMMF_TSU_BASECL_TestSource::SourceThreadLogoff()
	{
	delete iAsyncSrcEvent;
	iAsyncSrcEvent = NULL; //so it can be deleted in the destructor as well
	iState = ELoggedOff;
	}

void CMMF_TSU_BASECL_TestSource::SourcePrimeL()
	{
	iState = EPrimed;
	}

void CMMF_TSU_BASECL_TestSource::SourcePauseL()
	{
	if (iAsynchronous)
		iAsyncSrcEvent->Cancel();
	iState = EPaused;
	}

void CMMF_TSU_BASECL_TestSource::SourcePlayL()
	{
	if (iAsynchronous)
		iAsyncSrcEvent->Cancel();
	iFirstFrame = 0;
	iState = EPlaying;
	}

void CMMF_TSU_BASECL_TestSource::SourceStopL()
	{
	if (iAsynchronous)
		iAsyncSrcEvent->Cancel();
	iState = EStopped;
	}

TSrcSinkState CMMF_TSU_BASECL_TestSource::State()
	{
	return iState;
	}

TBool CMMF_TSU_BASECL_TestSource::BufferCreated()
	{
	return iBufferCreated;
	}

void CMMF_TSU_BASECL_TestSource::SetOwnsBuffer(TBool aOwnsBuffer)
	{
	iOwnsBuffer = aOwnsBuffer;
	}

TUint CMMF_TSU_BASECL_TestSource::FirstFrame()
	{
	return iFirstFrame;
	}

TUint CMMF_TSU_BASECL_TestSource::CurrentFrame()
	{
	return iCurrentFrame;
	}

TUint CMMF_TSU_BASECL_TestSource::Frames()
	{
	return iSizeInFrames;
	}



CMMF_TSU_BASECL_AsyncSinkEvent::CMMF_TSU_BASECL_AsyncSinkEvent() : CTimer(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
#ifdef _DEBUG
	TRAPD(err, CTimer::ConstructL());
	ASSERT(err == KErrNone);	// CTimer::ConstructL failed
#else
	TRAP_IGNORE(CTimer::ConstructL());
#endif
	}

CMMF_TSU_BASECL_AsyncSinkEvent::~CMMF_TSU_BASECL_AsyncSinkEvent()
	{
	Cancel();
	}


void CMMF_TSU_BASECL_AsyncSinkEvent::CallBufferEmptiedLAsynchronously(MDataSource* aSupplier, CMMFBuffer* aBuffer)
	{
	iCallBufferEmptied = ETrue;
	iSupplier = aSupplier;
	iBuffer = aBuffer;
	After(TTimeIntervalMicroSeconds32(KMmfTSUBASECLDefaultAsyncTime));	
	}

void CMMF_TSU_BASECL_AsyncSinkEvent::CallBufferEmptiedAfterPause(MDataSource* aSupplier, CMMFBuffer* aBuffer)
	{
	// This mimicks the behaviour seen when using the audio output DLL: BufferEmptiedL is called twice which
	// keeps the datapath going...
	iCallBufferEmptiedAfterPause = ETrue;
	iSupplier = aSupplier;
	iBuffer = aBuffer;
	After(TTimeIntervalMicroSeconds32(KMmfTSUBASECLDefaultAsyncTime));
	}

void CMMF_TSU_BASECL_AsyncSinkEvent::RunL()
	{
	if (iCallBufferEmptied)
		{
		// iBuffer will be NULL if play has stopped.
		// This is because the pointers to the src/sink buffers may not be valid and are
		// reset to NULL in the datapath by ResetRefBuffers when play stops. Refer to INC021405 for more info.
		iSupplier->BufferEmptiedL(iBuffer);
		iCallBufferEmptied = EFalse;
		}

	if (iCallBufferEmptiedAfterPause)
		{
		iSupplier->BufferEmptiedL(iBuffer);
		iCallBufferEmptiedAfterPause = EFalse;
		}
	}




MDataSink* CMMF_TSU_BASECL_TestSink::NewSinkL()
	{
	CMMF_TSU_BASECL_TestSink* s = new(ELeave) CMMF_TSU_BASECL_TestSink();
	return STATIC_CAST( MDataSink*, s ) ;
	}

CMMF_TSU_BASECL_TestSink::CMMF_TSU_BASECL_TestSink() :
	MDataSink(KMmfUidTSUBASECLTestSinkUid), iState(ELoggedOff), 
	iBufferCreated(EFalse), iOwnsBuffer(EFalse), iFourCC('a','b','c','d')
	{
	}

/**
 * Constructs a sink using the aInitData initialisation data
 * specifying the size in frames
 * the created buffer size
 * whether the sink owns the buffer ie is a ref buffer
 * whether the sink is synchronous or asynchronous
 */
void CMMF_TSU_BASECL_TestSink::ConstructSinkL( const TDesC8& aInitData ) 
	{
	TMMF_BASECL_TestSrcSinkConfig srcInitDataStruct;
	TPckgC<TMMF_BASECL_TestSrcSinkConfig> pckg(srcInitDataStruct);
	pckg.Set(aInitData);
	srcInitDataStruct = pckg();
	iSizeInFrames = srcInitDataStruct.iSizeInFrames;
	iCreatedBufferSize = srcInitDataStruct.iCreatedBufferSize;
	iOwnsBuffer = srcInitDataStruct.iOwnsBuffer;
	iAsynchronous = srcInitDataStruct.iAsynchronous;
	}

CMMF_TSU_BASECL_TestSink::~CMMF_TSU_BASECL_TestSink()
	{
	if (iOwnsBuffer)
		delete iBuffer;
	delete iAsyncSinkEvent;
	}

TFourCC CMMF_TSU_BASECL_TestSink::SinkDataTypeCode(TMediaId /*aMediaId*/) //used by data path MDataSource/Sink for codec matching
	{
	return iFourCC;
	}

TInt CMMF_TSU_BASECL_TestSink::SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId /*aMediaId*/)
	{
	iFourCC = aSinkFourCC;
	return KErrNone;
	}

CMMFBuffer* CMMF_TSU_BASECL_TestSink::CreateSinkBufferL(TMediaId /*aMediaId*/, TBool &aReference)
	{
	CMMFDataBuffer* buf = CMMFDataBuffer::NewL(iCreatedBufferSize);
	aReference = iOwnsBuffer;
	iBufferCreated = ETrue;
	if (iOwnsBuffer) 
		{
		delete iBuffer;
		iBuffer = buf; //store buffer pointer for later deletion
		}
	return buf;
	}


/**
 * Empties sink buffer
 * Expects the buffer to contain the text "FRAME NUMBER " or "frame number"
 * Depending on whether the 4CC code is set to 'ABCD' or 'abcd'
 * Only checks contents of buffer if frame size is 16 bytes - considered acceptable for test purposes
 * As tests can vary the source buffer size
 */
void CMMF_TSU_BASECL_TestSink::EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId /*aMediaId*/)//called by a MDataSource to empty a buffer
	{
	CMMFDataBuffer* buf = STATIC_CAST(CMMFDataBuffer*, aBuffer);

	if (buf)
		{
		//XXX Remove this line when DP EInitializeSink state has been removed
		//This is still present in the dataPath and may remain in for Typhoon
		//Probably won't be removed till Jetstream
		if ((buf->FrameNumber() == 0) && (buf->Data().Length() == 0)) 
			{ //return a dummy buffer
			aSupplier->BufferEmptiedL(buf);
			return;
			}
		if (buf->Data().Length() == 16)
			{//only check if buffer is 16 bytes - otherwise gets complicated
			//check contents of the buffer
			//the sink should accept lower case text if its fourCC is set to 'abcd'
			//and upper case text if its fourcc is set to 'ABCD'
			_LIT8(KExpectedFourCCabcdText, "frame number ???");
			_LIT8(KExpectedFourCCABCDText, "FRAME NUMBER ???"); //'???' wildcard -src/sink frm numbers may differ
			
			HBufC8* refBuf = HBufC8::NewL(16);
			TPtr8 ptr8((TUint8*)refBuf->Ptr(), 0, 16);
			ptr8.Zero();
			if (iFourCC == TFourCC('a','b','c','d'))
				ptr8.Append(KExpectedFourCCabcdText);
			else if (iFourCC == TFourCC('A','B','C','D'))
				ptr8.Append(KExpectedFourCCABCDText);	
			TInt validData = ptr8.Match(buf->Data());
			validData = buf->Data().Match(ptr8); //check if a data match can be found on the CMMFDataBuffer
			delete refBuf;
			if (validData == KErrNotFound)
				{//the test sink has received invalid data
				User::Leave(KErrNotFound); //signal problem to DP
				}
			}
			/*else
				{
				//don't know where in the destinaion data we are
				//so not possible to perform a direct comparison
			} */
		}	

	if (!iAsynchronous)
		aSupplier->BufferEmptiedL(aBuffer);
	else
		{
		//sink is asynchronous so return buffer later using timer
		iAsyncSinkEvent->CallBufferEmptiedLAsynchronously(aSupplier, aBuffer);
		}
	}


void CMMF_TSU_BASECL_TestSink::BufferFilledL(CMMFBuffer* /*aBuffer*/)
	{
	}


TBool CMMF_TSU_BASECL_TestSink::CanCreateSinkBuffer()
	{
	return ETrue;
	}

CMMFBuffer* CMMF_TSU_BASECL_TestSink::CreateSinkBufferL(TMediaId /*aMediaId*/)
	{
	iBufferCreated = ETrue;
	return NULL;
	}

/**
 * CMMF_TSU_BASECL_AsyncSinkEvent needs to be created in the same thread as 
 * the datapath execution so create this in the SinkThreadLogon rather
 * than the constructor
 */
TInt CMMF_TSU_BASECL_TestSink::SinkThreadLogon(MAsyncEventHandler& aEventHandler)
	{
	TInt err = KErrNone;
	if (iAsynchronous)
		{
		TRAP(err, iAsyncSinkEvent = new(ELeave)CMMF_TSU_BASECL_AsyncSinkEvent());
		}
	if ((!err) && (iState == ELoggedOff))
		{
		iState = ELoggedOn;
		}
	iEventHandler = &aEventHandler;
	return err;
	}

void CMMF_TSU_BASECL_TestSink::SinkThreadLogoff()
	{
	delete iAsyncSinkEvent;
	iAsyncSinkEvent = NULL;
	iState = ELoggedOff;
	}

void CMMF_TSU_BASECL_TestSink::SinkPrimeL()
	{
	iState = EPrimed;
	}

void CMMF_TSU_BASECL_TestSink::SinkPauseL()
	{
	if (iAsynchronous)
		{
		iAsyncSinkEvent->Cancel();
		iAsyncSinkEvent->CallBufferEmptiedAfterPause(iAsyncSinkEvent->Supplier(), iAsyncSinkEvent->Buffer());
		}
	iState = EPaused;
	}

void CMMF_TSU_BASECL_TestSink::SinkPlayL()
	{
	if (iAsynchronous)
		iAsyncSinkEvent->Cancel();
	iState = EPlaying;
	}

void CMMF_TSU_BASECL_TestSink::SinkStopL()
	{
	if (iAsynchronous)
		iAsyncSinkEvent->Cancel();
	iState = EStopped;
	}

TSrcSinkState CMMF_TSU_BASECL_TestSink::State()
	{
	return iState;
	}

TBool CMMF_TSU_BASECL_TestSink::BufferCreated()
	{
	return iBufferCreated;
	}

void CMMF_TSU_BASECL_TestSink::SetOwnsBuffer(TBool aOwnsBuffer)
	{
	iOwnsBuffer = aOwnsBuffer;
	}

TUint CMMF_TSU_BASECL_TestSink::Frames()
	{
	return iSizeInFrames;
	}
