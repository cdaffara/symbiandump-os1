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


#ifndef OGGDECODE_H
#define OGGDECODE_H

#include <mmf/server/mmffile.h>
#include <stdlib.h>
#include "OggUtil.h"
#include "VorbisInfo.h"

static const TInt KNumHeaders = 3;// vorbis has 3 header packets
//Average oggpage is of size 4K. we read 4K buffer during seeking and finding duration sothat we expect
//the page header in it.
static const TInt KReadBufferSize = 0x1000; // 4K

/**
@internalTechnology

Interface to be implemented by clients of the class COggDecode. In specific, COggPlayController needs to implement
this interface.MOggDecodeObserver provides interface for the classes that are interested in receiving BufferFilled event 
from COggDecode.
*/
class MOggDecodeObserver
	{
public:
	/**
	Function called by COggDecode to indicate that the buffer passed to it is filled.

	This is a pure virtual function that each derived class must implement.
	This method is used as the callback when the observer(controller in specific) requests COggDecode
	to fill the buffer through FillThisBufferL() call. When the cnotroller gets this callback it knows 
	that the buffer has been filled and can be sent to devsound.
	*/
	virtual void BufferFilled() = 0;
	};

/**
@internalTechnology

COggDecode class is mainly responsible for reading the encoded data from the clip and framing an oggpacket from it.
Reads header to retrieve the current audio configuration from the clip. This is useful when the client queries 
the current samplerate, bitrate etc.Also limited seeking is provided by this class.
*/
class COggDecode : public CBase, 
				   public MDataSink
{
public:
    static COggDecode* NewL(CMMFClip& aClip, MOggDecodeObserver& aObserver);
    virtual ~COggDecode();
    
    void InitializeVorbisL();
    void FillThisBufferL(CMMFBuffer* aBuffer);
	void GetPacketSyncL();
    void ResetL();
    void ResetFile();
    TBool IsAllDataSent();
    CVorbisInfo* Info();
    void SetPlayWindowL(TTimeIntervalMicroSeconds aStart, TTimeIntervalMicroSeconds aEnd);
    void ClearPlayWindowL();
	
	//from MDataSink
	TFourCC SinkDataTypeCode(TMediaId aMediaId);
	void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId);
	void BufferFilledL(CMMFBuffer* aBuffer);
	TBool CanCreateSinkBuffer();
	CMMFBuffer* CreateSinkBufferL(TMediaId aMediaId, TBool& aReference);
	void ConstructSinkL( const TDesC8& aInitData);
	
	//Repositioning methods
	TInt64 ReadHeader(CMMFPtrBuffer* aSourceBuffer, TInt& aPos);
	TTimeIntervalMicroSeconds DurationL();
	void SetPositionL(TTimeIntervalMicroSeconds aPosition);
	TInt GetStreamPositionL(TInt aSamples, TInt64& aGranulePos);
	TTimeIntervalMicroSeconds SeekingTime();
	void SetPositionL(TInt64 aGranulePos);
	void SetLastPageFlag(CMMFPtrBuffer* aSourceBuffer);
private:
    COggDecode(CMMFClip& aClip, MOggDecodeObserver& aObserver);
    void ConstructL();
    
private:
    MOggDecodeObserver& iObserver;
    CMMFClip* iClip;
    TBool iSeek;
    TInt iStoredSourcePos;
    TInt iSourcePos;
    //clip position corresponding to playwindowstart
    TInt iPlayWindowStart;
    //clip position corresponding to playwindowend
    TInt iPlayWindowEnd;
    //flag used to avoid calculating duration everytime DurationL() is called
    TBool iDurationCalculated;
    //we need to resend the codec initialization packets during seeking. 
    //This varible maintains number of header packets sent.
    TInt iInitSeek;
    TInt iHeaderSize;
    //Contains the current audio configuration read from the header of the clip
    CVorbisInfo* iInfo;
    //granulePos corresponding to the current file position
    TInt64 iCurrentGranulePos;
    //granulePos corresponding to the duration
    TInt64 iEndGranulePos;
    //set when playwindowend is reached. This flag is checked for stopping the play.
    TBool iPlayWindowEndReached;
    //set when all data sent to devsound. This flag is checked for proper playcompletion.
    TBool iAllDataSent;
    TTimeIntervalMicroSeconds iDuration;
    CMMFPtrBuffer* iSourceBuffer;
    CMMFDataBuffer* iBufferFromSink;
    COggPager* iPager;
    COggStream* iStream;
    TOggPacket iPacket;
    TOggPage iPage;
    /*data in an oggpacket is not always in one buffer may be spread over several using linked lists.
     so data_store is used to keep the extracted data while traversing the linkedlist.
     */
    unsigned char *iDataStore;
    TTimeIntervalMicroSeconds iPlayStartTime;
    TTimeIntervalMicroSeconds iPlayEndTime;
    TInt64 iPlayStartGranulePos;
    TInt64 iPlayEndGranulePos;
#ifdef SYMBIAN_SEP_HEAP    
    RHeap* iDefaultHeap;
    RHeap* iVorbisHeap;
    RChunk iVorbisChunk;
#endif
    };
#endif // OGGDECODE_H
