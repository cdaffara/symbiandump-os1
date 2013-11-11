// fzipup.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __FZIP_UP_H__
#define __FZIP_UP_H__

#include <zipfile.h>
#include <ezbufman.h>
#include <fshell/ioutils.h>

using namespace IoUtils;

const TUint32 KDefaultZipBufferLength = 0x8000; // 32 Kbytes

//
// class CBufferManager
// manage the i/o buffers for compression, wrapper over the MEZBufferManager mix-in
// takes input data from one file stream & writes the compressed output to another file stream
//
class CBufferManager : public CBase, public MEZBufferManager
	{
public:
	static CBufferManager *NewLC(RFileReadStream& aInput, RFileWriteStream& aOutput, TInt aInputStreamBytes, TInt aBufferSize);
	~CBufferManager();
	inline TInt TotalBytesOut(){ return iBytesWritten; }
	
	// from MEzBufferManager
	void InitializeL(CEZZStream &aZStream);
	void NeedInputL(CEZZStream &aZStream);
	void NeedOutputL(CEZZStream &aZStream);
	void FinalizeL(CEZZStream &aZStream);
private:
	CBufferManager(RFileReadStream& aInput, RFileWriteStream& aOutput, TInt aInputStreamBytes, TInt aBufferSize);
	void ConstructL();
	void ReadInputL();
	void WriteOutputL(CEZZStream& aZStream);
private:
	RFileReadStream& iInput;
	RFileWriteStream& iOutput;
	TInt iInputStreamBytes; // size of the input stream in bytes
	TInt iReadLength;
	TInt iBufferSize;
	TInt iBytesWritten;
	TUint8* iInputBuffer;
	TUint8* iOutputBuffer;
	TPtr8 iInputDescriptor;
	TPtr8 iOutputDescriptor;
	};

//
// class CZipEntry
// defines attributes associated with a file contained within a zip archive
// note these are taken from PKWARE'S APPNOTE.TXT zip-file format specification
//
class CZipItUp;
class CZipEntry : public CZipArchive
{
public:
	friend class CZipItUp;
	static CZipEntry* NewLC(RFs& aFs, const TDesC& aFilename, const TInt aUid);
        virtual ~CZipEntry();

	void SetOffset(TUint32 aOffset);
	TUint32 ReturnOffset();
	TUint32 FileHeaderSize();
private:
	CZipEntry(RFs& aFs, const TInt aUid);
	void ConstructL(const TDesC& aFilename);
	void CalcCRCL();
private:
        static const TInt iOffset;
        TLocalHeader iLFH;					// Local File Header
        HBufC8* iFileData;					// File Data
        TCentralDirectoryHeader iFH;		// File Header
        RFs& iFs;							// f32 hook
        TBuf8<KMaxFileName>	iAsciiName;	// Ascii file name
        RFile iInput;						// input data from file
        const TInt iUid;					// uid id'ing this particular zip entry
        TSglQueLink iLink;
	};
	
//
// class CZipItUp
// create a zip archive
//
typedef TSglQue<CZipEntry> CZipMemberLinkedList;
typedef TSglQueIter<CZipEntry> CZipMemberLinkedListIter;
	
class CZipItUp : public CBase
	{
public:
	static CZipItUp* NewLC(RFs& aFs, TDesC& aArchive);
	virtual ~CZipItUp();
	void AddFileL(const TDesC& aFile);
	void CreateZipL();
private:
	CZipItUp(RFs& aFs, TDesC& aArchive);
	void ConstructL();
	void AddEntryL(CZipEntry& aEntry);
	TBool DuplicateEntryL(const TDesC& aFile);
	void AppendLocalFileHeaderL(RFileWriteStream& aStream, CZipEntry& aZipEntry);
	void AppendCompressedDataL(RFileWriteStream& aStream, CZipEntry& aZipEntry);
	void AppendCentralDirectoryFileHeaderL(RFileWriteStream& aStream, CZipEntry& aEntry);
	void AppendCentralDirectoryTrailerL(RFileWriteStream& aStream);
private:
	RFs& iFs;
	TDesC& iFileName;
	RFile iFile;
	CZipMemberLinkedList iZipMemberLinkedList;
	CZipMemberLinkedListIter iZipMemberLinkedListIter;
	TInt iEntryUid;

  struct TCdt 
		{
        TUint32 iSignature;
		TUint16 iDiskNumber;
		TUint16 iStartDiskNumber;
		TUint16 iLocalEntryCount;
		TUint16 iTotalEntryCount;
		TUint32 iSize;
		TUint32 iOffset;
		TUint16 iCommentLength;
		};
	TCdt iCDT;		// Central Directory Trailer
	};

#endif // __FZIP_UP_H__
