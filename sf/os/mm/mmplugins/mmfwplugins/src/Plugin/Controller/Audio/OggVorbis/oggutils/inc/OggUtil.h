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


#ifndef OGGUTIL_H
#define OGGUTIL_H

// OggVorbis C libraries are not alloc safe. So we create a separate heap for C library calls with enough space well before 
// the codec starts processing and we switch to the new heap before making calls to the methods in these libraries. 
// Chunk and Heap sizes are taken such a way that we have enough space on the new heap to avoid OOM situations to the 
// maximum extent possible. Also the malloc, calloc and realloc functions in the C libraries are modified to leave with KErrMemory 
// when the allocation fails. We TRAP the error and cleanup the new heap we had created. This way the library calls are made alloc safe.
// SYMBIAN_SEP_HEAP is provided to enable/disable this separate heap mechanism.
#ifdef SYMBIAN_SEP_HEAP
	#define VORBIS_TRAP(err, c) { User::SwitchHeap(iVorbisHeap); TRAP(err, c); User::SwitchHeap(iDefaultHeap); }
	#define VORBIS_TRAPD(err, c) TInt err = KErrNone; VORBIS_TRAP(err, c);
	const TInt KInitialChunkSize = 0x80000;
	const TInt KMaxChunkSize = 0x800000;
	const TInt KMinHeapLength = 0x40000;
#else
	#define VORBIS_TRAP(err, c) c;
	#define VORBIS_TRAPD(err, c) TInt err = KErrNone; c;
#endif

class TOggPage;  //declared here

/**
@internalTechnology

Symbian C++ wrapper for the oggpage structure in ogglibraries.
*/
NONSHARABLE_CLASS(TOggPage)
    {
public:
    IMPORT_C TOggPage(const TPtr8& aHead, const TPtr8& aBody);
    IMPORT_C TOggPage();
    IMPORT_C TInt Version() const;
    IMPORT_C TBool Continued() const;
    IMPORT_C TInt Packets() const;
    IMPORT_C TInt BOS() const;
    IMPORT_C TInt EOS() const;
    IMPORT_C TInt GranulePos() const;
    IMPORT_C TInt SerialNo() const;
    IMPORT_C TInt PageNo() const;
public:
    TPtr8 iHead;
    TPtr8 iBody;
    };

class TOggPacket;  //declared here
/**
@internalTechnology

Symbian C++ wrapper for the oggpacket structure in ogglibraries.
*/
NONSHARABLE_CLASS(TOggPacket)
    {
public:
    IMPORT_C TOggPacket();
    IMPORT_C TOggPacket(const TPtrC8& aPtr, TInt64 aGranulePos, TInt64 aPacketNo);
#ifndef SYMBIAN_CODEC_FLOAT_POINT 
    IMPORT_C void GetOggData(TPtr8& aDataStore);
#endif
public:    
    TPtr8 iData;
    TBool iBOS; // beginning of stream
    TBool iEOS; // end of stream
    TInt64 iGranulePos; // last logical item that can be decoded from this stream
                        // e.g. sample of PCM or frame number
    TInt64 iPacketNo;   // number of packet in sequence (use to detect holes in data)
    };

class COggPager;   //declared here
/**
@internalTechnology

Symbian C++ wrapper for the ogg_sync_state structure in ogglibraries. This class is responsible
for retrieving the oggpages from the data in the clip.
*/
NONSHARABLE_CLASS(COggPager) : public CBase
    {
public:
    IMPORT_C static COggPager* NewL();
    virtual ~COggPager();
    //resets ogg_sync_state
    IMPORT_C void Reset(); 
    IMPORT_C TInt GetBuffer(TPtr8& aBuf, TInt aSize);
    IMPORT_C TInt DataWritten(TDes8& aBuf);
    IMPORT_C TInt NextPage(TOggPage& aPage);
private:
    COggPager();
    void ConstructL();
private:
    class CBody;
    CBody* iBody;
    };

class COggStream;  //declared here
/**
@internalTechnology

Symbian C++ wrapper for the ogg_stream_state structure in ogglibraries. This class maintains the state of
the oggvorbis stream during the data transfer.
*/
NONSHARABLE_CLASS(COggStream) : public CBase
    {
public:
    IMPORT_C static COggStream* NewL();
    virtual ~COggStream();
    IMPORT_C TInt PageIn(TOggPage& aPage);
    IMPORT_C TInt PacketOut(TOggPacket& aDst);
    IMPORT_C void Reset();
    IMPORT_C TInt GetSerialNo();
private:
    COggStream();
    void ConstructL();
private:
    class CBody;
    CBody* iBody;
    };

#endif
