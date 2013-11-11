// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

class CWavDecodeUtility: public CBase
	{
public:
	static CWavDecodeUtility* NewL(TDesC8& aBuffer);
	~CWavDecodeUtility();

	inline TUint GetChannels(void) const  {return iChannels;};
	inline TUint GetSampleRate(void) const {return iSampleRate;};
	inline TUint GetBitsPerSample(void) const {return iBitsPerSample;};
	inline TUint GetDataLength(void) const {return iDataLength;};

	inline TUint GetSamples(void) const {return iDataLength / (iChannels * iBitsPerSample / 8);};
	

private:
	CWavDecodeUtility();
	void ConstructL(TDesC8& aBuffer);
	TUint16 Read16(const TUint8* aPtr);
	TUint32 Read32(const TUint8* aPtr);
	void FindRiffChunksL(void);
	void ProcessFormatHeaderL();
	void ReadChunk(TMdaRiffChunk* aChunk);
	void AssignChunkTo(TMdaRiffChunk* aAssignedChunk);
	
private:
//	CMMFDataBuffer* iBuffer;
	TDesC8* iBuffer;
	const TUint8* iStartPtr;
	TUint iLastReadPosition;
	TBool iHasFactChunk;
	TMdaRiffChunk iCurrent;
	TMdaRiffChunk iFormatChunk;
	TMdaRiffChunk iFactChunk;
	TMdaRiffChunk iDataChunk;
	TUint iRiffChunkLength;
	TBool iFoundChunks;
	TBool iDone;
	TUint iStartPosition;

	TUint iCodecId;
	TUint iChannels;
	TUint iSampleRate;
	TUint iBlockAlign;				//needed for correct IMA
	TUint iBitsPerSample;

	TUint iDataLength;
	TUint iAverageBytesPerSecond;	//needed for correct IMA
	TUint iSamplesPerBlock;
	
	TUint iPos;
	TUint iClipLength;

	};

 
