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
// source\server\mmfdatapath.cpp
// 
//

#include <e32math.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/common/mmfpaniccodes.h>
#include <mmf/server/mmfaudiooutput.h>
#include <mmf/server/mmfaudioinput.h>
#include <mmf/server/mmfdatapath.h>
#include "mmfclientaudiostreamutils.h"
#include <mmf/common/mmfaudio.h>
#include <mmf/plugin/mmfcodecimplementationuids.hrh> // KUidMmfCodecAudioSettings
#include <mmf/server/devsoundstandardcustominterfaces.h>

const TUid KUidCodecAudioConfig = {KUidMmfCodecAudioSettings};

void Panic(TMMFDataPathPanicCode aPanicCode, TInt aSourceLineNumber)
	{
	_LIT(KMMFDataPathPanicCategory, "MMFDataPath");
	User::Panic(KMMFDataPathPanicCategory, STATIC_CAST(TInt,aPanicCode) + aSourceLineNumber);
	}

//all functions are exported form the DLL and are virtual to allow plugins to define there own CMMFDataPaths

/**
Allocates and constructs a data path.

Use this function if the codec UID is not already known by CMMFController
and there is no data path ambiguity - ie only one data path is possible.

Will create codec via fourCC.

@param  aEventHandler
        Installs an event handler to provide message passing between clients and sources/sinks.

@return Newly constructed data path object.
*/

EXPORT_C CMMFDataPath* CMMFDataPath::NewL(MAsyncEventHandler& aEventHandler)
	{
	CMMFDataPath* self = new(ELeave) CMMFDataPath(TMediaId(), aEventHandler);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


/**
Allocates and constructs a data path according to the specified media ID.

Use this function if the codec UID is not already known by CMMFController
and there is ambiguity with the data path ie. there is more than one possible data path.

@param  aMediaId
        Optional media ID parameter when there are multiple media types.
@param  aEventHandler
        Installs an event handler to provide message passing between clients and sources/sinks.

@return A newly constructed data path object.
*/

EXPORT_C CMMFDataPath* CMMFDataPath::NewL(TMediaId aMediaId, MAsyncEventHandler& aEventHandler)
	{
	CMMFDataPath* self = new(ELeave) CMMFDataPath(aMediaId, aEventHandler);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
Allocates and constructs a data path according to the specified codec UID.

Use this function if the codec UID is already known by CMMFController
and there is no data path ambiguity ie. only one data path is possible
will create codec explicitly using the supplied codec Uid

@param  aCodecUid
        Optional mediaID parameter when there are multiple media types
@param  aEventHandler
        Installs an event handler to provide message passing between clients and sources/sinks.

@return A newly constructed data path object.
*/

EXPORT_C CMMFDataPath* CMMFDataPath::NewL(TUid aCodecUid, MAsyncEventHandler& aEventHandler)
	{
	CMMFDataPath* self = new(ELeave) CMMFDataPath(TMediaId(), aEventHandler);
	CleanupStack::PushL(self);
	self->ConstructL(aCodecUid);
	CleanupStack::Pop();
	return self;
	}


/**
Allocates and constructs a data path according to the specified codec UID.

Use this function if the codec UID is already known by CMMFController
and there is ambiguity ie. more than one possible data path.
TMediaId used to select the path.

@param  aCodecUid
		The codec UID.
@param  aMediaId
        Optional mediaID parameter when there are multiple media types.
@param  aEventHandler
        Installs an event handler to provide message passing between clients and sources/sinks.

@return A newly constructed data path object.
*/
EXPORT_C CMMFDataPath* CMMFDataPath::NewL(TUid aCodecUid, TMediaId aMediaId, MAsyncEventHandler& aEventHandler)
	{
	CMMFDataPath* self = new(ELeave) CMMFDataPath(aMediaId, aEventHandler);
	CleanupStack::PushL(self);
	self->ConstructL(aCodecUid);
	CleanupStack::Pop();
	return self;
	}

/**
Standard destructor.
*/

EXPORT_C CMMFDataPath::~CMMFDataPath()
	{
	Cancel();
	delete iCodec;
 	DoCleanupBuffers();

	//log off the source and sink
	if (iDataSource)
		iDataSource->SourceThreadLogoff();
	if (iDataSink)
		iDataSink->SinkThreadLogoff();

	if (iCompleteCallback)
		{
		iCompleteCallback->Cancel();
		delete iCompleteCallback;
		}
	}

/**
Deletes buffers if this datapath's sources and sinks own the buffers returned by PrimeL().
Typically if buffers are created asychronously, the datapath doesn't own the buffer
so leaves cleanup handling to the owner sources/sinks.

Called when source and sink needs to be de-referenced. Sets iDataPathCreated, iSinkCanReceive, 
iSnkBufRef and iSrcBufRef to EFalse; sets iState to EStopped.
*/
EXPORT_C void CMMFDataPath::ResetL()
	{
	delete iCodec;
	iCodec = NULL;
	DoCleanupBuffers(); // Delete buffers
	//logoff and dereference source and sink
	if (iDataSource)
		{ iDataSource->SourceThreadLogoff(); iDataSource = NULL; }
	if (iDataSink)
		{ iDataSink->SinkThreadLogoff(); iDataSink = NULL; }

	// Reset states
	iDataPathCreated = EFalse;
	iState = EStopped;
	iSrcBufRef = EFalse;
	iSnkBufRef = EFalse;
	iPauseCalled = EFalse;

	delete iCompleteCallback; iCompleteCallback = NULL;
	}

/**
Delete source and/or sink buffers that are owned by DataPath.

Ownership indicated by iSrcBufRef and iSnkBufRef.

Ownership is assigned during buffer allocation within the datapath PrimeL().
*/
void CMMFDataPath::DoCleanupBuffers()
	{
	// delete source and/or sink buffer that is owned by DataPath
	if ( !iSrcBufRef && iSourceBuffer )
		{
		delete iSourceBuffer;
		}
	iSourceBuffer = NULL;
	if ( !iSnkBufRef && iSinkBuffer )
		{
		delete iSinkBuffer;
		}
	iSinkBuffer = NULL;
	}


/**
Obtain source and/or sink buffer using the synchronous API CreateSourceBufferL() and CreateSinkBufferL().
*/
void CMMFDataPath::ObtainSyncBuffersL()
	{
	//Try to create source and sink buffers. If we can't create them synchronously via
	//CreateSourceBufferL and CreateSinkBufferL we will need to obtain them by 
	//asynchronous buffer creation when playing starts.

	if (iBuffersToUse & ENeedSourceBuffer)
		{
		if (!iSourceBuffer) //we may already have a buffer from a previous initialization
			{
			TRAPD(err, iSourceBuffer = iDataSource->CreateSourceBufferL(iMediaId,*iSinkBuffer, iSrcBufRef));
			if(err != KErrNone && err != KErrNotSupported)
				{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::ObtainSyncBuffersL - Leaving %d  (this 0x%x)\n"),err, this);
#endif
				User::Leave(err);
				}
			}
		}


	if (iBuffersToUse & ENeedSinkBuffer)
		{
		if (!iSinkBuffer) //we may already have a buffer from a previous initialization
			{
			TRAPD(err, iSinkBuffer = iDataSink->CreateSinkBufferL(iMediaId, iSnkBufRef));
			if(err != KErrNone && err != KErrNotSupported)
				{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::ObtainSyncBuffersL - Leaving %d  (this 0x%x)\n"),err, this);
#endif
				User::Leave(err);
				}
			}
		}

	if (iSourceBuffer && !(iBuffersToUse & ENeedSinkBuffer))
		{//only need one buffer, use source
		iSinkBuffer =iSourceBuffer;
		iSnkBufRef = ETrue; //the sink buffer is not to be deleted
		}
	else if (iSinkBuffer && !(iBuffersToUse & ENeedSourceBuffer))
		{//only need one buffer, use sink
		iSourceBuffer =iSinkBuffer;
		iSrcBufRef = ETrue; //the sink buffer is not to be deleted
		}	

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::ObtainSyncBuffersL - DONE  iSourceBuffer=0x%x ref=%d   iSinkBuffer=0x%x ref=%d (this 0x%x)\n"),iSourceBuffer,iSrcBufRef,iSinkBuffer,iSnkBufRef, this);
#endif
	}





/**
Constructs a source.

The default implementation leaves with KErrNotSupported.

@param  aInitData
        The initialisation data.
*/

EXPORT_C void CMMFDataPath::ConstructSourceL( const TDesC8& /*aInitData*/ )
	{
	User::Leave(KErrNotSupported);
	}

/**
Constructs a sink.

Overridable constuction specific to this datasource.

The default implementation leaves with KErrNotSupported.

@param  aInitData
        The initialisation data.
*/
EXPORT_C void CMMFDataPath::ConstructSinkL( const TDesC8& /*aInitData*/ )
	{
	User::Leave(KErrNotSupported);
	}

/**
Takes UID of codec on construction, and if not an NULL codec sets the datapath up for codec instantiation.

@param  aCodecUid
        The UID of the codec.
*/

EXPORT_C void CMMFDataPath::ConstructL(TUid aCodecUid)
	{
	iUseSuppliedCodecUid = EFalse; //initially assume no supplied codec uid

	if (aCodecUid != KNullUid)
		{//the data path NewL has specified a specific codec
		//create CMMFCodec here
		iCodec = CMMFCodec::NewL(aCodecUid);
		if (iCodec)
			iUseSuppliedCodecUid = ETrue;
		}

	iSrcBufRef = EFalse;
	iSnkBufRef = EFalse;
	iObtainingAsyncSourceBuffer = EFalse;
	iObtainingAsyncSinkBuffer = EFalse;
	iSourceBufferWithSource = EFalse;
	iSinkBufferWithSink = EFalse;
	}


/** 
Adds a data source to the datapath and, if the sink already exists, tries to establish a connection
between the source and sink.

@param  aSource
        The data source to add to the data path.
*/
EXPORT_C void CMMFDataPath::AddDataSourceL(MDataSource* aSource)
	{
	if (!iDataSink) iDataSource=aSource; //can't create a data path without the MDataSink as well
	else if (!iUseSuppliedCodecUid) //no supplied uid need to see if we can create codec to establish a data path
		{//we have a data sink as well so check a data path can be established between source&sink
		CreateDataPathL(aSource, iDataSink);
		iDataSource = aSource;
		}
	else //the CMMFController specified the codec uid so must use existing codec
		{//note we are assuming here that the CMMFController knows what it is doing ie the supplied codec uid 
		//can make the appropriate data conversion
		iDataPathCreated = ETrue;
		iDataSource = aSource;
		}
	ClearPlayWindowL() ;
	User::LeaveIfError(iDataSource->SourceThreadLogon(*this));
	}


/** 
Adds a data sink to the datapath and, if the source already exists, tries to establish a connection
between the source and sink.

@param  aSink
        The data sink to add to the data path.
*/

EXPORT_C void CMMFDataPath::AddDataSinkL(MDataSink* aSink)
	{
	if (!iDataSource) iDataSink=aSink; //can't create a data path without the MDataSource as well
	else if (!iUseSuppliedCodecUid) //no supplied uid need to see if we can create codec to establish a data path
		{//we have a data source as well so check a media path can be established between source&sink
		CreateDataPathL(iDataSource, aSink);
		iDataSink = aSink;
		}
	else //the CMMFController specified the codec uid so must use existing codec
		{//note we are assuming here that the CMMFController knows what it is doing ie the supplied codec uid 
		//can make the appropriate data conversion
		iDataPathCreated = ETrue;
		iDataSink = aSink;	
		
		//set 4CCs
		iSourceFourCC  = iDataSink->SinkDataTypeCode(iMediaId);//sink because CMMFDataPath is an MDataSink to its MDataSource!
		iSinkFourCC = iDataSource->SourceDataTypeCode(iMediaId);//source because CMMFDataPath is an MDataSource to its MDataSink!
		}
	User::LeaveIfError(iDataSink->SinkThreadLogon(*this));
	}


/* 
 *  CreateDataPathL 
 * 
 *  internal function to establish a datapath between the source and sink
 *	the data supplied by the sink adn expected by the source are checked and 
 *	a codec is instantiated if necessary
 *
 *	@param	aSource
 *	@param	aSink
 */

void CMMFDataPath::CreateDataPathL(MDataSource* aSource, MDataSink* aSink)
	{ //procedure to attempt to match the source to the sink creating a codec if necessary
	// returns ETrue if the datapath could be constructed else false
	//sets iCodec to the appropriate codec.& sets its own iSink/iSource FourCC datatype codes
	iDataPathCreated = EFalse;
	if (aSource && aSink) //have a source and sink
		{ //we have a data source & sink but no codec so try and find one - if required
		TFourCC sourceFourCCCode = aSource->SourceDataTypeCode(iMediaId); //get MDataSource data type fourCC code
		TFourCC sinkFourCCCode = aSink->SinkDataTypeCode(iMediaId); //get MDataSink data type fourCC code
		if ((sourceFourCCCode != sinkFourCCCode) && //MDataSource & MDataSink datatypes are not compatible
			(sourceFourCCCode != KMMFFourCCCodeNULL) && (sinkFourCCCode != KMMFFourCCCodeNULL)) 
			{//we need a codec to make the conversion between the source and the sink
			CMMFCodec* codec = CMMFCodec::NewL(sourceFourCCCode, sinkFourCCCode);

			if (codec)
				{
				delete iCodec;
				iCodec = codec;
				//data path created ie have source/sink and can match their datatypes
				iDataPathCreated = ETrue;

				//now we have an source attached we need to configure the codec for sample rate
				//and number of channels

				//prepare a package to send to a codec
				TMMFAudioConfig audioSettings;
				
				//test for decoder
				if (aSource->DataSourceType() == KUidMmfFormatDecode)
					{
					audioSettings.iSampleRate = static_cast<CMMFFormatDecode*>(aSource)->SampleRate();
					audioSettings.iChannels = static_cast<CMMFFormatDecode*>(aSource)->NumChannels();
					}

				//package up to send to codec
				TPckgBuf<TMMFAudioConfig> configPackage(audioSettings);

				//we need to catch User::Leave(KErrNotSupported) as by default most codecs
				//do not support the ConfigureL method.
				TRAPD(err,iCodec->ConfigureL(KUidCodecAudioConfig, configPackage));
				// need to check other error here
				if (err != KErrNone && err != KErrNotSupported)
					{
					User::Leave(err);
					}
				}
			else
				{
				User::Leave( KErrNotSupported ) ; //couldn't find suitable codec
				}
			} //if (sourceFourCCCode != sinkFourCCCode)
		else 
			{ //source & sink fourCC datatypes are the same so no codec is required
			__ASSERT_DEBUG(iCodec == NULL,  Panic(EMMFDataPathPanicProgrammingError,__LINE__)); 			

			iDataPathCreated = ETrue;
			}
		//can assign FourCC codes for the CMMFDataPath
		iSinkFourCC = sourceFourCCCode; //sink because CMMFDataPath is an MDataSink to its MDataSource!
		iSourceFourCC = sinkFourCCCode; //source because CMMFDataPath is an MDataSource to its MDataSink!
		//If sink & source need its own Prime() done in controller	
		} 
	}

/**
Clears the specified buffer.

Pure virtual dummy implementation, not needed by datapath
comes from MDataSink - CMMFData path is a sink to its MDataSource.

This is only required for an active push MDataSource requesting a buffer empty.

@param  aBuffer
        The buffer to empty.
@param  aSupplier
        The MDataSource supplying this buffer.
@param  aMediaId
        An optional mediaID parameter when there are multiple buffers arriving of different media types.

*/
EXPORT_C void CMMFDataPath::EmptyBufferL(CMMFBuffer* /* aBuffer */, MDataSource* /*aSupplier*/, TMediaId /*aMediaId*/)
	{
	//not implemented
	}



/* 
 *  FillSourceBufferL
 * 
 *	Function to get data from the datapath's iDataSource
 */

void CMMFDataPath::FillSourceBufferL()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::FillSourceBufferL tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif

	__ASSERT_DEBUG((iState == EPlaying || iState == EConverting || iState == ERecording), Panic(EMMFDataPathPanicBadState,__LINE__)); 


	// clear the no-more-source flag here (as well as in PlayL()) because 
	// there may have been a re-position since the last call to BufferFilledL()
	iNoMoreSourceData = EFalse;

	if(!iObtainingAsyncSourceBuffer) 
		{//this is a normal request for data. 
		//If we are getting asynchronous buffers, then can't do this as iSourceBuffer == NULL
		iSourceBuffer->SetFrameNumber(++iCurrentSourceFrameNumber); //so source knows which data to load buffer with
		iSourceBuffer->SetStatus(EBeingFilled);
		iSourceBuffer->SetLastBuffer(EFalse);
		}

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP asking for buffer %d  - ptr=0x%x   (this 0x%x)\n"), iCurrentSourceFrameNumber, iSourceBuffer,this);	
#endif

	iSourceBufferWithSource = ETrue;

	// wait for BufferFilled callback from source. Do this here as some sources cause
	//re-entrancy into data path via BufferFilledL
	ChangeDataPathTransferState(EWaitSource);  

	iDataSource->FillBufferL(iSourceBuffer, this, iMediaId);

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::FillSourceBufferL - DONE tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	}


/** 
Indicates the data source has filled the specified buffer.

Called by the CMMFDataPath's MDataSource when it has filled the buffer.

@param aBuffer
       A pointer to the filled buffer.
*/
EXPORT_C void CMMFDataPath::BufferFilledL(CMMFBuffer* aBuffer)
	{	
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::BufferFilledL src has filled buffer %d (ptr=0x%x) with %d bytes EoF = %d  tick-%d    (this 0x%x)\n"),aBuffer->FrameNumber(),aBuffer, aBuffer->BufferSize(),aBuffer->LastBuffer(), User::TickCount(),this);
#endif
	
	//This assertion is commented because of PDEF117405
	//state only used if we are passing data
	//__ASSERT_DEBUG((iState == EPlaying || iState == EConverting || iState == ERecording), Panic(EMMFDataPathPanicBadState,__LINE__));

	__ASSERT_DEBUG((!iNoMoreSourceData), Panic(EMMFDataPathPanicBadState,__LINE__)); 
		
	iSourceBufferWithSource = EFalse;

	//Has the datapath stopped running, if so were not interested in any callbacks.
	if(iState == EStopped || iState == EPrimed || (iPauseCalled && iState != ERecording))
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::BufferFilledL called while not expecting callback iState=%d  iPauseCalled=%d  (this 0x%x)\n"),iState, iPauseCalled,this);
#endif
		return;
		}

#ifdef REPOSITION_SPEEDUP
	// if the source has been re-positioned, then go & get some more source data now
	if (!iObtainingAsyncSourceBuffer && iSourceBuffer->FrameNumber() != iCurrentSourceFrameNumber)
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::BufferFilledL source was re-positioned re-requesting source data (this 0x%x)\n"),this);
#endif
		ChangeDataPathTransferState(ENeedSourceData);
		return;
		}
