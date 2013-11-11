// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef READERS_H
#define READERS_H

#include "constants.h"
#include "mmruf.h"


static const TInt KAACFrameHeaderSize = 7;
static const TInt KMP3FrameHeaderSize = 4;
static const TInt KBufSize = 64;


//
// This class provides a level of abstraction that allows
// for parsing data. It is necessary because AppArc may
// only provide a buffer that contains a limited snapshot
// of the beginning of a file, or a file handle that allows
// the whole file to be parsed.
//
class CReader : public CBase
	{
public:

	//
	// Enums used so that CParsers can determine
	// the type of reader they've been given.
	//
	typedef enum
		{
		EUnknown = 0,
		EBuffer = 1,
		EFile = 2
		}
	TReaderType;
	
	//
	// This function resets the reader such that the next
	// read operation will be performed from the start of
	// the source.
	// For a CFileReader the buffer is filled from the start
	// of the file, for a CBufferReader the position in the 
	// buffer is set to zero.
	//
	virtual void Reset() { iBufPos = 0; }
	
	//
	// Seeks within the source.
	// This function tries to support 64-bit compatible file
	// formats as much as possible, but there may be some
	// instances when the operation cannot be performed due to
	// limitations of the file server.
	//
	virtual void SeekL(TInt64 aOffset) = 0;
	
	//
	// Seeks within the source.
	//
	virtual void SeekL(TInt aOffset) = 0;
	
	//
	// Returns the current position in the buffer.
	//
	virtual TInt Position() { return iBufPos; }
		
	//
	// Data reading routines.
	//
	TBool Match(const TDesC8& aPattern);
	void Read64L(TInt64& aData, TBool aLittleEndian = EFalse);
	void Read32L(TUint32& aData, TBool aLittleEndian = EFalse);
	void Read16L(TUint16& aData, TBool aLittleEndian = EFalse);
	void ReadByteL(TUint8& aData);
	void ReadBytesL(TDes8& aData);

	//
	// Gets the type of Reader being used.
	//
	inline TReaderType Type() { return iType; }
		
protected:
	CReader(const TDesC8& aBuffer, CReader::TReaderType aType);
	
	//
	// Returns ETrue if there is aAmount of unread bytes available,
	// EFalse otherwise.
	//
	virtual TBool CheckEnoughData(TInt aAmount);
	
	//
	// Non-leaving Seek
	//
	TInt Seek(TInt aOffset);
	TInt Seek(TInt64 aOffset);
	
private:
	const TDesC8& iBuffer;		// The buffer that contains the source data.
	TInt iBufPos;				// The current position in the data source.
	CReader::TReaderType iType;	// The type of reader it is.
	};


//
// This class allows reading and seeking operations to be
// performed on a data buffer. This is used when no file
// handle is available to the recogniser.
//
class CBufferReader : public CReader
	{
public:
	static CBufferReader* NewLC(const TDesC8& aBuffer);
	virtual ~CBufferReader();
	void Reset();
	void SeekL(TInt aOffset);				// CReader
	void SeekL(TInt64 aOffset);				// CReader

protected:
	CBufferReader(const TDesC8& aBuffer);
	CBufferReader(const TDesC8& aBuffer, TReaderType aType);
	void ConstructL();
	};


//
// This class allows reading and seeking operations to be
// performed on a file handle. This is used when a file
// handle is available to the recogniser.
//
class CFileReader : public CBufferReader
	{
public:
	static CFileReader* NewLC(RFile* aFile);
	virtual ~CFileReader();
	void Reset();
	
	//
	// CReader::Position() override.
	//
	TInt Position() { return CBufferReader::Position() + iFilePos; }
		
	void SeekL(TInt aOffset);			// CReader
	void SeekL(TInt64 aOffset);			// CReader
	
protected:
	CFileReader(RFile* aFile);
	void ConstructL();
	TBool CheckEnoughData(TInt aAmount);	// CReader
	
	//
	// Seeks to a new file location and fills the buffer from there.
	//
	TInt PhysicallySeekAndRead(TInt aAmount);
	TInt PhysicallySeekAndRead(TInt64 aOffset);
private:
	RFile* iFile;
	TInt64 iFilePos;
	TBuf8<KBufSize> iFileBuffer;
	};
	
	
#endif

