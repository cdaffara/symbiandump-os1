// file_reader.h
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __FILE_READER_H__
#define __FILE_READER_H__

#include <f32file.h>


class MFileReaderObserver
	{
public:
	enum TReadType
		{
		EFirst,
		EMiddle,
		ELast
		};
public:
	virtual void HandleFileData(const TDesC8& aData, TReadType aType, TBool& aContinue) = 0;
	virtual void HandleFileReadError(TInt aError) = 0;
	};


class CFileReader : public CActive
	{
public:
	static CFileReader* NewL(TInt aBlockSize, RFs& aFs, TBool aDirect);
	~CFileReader();
	void Read(const TDesC& aFileName, MFileReaderObserver& aObserver);
	void Read(const TDesC& aFileName, TInt aOffset, MFileReaderObserver& aObserver);
private:
	CFileReader(RFs& aFs, TBool aDirect, TInt aBlockSize);
	void ConstructL();
	void ReadNextChunk();
	void ReadNextDirectChunk();
private:	// From CActive.
	virtual void DoCancel();
	virtual void RunL();
private:
	RFs& iFs;
	MFileReaderObserver* iObserver;
	TFileName iFileName;
	TInt iBlockSize;
	HBufC8* iBuf;
	TPtr8 iPtr;
	RFile iFile;
	TBool iFirstRead;
	TBool iDirect;
	TInt iDirectOffset;
	};


#endif // __FILE_READER_H__