#endif //REPOSITION_SPEEDUP

	//bufer is NULL, indicating no more source data.
	if (!aBuffer)
		{
		//If we only hold a reference to the source buffer, set that to NULL
		if(iSnkBufRef)
			iSourceBuffer = NULL;


		iNoMoreSourceData = ETrue;

		if(!iCodec || //there's only one buffer and that has been returned as NULL, so must be end of data
		  iSinkBufferWithSink) //buffer is with sink, we don't have any more data to put in it, so must be end of data
			{
			ChangeDataPathTransferState(EEndOfData);
			}
		else //sink buffer is with datapath, see if there is anything to send to sink
			ChangeDataPathTransferState(ENeedToMatchSourceToSink);

#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::BufferFilledL DONE aBuffer==NULL tick-%d (this 0x%x)\n"),User::TickCount(),this);
#endif
		return;
		} 

	
	//We were waiting for a response from the source to get an asynchronous buffer.
	//We now have it, and we proceed to transfer this data to the sink.
	if	(iObtainingAsyncSourceBuffer)
		{
		iObtainingAsyncSourceBuffer = EFalse;
		}
	

	aBuffer->SetStatus(EFull);

	if(iSourceBuffer != aBuffer)
		{//buffer has been changed by the source
		iSourceBuffer = aBuffer;
		if (!(iBuffersToUse & ENeedSinkBuffer))
			{//we only need one buffer and use source
			iSinkBuffer = iSourceBuffer;
			iSnkBufRef = ETrue;
			}
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::BufferFilledL - iSourceBuffer=0x%x ref=%d   iSinkBuffer=0x%x ref=%d (this 0x%x)\n"),iSourceBuffer,iSrcBufRef,iSinkBuffer,iSnkBufRef, this);
#endif	
		}
	//Is this the last buffer from the source (0 length or LastBuffer flag set)
	//or have reached the end of the play window; we only look at the play window here
	//if we are converting. For conversion we look at the data we have read. This is then passed onto 
	//the source
	if (!iSourceBuffer->BufferSize() || iSourceBuffer->LastBuffer() ||
		(((iState == EConverting) || (iState == EPlaying)) && (iPlayWindowEndPosition < iCachedSourceDuration) && ( InputPosition() >= iPlayWindowEndPosition ))) 
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::BufferFilledL end of input data  tick-%d   (this 0x%x)\n"),User::TickCount(),this);
		RDebug::Print(_L("iSourceBuffer->BufferSize()=%d\n"),iSourceBuffer->BufferSize());
		RDebug::Print(_L("iSourceBuffer->LastBuffer()=%d\n"),iSourceBuffer->LastBuffer());
		RDebug::Print(_L("InputPosition()=%d  >= iPlayWindowEndPosition=%d\n"),I64INT(InputPosition().Int64()),I64INT(iPlayWindowEndPosition.Int64()));
