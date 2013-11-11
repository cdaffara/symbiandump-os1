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


#ifndef VORBISINFO_H
#define VORBISINFO_H

class CVorbisComment;  //declared here
/**
@internalTechnology

Symbian C++ wrapper for the vorbis_comment structure in ogglibraries.
*/
NONSHARABLE_CLASS(CVorbisComment) : public CBase
    {
public:
    // construct from a name and value
    static CVorbisComment* NewL(TDesC& aName, TDesC& aValue);
    // construct from 8-bit string of the form NAME=value
    static CVorbisComment* NewL(char* aComment, int aLength);

    ~CVorbisComment();
    
    IMPORT_C TDesC& Name();
    IMPORT_C TDesC& Value();
private:
    CVorbisComment();
    void ConstructL(TDesC& aName, TDesC& aValue);
    void ConstructL(char* aComment, int aLength);
private:
    HBufC* iName;
    HBufC* iValue;
    };

class CVorbisInfo;
/**
@internalTechnology

Symbian C++ wrapper for the vorbis_info structure in ogglibraries.
*/
NONSHARABLE_CLASS(CVorbisInfo) : public CBase
    {
public:
    IMPORT_C static CVorbisInfo* NewL();
    
    // initialise the info from the first 3 packets in an ogg vorbis file - used during playing
    IMPORT_C void InitializeL(const TOggPacket& aPckt1, 
                              const TOggPacket& aPckt2, 
                              const TOggPacket& aPckt3);
    
    // initialise the vorbis info from three packets, one at a time - used during playing
    IMPORT_C void InitializeL(const TOggPacket& aNextPacket);
    //initialize vorbis info with the samplerate, bitrate and number of channels - used during recording
    IMPORT_C void InitializeL(TUint aSampleRate, TUint aBitRate, TUint aNumChannels);
    IMPORT_C TInt SampleRate();
    IMPORT_C TInt Channels();
    
    IMPORT_C TDesC& Vendor();
    IMPORT_C TInt Comments();
    IMPORT_C CVorbisComment& GetComment(TInt aIndex);
    
    IMPORT_C TInt BitRateNominal();
    IMPORT_C TInt BitRateUpper();
    IMPORT_C TInt BitRateLower();
	IMPORT_C void SetSampleRate(TUint aSampleRate);
	IMPORT_C void SetBitRate(TUint aBitRate);
	IMPORT_C void SetChannels(TUint aChannels);
    ~CVorbisInfo();
private:
    CVorbisInfo();
    void ConstructL();
    
private:
    class CBody;
    CBody* iBody;
    };
    
#endif
