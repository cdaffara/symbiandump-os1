// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef OGGENCODE_H
#define OGGENCODE_H

#include <mmf/server/mmfclip.h>
#include "OggUtil.h"
#include "VorbisInfo.h"

#ifdef VORBIS_DEBUG
#define DEBUG(x) RDebug::Print(_L(x))
#define DEBUG1(x,x1) RDebug::Print(_L(x),x1)
#else
#define DEBUG(x)
#define DEBUG1(x,x1)
#endif

//this defines the valid sample rates and bitrates for vorbis taken from http://xiph.org/vorbis/
const TUint KOggVorbisSampleRates[] = { 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};
const TUint KOggVorbisBitRates[] = { 64000, 80000, 96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 500000};
const TUint KMono = 1;
const TUint KStereo = 2;

const TUint KOggVorbisDefaultSampleRate = 8000;
const TUint KOggVorbisDefaultBitRate = 64000;
const TUint KMMFFormatDefaultChannels = KStereo;

const TInt KMaxNumSampleRates = 9;
const TInt KMaxNumBitRates = 11;

//oggvorbis has three header packets
const TInt KMaxNumHeaders = 3;

//Average oggpage is of size 4K. we read 4K buffer during seeking and finding duration sothat we expect
//the page header in it.
static const TInt KReadBufferSize = 0x1000; // 4K

/**
@internalTechnology

Interface to be implemented by clients of the class COggEncode. In specific, COggRecordController needs to implement
this interface. MOggEncodeObserver provides interface for the classes that are interested in receiving BufferEmptied event 
from COggEncode.
*/
class MOggEncodeObserver
	{
public:
	/**
	Function called by COggDecode to indicate that the buffer passed to it is emptied.

	This is a pure virtual function that each derived class must implement.
	This method is used as the callback when the observer(controller in specific) requests COggEncode
	to empty the buffer through EmptyThisBufferL() call. When the cnotroller gets this callback it knows 
	that the buffer has been emptied and can be reused.
	*/
	virtual void BufferEmptied() = 0;
	};
	
/**
@internalTechnology

COggEncode class is mainly responsible for writing the encoded data into the clip. Reads header to 
retrieve the current audio configuration from the clip. This is useful when the client queries the 
current samplerate, bitrate etc.Also limited seeking is provided by this class.
*/

class COggEncode :  public CBase,
					public MDataSource
    {
public:
    static COggEncode* NewL(MOggEncodeObserver& aObserver, CMMFClip& aClip);
    virtual ~COggEncode();
    void EmptyThisBufferL(CMMFBuffer* aBuffer);
    void InitializeVorbisL();
    void GetPacketSyncL();
	void Reset();
	TBool IsAllDataWritten();
	TInt SetNumChannels(TUint aChannels);
	TInt SetSampleRate(TUint aSampleRate);
	TInt SetBitRate(TUint aSampleRate);
	TUint NumChannels();
	TUint SampleRate();
	TUint BitRate();
	void GetSupportedSampleRatesL(RArray<TUint>& aSampleRates);
	void GetSupportedNumChannelsL(RArray<TUint>& aNumChannels);
	void GetSupportedBitRatesL(RArray<TUint>& aNumChannels);
	CVorbisInfo* Info();
	void GetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime);
	void SetMaximumClipSizeL(TInt aFileSize);
	TInt64 ReadHeader(CMMFPtrBuffer* aSourceBuffer, TInt& aPos);
	TTimeIntervalMicroSeconds DurationL();
	void SetPositionL(TTimeIntervalMicroSeconds aPosition);
	TTimeIntervalMicroSeconds PositionL();
	TInt GetStreamPositionL(TInt64 aSamples, TInt64& aGranulePos);
		
	//from MDataSource
	virtual void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId);
	virtual void BufferEmptiedL(CMMFBuffer* aBuffer);
	virtual TBool CanCreateSourceBuffer();
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, TBool &aReference);
	virtual TFourCC SourceDataTypeCode(TMediaId aMediaId);
	virtual void ConstructSourceL(const TDesC8& aInitData);
		
private:
    COggEncode(MOggEncodeObserver& aObserver, CMMFClip& aClip);
    void ConstructL();
private:
    MOggEncodeObserver& iObserver;
    CMMFClip* iClip;
    TBool iClipAlreadyExists;
    TUint iMaxClipSize;
    TInt iCurrentPosition;
    TInt64 iCurrentGranulePos;
    TInt64 iEndGranulePos;
    CMMFPtrBuffer* iSinkBuffer;
    CMMFDataBuffer* iBufferFromSource;
    COggStream* iStream;
    COggPager* iPager;
    TOggPacket iPacket;
    TOggPage iPage;
    CVorbisInfo* iInfo;
    TInt iHeaderSize;
	TTimeIntervalMicroSeconds iDuration;
	/**	
	Flag to indicate that the duration is already calculated.
	
	Set to ETrue when the duration is calculated for teh first time. 
	Set to EFalse when the clip is cropped sothat duration is recalculated.
    */
	TBool iDurationCalculated;
	TInt iPageCount;
	/**	
	Flag to indicate that the last buffer is already written into the clip.
	This is used to check the proper record completion.
    */
	TBool iAllDataWritten;
#ifdef SYMBIAN_SEP_HEAP
    RHeap* iDefaultHeap;
    RHeap* iVorbisHeap;
    RChunk iVorbisChunk;
#endif
	};

#endif // OGGENCODE_H