#endif
		iNoMoreSourceData = ETrue;
		iSourceBuffer->SetLastBuffer(ETrue); //just in-case we are terminating on BufferSize == 0 or play window
		}


	if (!iCodec)
		ChangeDataPathTransferState(ESendDataToSink);
	else if(!iSinkBufferWithSink) //sink buffer is with data path, can try to fill it
		ChangeDataPathTransferState(ENeedToMatchSourceToSink);
	//else wait for sink to return buffer BufferEmptied will send us into ENeedToMatchSourceToSink state

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::BufferFilledL - DONE tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	}




/* 
 *  FillSinkBufferL
 * 
 *	Function to take the data from an already full source buffer and by using
 *	a codec if necessary fills the sink buffer
 */

void CMMFDataPath::FillSinkBufferL()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::FillSinkBufferL tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif

	//This state is only used if we are passing data
	__ASSERT_DEBUG((iState == EPlaying || iState == EConverting || iState == ERecording), Panic(EMMFDataPathPanicBadState,__LINE__)); 

	//This state is only used if we have a real codec
	__ASSERT_DEBUG(iCodec, Panic(EMMFDataPathPanicBadState,__LINE__)); 


	//The sink buffer is with the sink so we can't fill it.
	//When it has been emptied, this state will be re-entered from BufferEmptiedL
	if(iSinkBufferWithSink)
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::FillSinkBufferL buffer is in use by SINK - DONE   (this 0x%x)\n"),this);
#endif
		ChangeDataPathTransferState(EWaitSink);  // wait for BufferEmptied callback from sink
		return;
		}

	//The source buffer is with the source so we can't take data from it.
	//When it has been filled, this state will be re-entered from BufferFilledL
	if(iSourceBufferWithSource)
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::FillSinkBufferL buffer is in use by SOURCE - DONE   (this 0x%x)\n"),this);
#endif
		ChangeDataPathTransferState(EWaitSource);  // wait for BufferFilled callback from source
		return;
		}

	//source buffer is NULL, can't be any more data to send.
	//iNoMoreSourceData is set and the source buffer is empty, can't be any more data to send.
	if(!iSourceBuffer || (iNoMoreSourceData && !iSourceBuffer->BufferSize()))
		{
		if(iSinkBuffer->Status() == EBeingFilled)
			{//if we have data in sink buffer, mark it as last buffer and send
			iSinkBuffer->SetLastBuffer(ETrue);
			ChangeDataPathTransferState(ESendDataToSink);
			}
		else //the sink buffer can't have anything in it
			ChangeDataPathTransferState(EEndOfData);
		}

#ifdef REPOSITION_SPEEDUP
	// if the source has been re-positioned, 
	// speed things up by getting some more source data now
	if(iSourceBuffer && iSourceBuffer->FrameNumber() != iCurrentSourceFrameNumber)
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::FillSinkBufferL() source was re-positioned re-requesting source data (this 0x%x)\n"),this);
#endif
		ChangeDataPathTransferState(ENeedSourceData);
		return;
		}
#endif //REPOSITION_SPEEDUP

	iSinkBuffer->SetStatus(EBeingFilled);
	iSinkBuffer->SetLastBuffer(EFalse);

	//pass buffer to codec for processing
	iCodecProcessResult = iCodec->ProcessL(*iSourceBuffer, *iSinkBuffer);
	//the codec tries to fill the sink buffer to its max length
	//TCodecProcessResult returns the status of the codec Process -
	//this can result in result conditions such as:
	//EProcessComplete - the codec processed all the source data into the sink buffer
	//EProcessIncomplete - the codec filled sink buffer before all the source buffer was processed
	//EDstNotFilled - the codec processed the source buffer but the sink buffer was not filled
	//EEndOfData - the codec detected the end data - all source data in processed but sink may not be full
	//EProcessError - the codec process error condition

	
	switch (iCodecProcessResult.iStatus)
		{
	case TCodecProcessResult::EProcessComplete:
	//finished procesing source data - all data in sink buffer
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("CMMFDataPath::FillSinkBufferL codec EProcessComplete   (this 0x%x)\n"),this);
#endif
		iSourceBuffer->SetStatus(EAvailable); //source buffer is now avaialble
		iSinkBuffer->SetStatus(EFull);	//sink buffer is full	
		if (iNoMoreSourceData) 
			iSinkBuffer->SetLastBuffer(ETrue);
		ChangeDataPathTransferState(ESendDataToSink);// the full sink buffer needs to be sent to the sink 
		}
	break;
	case TCodecProcessResult::EProcessIncomplete:
		// the sink was filled before all the src was processed 
		// therefore still send everything to sink 
		//but datapath needs to carry on processing the source buffer before it gets more source data
		//when sink has emptied data path needs to send rest of data
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("CMMFDataPath::FillSinkBufferL codec EProcessIncomplete   (this 0x%x)\n"),this);
#endif
		TUint sourceBufferPosition = iCodecProcessResult.iSrcBytesProcessed + iSourceBuffer->Position();
		iSourceBuffer->SetPosition(sourceBufferPosition);//update source buffer position
		iSinkBuffer->SetStatus(EFull); //sink & source buffers are both full
		ChangeDataPathTransferState(ESendDataToSink); // the full sink buffer needs to be sent to the sink 
		}
	break;
	case TCodecProcessResult::EDstNotFilled:
		// the destination is not full 
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("CMMFDataPath::FillSinkBufferL codec EDstNotFilled   (this 0x%x)\n"),this);
#endif
		iSourceBuffer->SetStatus(EAvailable); //source buffer is now available
		TUint sinkBufferPosition = iCodecProcessResult.iDstBytesAdded + iSinkBuffer->Position();
		iSinkBuffer->SetPosition(sinkBufferPosition);//update sink  buffer position (still EBeingFilled)
		// if this was the last source buffer, send what we've got (if anything) 
		// to the sink... EmptySinkBuffer() should then enter EEndOfData state
		if (iNoMoreSourceData)
			{
			iSinkBuffer->SetLastBuffer(ETrue);
			ChangeDataPathTransferState(ESendDataToSink);//send what we've got to the sink - 
			}
		else
			{
			ChangeDataPathTransferState(ENeedSourceData); //need to get more source data to fill sink buffer
			}
		}
	break;	
	case TCodecProcessResult::EEndOfData:
		//no more data - send what we've got to the sink
		//note we can't always rely on this  - in many cases the codec will not know when
		//it has reached the end of data.
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("CMMFDataPath::FillSinkBufferL codec EEndOfData   (this 0x%x)\n"),this);
#endif
		iSourceBuffer->SetStatus(EAvailable); //source buffer is now avaialble
		iSinkBuffer->SetStatus(EFull);//sink buffer may not really be 'full' but its as full as it going to get

		//This only occurs where the codec can detect the end of data, but the source can't
		iNoMoreSourceData=ETrue;
		iSinkBuffer->SetLastBuffer(ETrue); 

		ChangeDataPathTransferState(ESendDataToSink);//send what we've got to the sink - 
		//doesn't matter if sink buffer is not full
		}
	break;
	case TCodecProcessResult::EProcessError:
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::FillSinkBufferL tick-%d  DONE %d   (this 0x%x)\n"),User::TickCount(), __LINE__,this);
#endif
		User::Leave(KErrCorrupt); //codec process error
	break;
	default:
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::FillSinkBufferL tick-%d  DONE %d   (this 0x%x)\n"),User::TickCount(), __LINE__,this);
#endif
		User::Leave(KErrCorrupt); //should never get here
		}
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::FillSinkBufferL - done  tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	}



/**
Tests whether the data path can create a sink buffer.

The default implementation returns false.

@return ETrue if the data path can create a sink buffer. EFalse if the data path cannot create a sink buffer.
*/
EXPORT_C TBool CMMFDataPath::CanCreateSinkBuffer()
	{
	return NULL; //CMMFDataPath cannot create buffer
	}

/**
Creates a sink buffer according to the specifed media ID.

Intended for synchronous usage (buffers supplied by datapath for an MDataSink).
This method is essentially a dummy implementation of an MDataSink pure virtual.

The default implementation returns NULL.

@param  aMediaId
        An optional mediaID parameter when there are multiple buffers arriving of different media types.

@return Returns NULL in this instance as datapath can't create sink buffers
*/
EXPORT_C CMMFBuffer* CMMFDataPath::CreateSinkBufferL(TMediaId /*aMediaId*/)
	{//CMMFDataPath can't create buffers
	return NULL;
	}

/**
Creates a sink buffer according to the specifed media ID and reference.

Intended for asynchronous usage (buffers supplied by Devsound device).
This method is essentially a dummy implementation of an MDataSink pure virtual.

The default implementation returns NULL.

@param  aMediaId
        An optional mediaID parameter when there are multiple buffers arriving for different media types.
@param  aReference
        A boolean indicating buffer ownership.

@return	Returns NULL in this instance as datapath can't create sink buffers.
*/
EXPORT_C CMMFBuffer* CMMFDataPath::CreateSinkBufferL(TMediaId /*aMediaId*/, TBool& /*aReference*/)
	{//CMMFDataPath can't create buffers
	return NULL;
	}

/**
Gets the sink's data type for the specified media ID.

@param  aMediaId
        An optional parameter to specifiy the specific stream when datasource contains more than one stream of data
@return The sink's data type.
*/
EXPORT_C TFourCC CMMFDataPath::SinkDataTypeCode(TMediaId /*aMediaId*/)
	{
	return(iSinkFourCC);
	}

/**
Fills the specified buffer.

Pure virtual dummy implementation, not needed by datapath
comes from MDataSink - CMMFData path is a source to its MDataSink

Only required for an active pull MDataSink requesting a buffer fill. The default implementation is empty.

@param  aBuffer
        The buffer to fill.
@param  aConsumer
        The MDataSink supplying this buffer.
@param  aMediaId
        An optional mediaID parameter when there are multiple buffers arriving of different media types
*/
EXPORT_C void CMMFDataPath::FillBufferL(CMMFBuffer* /*aBuffer*/, MDataSink* /*aConsumer*/, TMediaId /*aMediaId*/)
	{
	//not implementated
	}

void CMMFDataPath::SetBuffersAvailable()
	{
	// set source buffer to be available
  	if (iSourceBuffer)
  		iSourceBuffer->SetStatus(EAvailable);
	// set sink buffer to be available
  	if (iSinkBuffer)
  		iSinkBuffer->SetStatus(EAvailable);
	}

void CMMFDataPath::ResetRefBuffers()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::ResetRefBuffers iSourceBuffer=0x%x ref=%d   iSinkBuffer=0x%x ref=%d (this 0x%x)\n"),iSourceBuffer,iSrcBufRef,iSinkBuffer,iSnkBufRef, this);
#endif

	// Reset the buffer pointers to NULL if they are supplied by DevSound
	// We do this because buffers that are not owned by the datapath may not be valid any more.
	if (iSrcBufRef)
		{
		iSourceBuffer = NULL;
		}
	if (iSnkBufRef)
		{
		iSinkBuffer = NULL;
		}
	}




TInt CMMFDataPath::DetermineBuffersToUseL(void) const
	{
	TInt buffs = ENoBuffers;
	if(iCodec)
		{//Using a real Codec, need both sets of buffers
		if(!iDataSink->CanCreateSinkBuffer() || ! iDataSource->CanCreateSourceBuffer())
			User::Leave(KErrNotSupported);

		buffs = CMMFDataPath::ENeedSinkBuffer | CMMFDataPath::ENeedSourceBuffer;	
		}
	else //we are using a Null Codec, only need one buffer, but which one?
		{//use buffer from DevSound, if no DevSound (ie, clip to clip), prefer source buffer.
		//If preferring source but it can't create buffers, use sink.
		if ((iDataSink->DataSinkType() == KUidMmfAudioOutput) && (iDataSink->CanCreateSinkBuffer()))
			buffs = ENeedSinkBuffer;
		else if(iDataSource->CanCreateSourceBuffer())
			buffs = ENeedSourceBuffer;
		else if(iDataSink->CanCreateSinkBuffer())
			buffs = ENeedSinkBuffer;
		else
			User::Leave(KErrNotSupported);
		}
	return buffs;
	}



/*
 *  InitializeSinkL
 *
 *	Function to initialize iDataSink before it can start sending data
 *	This is a one time prime. This will synchronize DataPath's data driving
 *	mechanism with HwDevice implementation.
 *
 *  This initialisation method detects its attached sources and sinks and makes adjustments to the state machine
 *  This avoid the possibility of NULL buffers (not yet returned by an asychronous sink or source) being passed around the MMF
 */

void CMMFDataPath::InitializeSinkL()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::InitializeSinkL  iSourceBuffer=0x%x ref=%d   iSinkBuffer=0x%x ref=%d (this 0x%x)\n"),iSourceBuffer,iSrcBufRef,iSinkBuffer,iSnkBufRef, this);
#endif

	//state only used if we are passing data
	__ASSERT_DEBUG((iState == EPlaying || iState == EConverting || iState == ERecording), Panic(EMMFDataPathPanicBadState,__LINE__));

	iObtainingAsyncSinkBuffer = EFalse;

	if (iBuffersToUse & ENeedSinkBuffer)
		{
		//Buffers are initially created in the Prime method. But following a pause, we must re-create 
		//any referenced buffers, so try direct creation.
		//NB: this does mean we are trying this twice, Prime and here
		if (!iSinkBuffer) //we may already have a buffer from a previous initialization
			{
			TRAPD(err, iSinkBuffer = iDataSink->CreateSinkBufferL(iMediaId, iSnkBufRef));
			if(err != KErrNone && err != KErrNotSupported)
				User::Leave(err);
			}


		//If buffer has not been supplied via CreateSinkBufferL, 
		//must use asynchronous buffer creation		
		if (!iSinkBuffer) 
			{
			iObtainingAsyncSinkBuffer = ETrue;
			ChangeDataPathTransferState(EWaitSink);  // wait for BufferEmptied callback from sink
			iDataSink->EmptyBufferL(iSinkBuffer, this, iMediaId);
			}
		else
			{
			//we have a sink buffer from CreateSinkBufferL
			iSinkBuffer->SetStatus(EAvailable);
			
			if (iBuffersToUse & ENeedSourceBuffer)
				{//need a source buffer, go get it
				ChangeDataPathTransferState(EInitializeSource);
				}
			else
				{//only need one buffer, use sink
				iSourceBuffer = iSinkBuffer;
				iSrcBufRef = ETrue; //the src buffer is not to be deleted

				ChangeDataPathTransferState(ENeedSourceData); //got all buffers, start getting data
				}
			}
		}
	else
		{//don't need a sink buffer, but we need a source one
		ChangeDataPathTransferState(EInitializeSource);
		}

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::InitializeSinkL - DONE  iSourceBuffer=0x%x ref=%d   iSinkBuffer=0x%x ref=%d (this 0x%x)\n"),iSourceBuffer,iSrcBufRef,iSinkBuffer,iSnkBufRef, this);
#endif	
	}


/*
 *  InitializeSourceL
 *
 *	Function to initialize iDataSource before it can start sending data
 *	This is a one time prime. This will synchronize DataPath's data driving
 *	mechanism with HwDevice implementation.
 *
 *  This initialisation method detects its attached sources and sinks and makes adjustments to the state machine
 *  This avoid the possibility of NULL buffers (not yet returned by an asychronous sink or source) being passed around the MMF
 */

void CMMFDataPath::InitializeSourceL()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::InitializeSourceL -  iSourceBuffer=0x%x ref=%d   iSinkBuffer=0x%x ref=%d (this 0x%x)\n"),iSourceBuffer,iSrcBufRef,iSinkBuffer,iSnkBufRef, this);
#endif	

	//state only used if we are passing data
	__ASSERT_DEBUG((iState == EPlaying || iState == EConverting || iState == ERecording), Panic(EMMFDataPathPanicBadState,__LINE__));

	iObtainingAsyncSourceBuffer = EFalse;

	if (iBuffersToUse & ENeedSourceBuffer)
		{
		//Buffers are initially created in the Prime method. But following a pause, we must re-create 
		//any referenced buffers, so try direct creation.
		//NB: this does mean we are trying this twice, Prime and here.
		if (!iSourceBuffer) //we may already have a buffer from a previous initialization
			{
			TRAPD(err, iSourceBuffer = iDataSource->CreateSourceBufferL(iMediaId,*iSinkBuffer, iSrcBufRef));
			if(err != KErrNone && err != KErrNotSupported)
				User::Leave(err);
			}


		//If buffer has not been supplied via CreateSourceBufferL
		//must use asynchronous buffer creation
		if (!iSourceBuffer) 
			{
			iObtainingAsyncSourceBuffer = ETrue;
			ChangeDataPathTransferState(ENeedSourceData);			
			}
		else
			{//we have a source buffer from CreateSourceBufferL
			iSourceBuffer->SetStatus(EAvailable);

			if (!(iBuffersToUse & ENeedSinkBuffer))
				{//only need one buffer, use sink
				iSinkBuffer = iSourceBuffer;
				iSnkBufRef = ETrue;
				}

			ChangeDataPathTransferState(ENeedSourceData); //got all buffers, start getting data			
			}
		}
	else
		{//don't need a source buffer, use sinks
		if(iSinkBuffer)
			{
			iSourceBuffer = iSinkBuffer;
			iSrcBufRef = iSnkBufRef;
			SetBuffersAvailable();
			}
#ifdef _DP_DEBUG
		else
			Panic(EMMFDataPathPanicProgrammingError,__LINE__);
#endif
		ChangeDataPathTransferState(ENeedSourceData); //got all buffers, start getting data

		
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::InitializeSourceL -  iSourceBuffer=0x%x ref=%d   iSinkBuffer=0x%x ref=%d (this 0x%x)\n"),iSourceBuffer,iSrcBufRef,iSinkBuffer,iSnkBufRef, this);
#endif			
		}
	}

/*
 *  EmptySinkBufferL
 * 
 *	Function to pass a full databuffer to the iDataSink
 */
void CMMFDataPath::EmptySinkBufferL()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::EmptySinkBufferL pass data to sink  tick-%d   (this 0x%x)\n"),User::TickCount(),this);
	if(iSinkBuffer)
		RDebug::Print(_L("iSinkBuffer %d contains %d bytes  eof = %d line %d   (this 0x%x)\n"),iSinkBuffer->FrameNumber(), iSinkBuffer->BufferSize(),iSinkBuffer->LastBuffer(),__LINE__,this);		
#endif

	//Before emptying the sink buffer we need to check it has data to empty - this
	//may not be the case if there is no more data ie iNoMoreSourceData is true.
	//In this case we need to check to see if there is any data left in the sink 
	//buffer, ie the sink buffer is either full or being filled. If there is not any
	//data in the sink buffer, ie it is not in the state of EBeingFilled or EFull
	//then there is nothing to empty so the datapath state is set to EEndOfData and
	//we return from the procedure.

	//state only used if we are passing data
	__ASSERT_DEBUG((iState == EPlaying || iState == EConverting || iState == ERecording || (iState == EPrimed && iPauseCalled)), Panic(EMMFDataPathPanicBadState,__LINE__)); 
	__ASSERT_DEBUG(iSinkBuffer &&
				   ((iSinkBuffer->Status()==EBeingFilled) || (iSinkBuffer->Status()==EFull)),
				   Panic(EMMFDataPathPanicProgrammingError,__LINE__)); 

	__ASSERT_DEBUG(iSinkBufferWithSink == EFalse, Panic(EMMFDataPathPanicBadState,__LINE__)); 


	//Due to sinks that may call BuferEmptied directly (ie. re-entrancy onto DataPath) we
	//must work out next state here. If re-entrancy, the next state may validly get overwritten 
	// in BuferEmptied.
	if(iObtainingAsyncSinkBuffer) //wait for buffer to be returned in BufferEmptied
		{
		ChangeDataPathTransferState(EWaitSink);  // wait for BufferEmptied callback from sink
		}

#ifdef REPOSITION_SPEEDUP
	// if the source has been re-positioned, 
	// speed things up by getting some more source data now
	if(iSourceBuffer && iSourceBuffer->FrameNumber() != iCurrentSourceFrameNumber)
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::EmptySinkBufferL() source was re-positioned re-requesting source data (this 0x%x)\n"),this);
#endif
		ChangeDataPathTransferState(ENeedSourceData);
		return;
		}
#endif //REPOSITION_SPEEDUP

	//We have sent data to sink, if we are using a real Codec, we can now get more data from source
	//if there is any more to get and the codec has emptied it.
	//NB: No need to check we own the source buffer as we will no be in this state
	//if we have asked for more source data and haven't received it.
	else if (iCodec && !iNoMoreSourceData && (iSourceBuffer->Status() == EAvailable))
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("ASKING for more source data iCodec = 0x%x  iNoMoreSourceData=%d  iSourceBufferWithSource = %d   (this 0x%x)\n"), iCodec, iNoMoreSourceData, iSourceBufferWithSource,this);
#endif
		ChangeDataPathTransferState(ENeedSourceData);
		}
	else
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("Not asking for any more source data iCodec = 0x%x  iNoMoreSourceData=%d  iSourceBufferWithSource = %d   iSourceBuffer->Status=%d   (this 0x%x)\n"), iCodec, iNoMoreSourceData, iSourceBufferWithSource ,iSourceBuffer->Status(), this);
#endif

		//if this is the last buffer, set this flag so we can deal with KErrUnderflow 
		//as a valid termination of playing
		if(iSinkBuffer->LastBuffer())
			iAllDataSentToSink=ETrue;

		ChangeDataPathTransferState(EWaitSink);  // wait for BufferEmptied callback from sink
		}


	if(!iObtainingAsyncSinkBuffer) //normal data transfer
		iSinkBuffer->SetFrameNumber(++iCurrentSinkFrameNumber);

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP sending buffer %d ptr=0x%x of %d bytes to sink   (this 0x%x)\n"), iSinkBuffer->FrameNumber(),iSinkBuffer,iSinkBuffer->BufferSize(),this);
#endif
	
	iSinkBufferWithSink = ETrue;
	TRAPD(error, iDataSink->EmptyBufferL(iSinkBuffer, this, iMediaId));

	// Check that we haven't exceeded the maximum clip length - if so, go to the EndOfData state
	// so we perform necessary cleanup.
	if (error == KErrEof || error == KErrOverflow || error == KErrUnderflow)
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::EmptySinkBufferL DONE %d error = %d  tick-%d   (this 0x%x)\n"),__LINE__, error, User::TickCount(),this);
#endif
		iDataPathCompletedErrorCode = error;
		ChangeDataPathTransferState(EEndOfData);
		return;
		}
	User::LeaveIfError(error);

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::EmptySinkBufferL - done  tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	}


/** 
Indicates the data sink has emptied the buffer.

Called by the CMMFDataPath's MDataSink when it has emptied the buffer

@param  aBuffer
		The emptied buffer.
*/
EXPORT_C void CMMFDataPath::BufferEmptiedL(CMMFBuffer* aBuffer)
	{
#ifdef _DP_DEBUG
	TInt bufNum = 9999;
	if(aBuffer)
		bufNum = aBuffer->FrameNumber();
	else
		RDebug::Print(_L("DP::BufferEmptiedL returned NULL   (this 0x%x)\n"),this);

	RDebug::Print(_L("DP::BufferEmptiedL sink has taken buffer %d (ptr=0x%x) bytes %d eof= %d   iNoMoreSourceData = %d tick-%d   (this 0x%x)\n"),
		bufNum, aBuffer, aBuffer->BufferSize(),aBuffer->LastBuffer(), iNoMoreSourceData, User::TickCount(),this);
#endif

	iSinkBufferWithSink = EFalse;

    //Has the datapath stopped running, if so were not interested in any callbacks.
    if(iState == EStopped || (iState == EPrimed && !iPauseCalled))
        {
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::BufferEmptiedL called while not expecting callback iState=%d  iPauseCalled=%d  (this 0x%x)\n"),iState, iPauseCalled,this);
#endif
		return;
		}


	// This will allow MDataSink to send dynamic buffer to DataPath with each BufferEmptiedL request.
	if (iSinkBuffer != aBuffer) //buffer has been updated
		{
		iSinkBuffer = aBuffer;
		if (!(iBuffersToUse & ENeedSourceBuffer))
			{ //can use a single buffer
			iSourceBuffer = iSinkBuffer;
			iSrcBufRef = iSnkBufRef;
			}

		}

	iSinkBuffer->SetStatus(EAvailable);
	
	if (iObtainingAsyncSinkBuffer) //we are creating an asynchronous sink buffer
		{
		iObtainingAsyncSinkBuffer = EFalse;

		//we have a sink buffer, should this also be used by the source
		if (!(iBuffersToUse & ENeedSourceBuffer))
			{//using a single buffer, so start getting data
			iSourceBuffer = iSinkBuffer;
			iSrcBufRef = iSnkBufRef;

			ChangeDataPathTransferState(ENeedSourceData);
			}
		else //obtain a separate source buffer
			ChangeDataPathTransferState(EInitializeSource);

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::BufferEmptiedL - DONE iSourceBuffer=0x%x ref=%d   iSinkBuffer=0x%x ref=%d (this 0x%x)\n"),iSourceBuffer,iSrcBufRef,iSinkBuffer,iSnkBufRef, this);
#endif	
		return;
		}

	if(!iCodec) //No Codec in use
		{
		if(iNoMoreSourceData)
			ChangeDataPathTransferState(EEndOfData);//final buffer returned from sink
		else
			ChangeDataPathTransferState(ENeedSourceData);//get more data from source
		}
	else //real codecs
		{
		//There is more source data and src buffer is being filled or we are about to go into 
		// ENeedSourceData state to fill it, so wait for source data to arrive.
		//NB:if there was more source data and we are using a real codec and source buffer was empty,
		//more source data would have been requested in EmptySinkBuffer
		if(!iNoMoreSourceData && (iSourceBufferWithSource || iTransferState == ENeedSourceData))
			{
#ifdef _DP_DEBUG
			RDebug::Print(_L("DP::BufferEmptiedL - waiting for more source data - DONE tick-%d line %d   (this 0x%x)\n"),User::TickCount(),__LINE__,this);
#endif
			return;
			}

		//source has supplied a NULL buffer or it has been emptied; no more data to send.
		if(!iSourceBuffer || (iSourceBuffer->Status() == EAvailable))
			ChangeDataPathTransferState(EEndOfData);
		else if(iSourceBuffer->Status() == EFull) //there is data in the source buffer, go and get it
			ChangeDataPathTransferState(ENeedToMatchSourceToSink);

#ifdef _DP_DEBUG
		else
			Panic(EMMFDataPathPanicProgrammingError,__LINE__);
#endif
		}


#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::BufferEmptiedL - DONE  tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	}



/**
Tests whether the data path can create a source buffer.

Would expect datapath to always return NULL, so this is a default implementation of a pure virtual from MDataSink.

The default implementation returns EFalse.

@return ETrue if the data path can create a source buffer. EFalse if the data path cannot create a source buffer.
*/
EXPORT_C TBool CMMFDataPath::CanCreateSourceBuffer() //from both MDataSource & MDataSink?
	{
	return EFalse; //CMMFDataPath cannot create buffer
	}

/**
Creates a source buffer.

Intended for synchronous usage (buffers supplied by datapath for a MDataSource)
This method is essentially a dummy implementation of an MDataSource pure virtual.

The default implementation leaves with KErrNotSupported and returns NULL.

@param  aMediaId
        An optional mediaID parameter when there are multiple buffers arriving of different media types.
@return A pointer to a newly created buffer. Returns NULL in this instance as datapath can't create source buffers
*/
EXPORT_C CMMFBuffer* CMMFDataPath::CreateSourceBufferL(TMediaId /*aMediaId*/) //CMMFDataPath can't create buffers
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

/**
Creates a source buffer according to the specifed media ID and reference.

Intended for asynchronous usage (buffers supplied by datapath for a MDataSource)
This method is essentially a dummy implementation of an MDataSource pure virtual.

The default implementation leaves with KErrNotSupported and returns NULL.

@param  aMediaId
        An optional mediaID parameter when there are multiple buffers arriving of different media types.
@param  aReference
        A boolean indicating buffer ownership. ETrue if the MDataSource owns the buffer, EFalse if the caller owns the buffer.

@return A pointer to a newly created buffer. Returns NULL in this instance as datapath can't create source buffers.
*/
EXPORT_C CMMFBuffer* CMMFDataPath::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool& /*aReference*/) //CMMFDataPath can't create buffers
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}


/**
Gets the source data type for the specified media ID.

@param  aMediaId
        An optional parameter to specifiy specific stream when datasource contains more than one stream of data.

@return The source data type.
*/
EXPORT_C TFourCC CMMFDataPath::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	return(iSourceFourCC);
	}


/**
Allocates buffers in preparation to play.

Must be called before calling PlayL().

iSnkBufRef and iSrcBufRef contain ETrue if these buffers are created and owned by a MDataSource or MDataSink
For clean-up purposes, datapath only cleans up buffers allocated directly by PrimeL().
*/

EXPORT_C void CMMFDataPath::PrimeL()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::PrimeL  tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	
	//allocate resources ie buffers & prepare to play
	if (iDataPathCreated && (iState == EStopped))
		//luckily the client utility does this.
		{//can only prime from the stopped state
		
		//This will determine what buffers we need to run the datapath.
		//Can leave KERRNotSupported
		iBuffersToUse = DetermineBuffersToUseL();

		//Try to create source and sink buffers. If we can't create them in the Prime, 
		//we will need to obtain them by asynchronous buffer creation when playing starts.
		ObtainSyncBuffersL();

		iDataSource->SourcePrimeL(); //propogate state change to source
		iDataSink->SinkPrimeL(); //propogate state change to sink


		//If Client has set these, they will be set following the prime
		iPlayWindowStartPosition = 0;
		iPlayWindowEndPosition = Duration();

		iState = EPrimed;	
		iPauseCalled = EFalse;

		if (iCompleteCallback)
			{
			delete iCompleteCallback;
			iCompleteCallback = NULL;
			}
		TBool waitForSink = (iDataSink->DataSinkType() == KUidMmfAudioOutput)?ETrue:EFalse;
		iCompleteCallback = new (ELeave) CCompleteCallback(*this,waitForSink);
		}		
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::PrimeL  Done  tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	}


/**
Starts an active scheduler 'play' loop.

Can only play from the primed state.
*/
EXPORT_C void CMMFDataPath::PlayL()
	{
#if defined(__PROFILING)
	RDebug::ProfileEnd(1);
#endif  // defined(__PROFILING)

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::PlayL, on src buff %d  sink buf %d (this 0x%x)\n"), iCurrentSourceFrameNumber, iCurrentSinkFrameNumber,this);		
	RDebug::Print(_L("iStartPosition = %d\n"), I64INT(iStartPosition.Int64()));		
#endif

	if ((iDataPathCreated) && (iState == EPrimed))
		{
		//can only play from the primed state

		TBool savedPauseCalled=EFalse;
		if(iPauseCalled) //sink and source will have been stopped, and we will not have been re-primed
			{
			savedPauseCalled=ETrue;
			iDataSink->SinkPrimeL(); //propagate change down to sink
			iPauseCalled = EFalse;
			}

		iCurrentSourceFrameNumber = 0; //reset to beginning
		iCurrentSinkFrameNumber = 0; //reset to beginning

		iSourceBufferWithSource = EFalse;
		iSinkBufferWithSink = EFalse;

		iNoMoreSourceData = EFalse;
		iAllDataSentToSink=EFalse;
		iDataPathCompletedErrorCode=KErrNone;

		SetPositionL( iStartPosition ) ;
		iReferenceAudioSamplesPlayed = 0;
		iReferenceAudioSamplesRecorded = 0;

		//complete a request on iStatus to invoke play code
		iDataSource->SourcePlayL(); //propagate state change to source
		if (!(savedPauseCalled && (iTransferState==EWaitSink || iTransferState==EInitializeSink)))
			{
			iDataSink->SinkPlayL(); //propogate state change to sink
			}

		if (iDataSink->DataSinkType() == KUidMmfAudioOutput)
			iState = EPlaying;
		else if(iDataSource->DataSourceType() == KUidMmfAudioInput)
			iState = ERecording;
		else
			iState = EConverting;

		//need to re-initialize any buffer(s) that we only own references to
		ChangeDataPathTransferState(EInitializeSink);
		}
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::Play - DONE\n"));		
#endif
	}


/** 
Pauses playing.

Sends KMMFErrorCategoryDataPathGeneralError to the client if an error occurs.
*/
EXPORT_C void CMMFDataPath::Pause()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::Pause, on src buff %d  sink buf %d   (this 0x%x)\n"), iCurrentSourceFrameNumber, iCurrentSinkFrameNumber,this);			
	RDebug::Print(_L("DP::Pause current state=%d  tick-%d    (this 0x%x)\n"),iTransferState, User::TickCount(),this);
#endif

	TRAPD(err, DoPauseL());
	
	if (err)
		{
		DoSendEventToClient(KMMFErrorCategoryDataPathGeneralError, err);
		}
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::Pause - DONE tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	}

/** 
Stops playing.

Resets datapath position - currently does not clean up buffers. Sends KMMFErrorCategoryDataPathGeneralError 
to the client if an error occurs.
*/
EXPORT_C void CMMFDataPath::Stop()
	{ 
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::Stop current state=%d  tick-%d   (this 0x%x)\n"), iTransferState, User::TickCount(),this);
#endif

	if ((iDataPathCreated)  && (iState != EStopped))
		{ 
		TRAPD(err, DoStopL());
		
		if (err)
			DoSendEventToClient(KMMFErrorCategoryDataPathGeneralError, err);
		}
	}
/**
Forces and end of data state on the datapath
*/
EXPORT_C void CMMFDataPath::EndOfData()
	{
	TRAPD(err, DoEndOfDataL());
	
	if (err)
		{
		DoSendEventToClient(KMMFErrorCategoryDataPathGeneralError, err);
		}
	}

/**

*/
TInt CMMFDataPath::AudioSamplesPlayed() const
	{
	if (iDataSink->DataSinkType() != KUidMmfAudioOutput)
		return 0;

	CMMFAudioOutput* audioOutput = STATIC_CAST(CMMFAudioOutput*,iDataSink);

	TInt samples = 0;

	if(iState == EPlaying)
		samples = audioOutput->SoundDevice().SamplesPlayed();

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::AudioSamplesPlayed = %d\n"),samples);
#endif

	return samples;
	}


/** 

*/
TInt CMMFDataPath::AudioSamplesRecorded() const
	{
	if (iDataSource->DataSourceType() != KUidMmfAudioInput)
		return 0;

	CMMFAudioInput* audioInput = STATIC_CAST(CMMFAudioInput*,iDataSource);

	TInt samples = 0;

	if(iState == ERecording)
		samples = audioInput->SoundDevice().SamplesRecorded();

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::AudioSamplesRecorded = %d\n"),samples);
#endif

	return samples;
	}


/**

*/
TTimeIntervalMicroSeconds CMMFDataPath::CalculateAudioOutputPosition() const
    {
	//This operation can only be carried out on an Audio Output
	__ASSERT_ALWAYS(iDataSink->DataSinkType() == KUidMmfAudioOutput, Panic(EMMFDataPathPanicProgrammingError,__LINE__));


	//If we are not playing, simply return where we will play from
	if(iState != EPlaying || iCurrentSinkFrameNumber == 0)
		return iStartPosition;

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::CalculateAudioOutputDuration from %d\n"),iReferenceAudioSamplesPlayed);
#endif

	TReal samplesPlayed = AudioSamplesPlayed() - iReferenceAudioSamplesPlayed;

	CMMFAudioOutput* audioOutput = STATIC_CAST(CMMFAudioOutput*,iDataSink);
	CMMFDevSound& devSound = audioOutput->SoundDevice();

	TMMFCapabilities devSoundConfig = devSound.Config();
		
	TInt samplingFreq = StreamUtils::SampleRateAsValue(devSoundConfig);

#ifdef _DP_DEBUG
	RDebug::Print(_L("samplingFreq %d\n"),samplingFreq);
#endif

	TReal timePlayedSeconds = 0;
	if(samplesPlayed)
		timePlayedSeconds = samplesPlayed/samplingFreq;

	TInt64 timePlayed(I64DOUBLECAST(timePlayedSeconds * 1000000));

#ifdef _DP_DEBUG
	RDebug::Print(_L("timePlayed %d\n"), I64LOW(timePlayed));
#endif

	return TTimeIntervalMicroSeconds(timePlayed + iStartPosition.Int64());
    }



/**

*/
TTimeIntervalMicroSeconds CMMFDataPath::CalculateAudioInputPosition() const
    {
	//This operation can only be carried out on an Audio Input
	__ASSERT_ALWAYS(iDataSource->DataSourceType() == KUidMmfAudioInput, Panic(EMMFDataPathPanicProgrammingError,__LINE__));


	//If we are not playing, simply return where we will play from
	if(iState != ERecording)
		return iStartPosition;

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::CalculateAudioInputPosition from %d\n"),iReferenceAudioSamplesRecorded);
#endif

	TReal samplesRecorded = AudioSamplesRecorded() - iReferenceAudioSamplesRecorded;

	CMMFAudioInput* audioInput = STATIC_CAST(CMMFAudioInput*,iDataSource);
	CMMFDevSound& devSound = audioInput->SoundDevice();

	TMMFCapabilities devSoundConfig = devSound.Config();
		
	TInt samplingFreq = StreamUtils::SampleRateAsValue(devSoundConfig);

#ifdef _DP_DEBUG
	RDebug::Print(_L("samplingFreq %d\n"),samplingFreq);
#endif

	TReal timeRecordedSeconds = 0;
	if(samplesRecorded)
		timeRecordedSeconds = samplesRecorded/samplingFreq;

	TInt64 timeRecorded(I64DOUBLECAST(timeRecordedSeconds * 1000000));

#ifdef _DP_DEBUG
	RDebug::Print(_L("timeRecorded %d\n"), I64LOW(timeRecorded));
#endif
	return TTimeIntervalMicroSeconds(timeRecorded);
    }





TTimeIntervalMicroSeconds CMMFDataPath::OutputPosition() const
	{
	TTimeIntervalMicroSeconds interval;
	TTimeIntervalMicroSeconds position;

    if (iDataSink->DataSinkType() == KUidMmfAudioOutput)
        {
		position = CalculateAudioOutputPosition();
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::OutputPosition from audio output= %d\n"),I64INT(position.Int64()));
#endif
        }
	else if (iDataSink->DataSinkType() == KUidMmfFormatEncode)
		{
		//note Encode format position takes priority if both source & sink are formats?
        // try to get the position directly from the format. If that fails, work it out here
        TRAPD(error, position = ((CMMFFormatEncode*)iDataSink)->PositionL());
        if (error)//getting the position from the format didn't work so calculate it here
            {
		    interval = ((CMMFFormatEncode*)iDataSink)->FrameTimeInterval(iMediaId);
			TInt64 position64 = interval.Int64() * iCurrentSinkFrameNumber;
			position = position64;
            }

		TTimeIntervalMicroSeconds duration = CONST_CAST(CMMFDataPath*,this)->Duration(); //need this to check position doesn't exceed duration
		if (position > duration)//this can happen on last buffer 
			position = duration;


#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::OutputPosition  from format= %d\n"),I64INT(position.Int64()));
#endif
		}
	else
		{//can only read output position if sink is a format or an audio output
		return TTimeIntervalMicroSeconds(0);
		}

	return position;
	}

TTimeIntervalMicroSeconds CMMFDataPath::InputPosition() const
	{
	TTimeIntervalMicroSeconds interval;
	TTimeIntervalMicroSeconds position;

    if (iDataSource->DataSourceType() == KUidMmfAudioInput)
        {
		position = CalculateAudioInputPosition();
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::InputPosition from audio input= %d\n"),I64INT(position.Int64()));
#endif
        }
	else if (iDataSource->DataSourceType() == KUidMmfFormatDecode)
		{//note Decode format position takes priority if both source & sink are formats?
        // try to get the position directly from the format. If that fails, work it out here
        TRAPD(error, position = ((CMMFFormatDecode*)iDataSource)->PositionL());
        if (error)//getting the position from the format didn't work so calculate it here
            {
		    interval = ((CMMFFormatDecode*)iDataSource)->FrameTimeInterval(iMediaId);
		    TInt64 position64 = interval.Int64() * iCurrentSourceFrameNumber;
		    position = position64;
            }

		TTimeIntervalMicroSeconds duration = CONST_CAST(CMMFDataPath*,this)->Duration(); //need this to check position doesn't exceed duration
		if (position > duration)//this can happen on last buffer 
			position = duration;

#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::InputPosition from format = %d\n"),I64INT(position.Int64()));
#endif
		}
	else
		{//can only read input position if source is a format or an audio input
		return TTimeIntervalMicroSeconds(0);
		}

	return position;	
	}




/** 
Gets the data path position.

@return The data path position.
*/
EXPORT_C  TTimeIntervalMicroSeconds CMMFDataPath::Position() const
	{
	if ((iState == ERecording) || (iState == EConverting))
		return InputPosition();
	else if(iState == EPlaying)
		return OutputPosition();
	else
		{
		return iStartPosition;
		}
	}

/** 
Sets the data path position.

@param  aPosition
		The data path position.
*/
EXPORT_C void CMMFDataPath::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
	{//need to map to source position to frame position 
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::SetPositionL = %d  ticks-%d   (this 0x%x)\n"),I64INT(aPosition.Int64()), User::TickCount(),this);
#endif

	if (iState == EStopped)
		User::Leave(KErrNotReady); //can only set position if primed

	//As this will affect the position, we need to know how many bytes were 
	//played when position was updated. Future Position() requests will
	//then use this refernce to determine the current position.
	iReferenceAudioSamplesPlayed = AudioSamplesPlayed();

	// Force the new position to be inside the play window (also within the file duration)
	if ( aPosition < iPlayWindowStartPosition )
		iStartPosition = iPlayWindowStartPosition;
	else if ( aPosition > iPlayWindowEndPosition )
		iStartPosition = iPlayWindowEndPosition; //clearly this will cause nothing to be played
	else
		iStartPosition = aPosition;

	TTimeIntervalMicroSeconds interval;

	//can only set the position on an MDataSource that is a format object
	//Note: position defaults to source if both source & sink are clips
	if (iDataSource->DataSourceType() == KUidMmfFormatDecode)
		{
		//position is not beyond the end of file
		interval = ((CMMFFormatDecode*)iDataSource)->FrameTimeInterval(iMediaId);

		// for some reason this code won't compile without these intermediate steps
		TInt64 position = iStartPosition.Int64();
		TInt64 interval64 = interval.Int64();
		if (interval64 == 0)
			User::Leave(KErrDivideByZero); 
		TInt64 datapos64 = position/interval64; 
		iCurrentSourceFrameNumber = I64LOW(datapos64);


        // Try to set the position directly on the format
        TRAP_IGNORE(((CMMFFormatDecode*)iDataSource)->SetPositionL(iStartPosition));
        //NB: don't worry about error, since we'll reposition anyway when we get the next buffer
		}
	else if (iDataSink->DataSinkType() == KUidMmfFormatEncode)
		{			
		//position is not beyond the end of file
		interval = ((CMMFFormatEncode*)iDataSink)->FrameTimeInterval(iMediaId);

		//convert to TUint - for some reason it won't compile without these intermediate steps
		TInt64 position = iStartPosition.Int64();
		TInt64 interval64 = interval.Int64();
		if (interval64 == 0)
			User::Leave(KErrDivideByZero); 
		TInt64 datapos64 = position/interval64; 
		iCurrentSinkFrameNumber = I64LOW(datapos64);


        // Try to set the position directly on the format
        TRAP_IGNORE(((CMMFFormatEncode*)iDataSink)->SetPositionL(iStartPosition));
        //NB: don't worry about error, since we'll reposition anyway when we get the next buffer
		}
	else
		{//can only set position if source or sink is a format
		//If both source and sink are formats position is relative to the source
		User::Leave(KErrNotSupported); //can't set position if neither source nor sink are clips
		}

	if(iCodec) //we have a real codec, must reset it
		iCodec->ResetL(); // Need to preserve sync when resuming play

	// Once we've sent the last buffer to the sink it's too late to start
	// changing the state since we may get a RunError(KErrUnderflow) at any time.
	// Once this happens, the sound driver may have unloaded etc..and recovery
	// would be complicated.
	if (iAllDataSentToSink)
		return;

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::SetPosition - Done iCurrentSourceFrameNumber=%d  iStartPosition=%d  ticks-%d   (this 0x%x)\n"),iCurrentSourceFrameNumber, I64INT(iStartPosition.Int64()), User::TickCount(),this);	
#endif
	}




/** 
Sets the play window absolutely (i.e. the parameters are relative to the start of the entire clip).

@param  aStart
        The offset from the start of the Clip
@param  aEnd
        The offset from the end of the clip (if this is less than aStart, then the two will be inverted).
*/
EXPORT_C void CMMFDataPath::SetPlayWindowL( const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd ) 
	{
	// Clear the existing Play window
	ClearPlayWindowL() ;
	
	// Check that the parameters are legitimate.  0 <= startpos < endpos <= duration & update member variables
	TTimeIntervalMicroSeconds duration = Duration();
		
	if ( aStart < TTimeIntervalMicroSeconds(0) ) 
		iPlayWindowStartPosition = TTimeIntervalMicroSeconds(0);
	else if ( aStart > duration )
		iPlayWindowStartPosition = duration;
	else iPlayWindowStartPosition = aStart;

	if ( aEnd < TTimeIntervalMicroSeconds(0) )
		iPlayWindowEndPosition = TTimeIntervalMicroSeconds(0);
	else if ( aEnd > duration )
		iPlayWindowEndPosition = duration;
	else iPlayWindowEndPosition = aEnd;

	// ensure that the current position is inside the new play window
	if ( iPlayWindowEndPosition != TTimeIntervalMicroSeconds(0) )
		{
		TTimeIntervalMicroSeconds currentPosition = Position() ;
		if ( currentPosition < iPlayWindowStartPosition )
			SetPositionL( iPlayWindowStartPosition ) ;
		else if ( currentPosition > iPlayWindowEndPosition )
			SetPositionL( iPlayWindowEndPosition ) ;
		}
	else
		ClearPlayWindowL() ;

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::SetPlayWindowL iPlayWindowStartPosition=%d iPlayWindowEndPosition=%d\n"),I64INT(iPlayWindowStartPosition.Int64()),I64INT(iPlayWindowEndPosition.Int64()));
#endif
	}


/**
Sets the play window to the full length of clip.
*/
EXPORT_C void CMMFDataPath::ClearPlayWindowL() 
	{
	iPlayWindowStartPosition = TTimeIntervalMicroSeconds(0) ;
	iPlayWindowEndPosition = Duration();


	if(iState == EStopped)
		iStartPosition = iPlayWindowStartPosition;
	}

/**
Returns the current data path state.
*/
EXPORT_C  TInt CMMFDataPath::State()
	{
	return iState ;
	}


/**
Uses the AO mechanism to drive state changes between sources and sinks.

RunL() moves and assigns buffers between its attached MDataSource/MDataSinks.
*/
void CMMFDataPath::ChangeDataPathTransferState(TTransferState aNewDataPathTransferState)
	{
#ifdef _DP_DEBUG
		switch (aNewDataPathTransferState)
			{
		case EWaitSink:
	RDebug::Print(_L("Next State EWaitSink ticks-%d   (this 0x%x)\n"),User::TickCount(),this);
			break;
		case EWaitSource:
	RDebug::Print(_L("Next State EWaitSource ticks-%d   (this 0x%x)\n"),User::TickCount(),this);
			break;
		case EInitializeSink:
	RDebug::Print(_L("Next State EInitializeSink ticks-%d   (this 0x%x)\n"),User::TickCount(),this);
			break;
		case EInitializeSource:
	RDebug::Print(_L("Next State EInitializeSource ticks-%d   (this 0x%x)\n"),User::TickCount(),this);
			break;
		case ENeedSourceData:
	RDebug::Print(_L("Next State ENeedSourceData ticks-%d   (this 0x%x)\n"),User::TickCount(),this);
			break;
		case ENeedSinkData:
	RDebug::Print(_L("Next State ENeedSinkData ticks-%d   (this 0x%x)\n"),User::TickCount(),this);
			break;
		case ENeedToMatchSourceToSink:
	RDebug::Print(_L("Next State ENeedToMatchSourceToSink ticks-%d   (this 0x%x)\n"),User::TickCount(),this);
			break;
		case ESendDataToSink:
	RDebug::Print(_L("Next State ESendDataToSink ticks-%d   (this 0x%x)\n"),User::TickCount(),this);
			break;
		case EEndOfData:
	RDebug::Print(_L("Next State EEndOfData ticks-%d   (this 0x%x)\n"),User::TickCount(),this);
			break;
			}
#endif


	TRequestStatus* stat = &iStatus;
	//change state
	iTransferState = aNewDataPathTransferState;
	if ((iTransferState != EWaitSink) && (iTransferState != EWaitSource) && 
		(iState == EPlaying || iState == ERecording || iState == EConverting || (iState == EPrimed && iPauseCalled)))
		{//can go ahead with transfer
		if (!IsActive())
			{
			User::RequestComplete(stat, KErrNone);
			SetActive();
			}
		}
#ifdef _DP_DEBUG
	else
		RDebug::Print(_L("Datapath is no longer active, not going to new state (this 0x%x)\n"),this);
#endif
	}

/**
Runs the clip depending on the current data path and transfer state.

For example, fills the sink buffer if TDataPathState is EPlaying and TTransferState is ENeedSinkData.
*/
EXPORT_C void CMMFDataPath::RunL()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::RunL tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	
	switch (iState)
		{
	case EStopped:
		break;
	case EPrimed:
		//paused with stored position
		break;
	case EPlaying:
	case ERecording:
	case EConverting:
		switch (iTransferState)
			{
		case EWaitSink:
		case EWaitSource:
			break;
		case EInitializeSink:
			InitializeSinkL();
			break;
		case EInitializeSource:
			InitializeSourceL();
			break;
		case ENeedSourceData:
			FillSourceBufferL();
			break;
		case ENeedSinkData:
			FillSinkBufferL();
			break;
		case ENeedToMatchSourceToSink:
			FillSinkBufferL();
			break;
		case ESendDataToSink:
			EmptySinkBufferL();
			break;
		case EEndOfData:
			EndOfData();
			break;
			}
		break;
	default:
		break;
		}
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::RunL DONE\n"));
#endif
	}

/**
Cancels the clip.

The default implementation is empty.
*/
EXPORT_C void CMMFDataPath::DoCancel()
	{
	//don't need to do anything as we don't have any async requests to other objects
	}

/**
Handles errors coming from attached sources and passes them to the clients.

@param  aError
        Standard error code (KErrNone = No Error).

@return The event code returned to the data path. KErrNone if end of file is encountered.
*/
EXPORT_C TInt CMMFDataPath::RunError(TInt aError)
	{
	return DoSendEventToClient(KMMFErrorCategoryDataPathGeneralError, aError);
	}


/**
Returns the duration of the clip.

@return The length of clip in TTimeIntervalMicroSeconds.
*/
TTimeIntervalMicroSeconds CMMFDataPath::Duration() const
	{
	TTimeIntervalMicroSeconds duration(0);

	if ( iDataSource &&  ( iDataSource->DataSourceType() == KUidMmfFormatDecode ) )
		{
		//this updated version of datapath caches the duration of the
		//source clip for efficiency - since this meathod is const
		//we need to cast away the constness to set iCachedSourceDuration
		CMMFDataPath* thisNonConst = const_cast<CMMFDataPath*>(this);
		duration = STATIC_CAST(CMMFFormatDecode*, thisNonConst->iDataSource )->Duration( iMediaId ) ;
		thisNonConst->iCachedSourceDuration = duration;
		}
	else if ( iDataSink && ( iDataSink->DataSinkType() == KUidMmfFormatEncode ) )
		duration = STATIC_CAST(CMMFFormatEncode*, iDataSink )->Duration( iMediaId ) ;

	return duration ;
	}

/**
This is a virtual function datapath (or derivations off) that can be implemented but may be left blank for default behaviour.

Additional Stop() method specific to this datapath.
*/
void CMMFDataPath::DoStopL()
	{
	// Note that the datapath needs to be paused first
	// before it can be stopped - this is important for audio play
	// for instance to effect an immediate stop rather than playing out
	// the current buffer.



	// Stop data source and sink
	iDataSource->SourceStopL();					// propagate state change to source
	iDataSink->SinkStopL();						// propagate change down to sink

	iSinkBufferWithSink = EFalse;
	iSourceBufferWithSource = EFalse;

	//Contains the completion code if the datapath completes as a result of an error
	iDataPathCompletedErrorCode = KErrNone;

	ResetRefBuffers();							// buffer references may not be valid any more

	SetPositionL(iPlayWindowStartPosition);		// reset position

	iState = EStopped;							// stop succeeded, set state to stopped
	}

/**
This is a virtual function datapath (or derivations off) that can be implemented but may be left blank for default behaviour.

Additional Pause method specific to this datapath.

The DataPath implements pause by recording the current position and stopping the source and sink.
When Play is called the DataPath uses the stored position as the starting point and resumes
playing. The reason for this (rather than using the PauseL() API on sources and sinks) is that
some implementations do not support a suitable pause, therefore the DataPath is implemented to
support the lowest common denominator.

Note:
A suitable pause implementation will retain any buffers in use. There will be no
need to call PrimeL() prior to PlayL().
*/
void CMMFDataPath::DoPauseL()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::DoPauseL tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif


	if ((iDataPathCreated) && ((iState == EPlaying) || (iState == EConverting)))
		{
		// try to pause source and sink
		iDataSource->SourcePauseL();		// propagate state change to source
		SetPositionL(Position());
		iDataSink->SinkStopL();

		iPauseCalled = ETrue;				// indicate pause is called
		
		iState = EPrimed;					// go back to primed state (we're not playing)

		iSinkBufferWithSink = EFalse;
		iSourceBufferWithSource = EFalse;

		ResetRefBuffers();					// buffer references may not be valid any more

		Cancel(); //Stop the state machine
		}
	else if(iState == ERecording)
		{
		iPauseCalled = ETrue;
#ifdef _DP_DEBUG
		RDebug::Print(_L("DP::DoPauseL Recording, pause datasource\n"));
#endif
		iDataSource->SourcePauseL();
		}
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::DoPauseL - Done iReferenceAudioSamplesPlayed = %d\n"),iReferenceAudioSamplesPlayed);
	RDebug::Print(_L("DP::DoPauseL - Done restart at %d tick-%d   (this 0x%x)\n"),I64INT(iStartPosition.Int64()), User::TickCount(),this);
#endif
	}

/**
This is a virtual function datapath (or derivations off) that can be implemented or may be left blank for default behaviour.

Additional Pause method specific to this datapath.
*/
void CMMFDataPath::DoEndOfDataL()
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::DoEndOfDataL tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	SetPositionL(iPlayWindowStartPosition);	// reset position

	ASSERT(iCompleteCallback);
	iCompleteCallback->SignalDataPathComplete(iDataPathCompletedErrorCode);

	ResetRefBuffers();
	iState = EStopped;

#ifdef _DP_DEBUG
	RDebug::Print(_L("DP::DoEndOfDataL - Done  tick-%d   (this 0x%x)\n"),User::TickCount(),this);
#endif
	}


/**
Passes error handling and general messages up to clients.

@param  aEventType
        Category code for the event. Category codes can be used as unique identifers.
@param  aErrorCode
        Standard error code.

@return The event code sent to client.
*/
//error handling
EXPORT_C TInt CMMFDataPath::DoSendEventToClient(TUid aEventType, TInt aErrorCode)
	{
	TMMFEvent event(aEventType, aErrorCode);
	return iEventHandler.SendEventToClient(event);
	}

/**
Passes error handling and general messages to clients.

@param  aEvent
        TMMFEvent supplied by callee (typically DoSendEventToClient)

@return The Event Message sent to the datapath event handler
*/
EXPORT_C TInt CMMFDataPath::SendEventToClient(const TMMFEvent& aEvent)
	{
#ifdef _DP_DEBUG
	RDebug::Print(_L("CMMFDataPath::SendEventToClient CODE = %d  ticks=%d   (this 0x%x)\n"),aEvent.iErrorCode,User::TickCount(),this);
	RDebug::Print(_L("CMMFDataPath::SendEventToClient iEventType = %d  == %d\n"),aEvent.iEventType, KMMFEventCategoryPlaybackComplete);
#endif

		//If we have sent all the data to the sink, it is legal for it to send KErrUnderFlow
		//to us and we can go through a clean shutdown, otherwise we pass the error to the 
		//controller and it is responsible for determining what to do
		if(iAllDataSentToSink &&
			(aEvent.iEventType == KMMFEventCategoryPlaybackComplete) &&
			(aEvent.iErrorCode == KErrUnderflow))
			{
#ifdef _DP_DEBUG
			RDebug::Print(_L("CMMFDataPath::SendEventToClient Clean complete\n"));
#endif
			//sink may not return the final buffer once it has finished with it
			//force ourselves into the EndOfData state
			if(iTransferState != EEndOfData)
				{
				iDataPathCompletedErrorCode = KErrNone;
				TRAP_IGNORE(DoEndOfDataL());
				}

			iCompleteCallback->SignalSinkComplete(KErrNone);
			return KErrNone;
			}



	return iEventHandler.SendEventToClient(aEvent);	
	}

CMMFDataPath::CCompleteCallback::CCompleteCallback(CMMFDataPath& aDataPath, TBool aWaitForSink)
	: CActive(EPriorityStandard), 
	iDataPath(aDataPath),
	iWaitForSink(aWaitForSink)
	{
	CActiveScheduler::Add(this);
	}

CMMFDataPath::CCompleteCallback::~CCompleteCallback()
	{
	Cancel();
	}

void CMMFDataPath::CCompleteCallback::SignalDataPathComplete(TInt aDataPathError)
	{
	iDataPathComplete = ETrue;
	iDataPathError = aDataPathError;
	if (!IsActive()) 
		{
		// Signal ourselves to run with the given completion code
		TRequestStatus* status = &ActiveStatus();
		User::RequestComplete(status, KErrNone);
		}
	}

void CMMFDataPath::CCompleteCallback::SignalSinkComplete(TInt aSinkError)
	{
	iSinkComplete = ETrue;
	iSinkError = aSinkError;
	if (!IsActive()) 
		{
		// Signal ourselves to run with the given completion code
		TRequestStatus* status = &ActiveStatus();
		User::RequestComplete(status, KErrNone);
		}
	}


TRequestStatus& CMMFDataPath::CCompleteCallback::ActiveStatus()
	{
	SetActive();
	return iStatus;
	}

void CMMFDataPath::CCompleteCallback::DoCancel()
	{
	}

void CMMFDataPath::CCompleteCallback::RunL()
	{			
	if (iWaitForSink)
		{
		if (iDataPathComplete && iSinkComplete)
			{
#ifdef _DP_DEBUG
			RDebug::Print(_L("CMMFDataPath::CCompleteCallback::RunL STOPPING src & sink ticks=%d   (this 0x%x)\n"),User::TickCount(),this);
#endif

			TRAP_IGNORE(iDataPath.DoStopL())

			iDataPathComplete = EFalse;
			iSinkComplete = EFalse;

			// if we have to wait for the sink to complete, always use the sink error
			iDataPath.DoSendEventToClient(KMMFEventCategoryPlaybackComplete, iSinkError);
			}
		}
	else if (iDataPathComplete)
		{
#ifdef _DP_DEBUG
		RDebug::Print(_L("CMMFDataPath::CCompleteCallback::RunL STOPPING src & sink ticks=%d   (this 0x%x)\n"),User::TickCount(),this);
#endif

		TRAP_IGNORE(iDataPath.DoStopL())

		iDataPathComplete = EFalse;
		iSinkComplete = EFalse;

		iDataPath.DoSendEventToClient(KMMFEventCategoryPlaybackComplete, iDataPathError);
		}
	}
	
EXPORT_C TInt CMMFDataPath::SetBlockLength(TUint aBlockLength)
	{
	MMMFDevSoundCustomInterfaceFileBlockLength* fileBlockLengthCI = NULL;
	TInt err = KErrNotSupported;
	if (iCodec)
		{
		err = iCodec->ExtensionInterface(KUidCustomInterfaceDevSoundFileBlockLength.iUid, (TAny*&)fileBlockLengthCI); 
		}

	if (err == KErrNone)
		{
		fileBlockLengthCI->SetFileBlockLength(aBlockLength);
		}

	return err;
	}

