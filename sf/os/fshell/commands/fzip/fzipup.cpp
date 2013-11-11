// fzipup.cpp
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

#include <ezstream.h>
#include <ezcompressor.h>
#include <ezdecompressor.h>
#include <ezfilebuffer.h>
#include <f32file.h>
#include "fzipup.h"

const TInt KDefaultDiskNumber = 0;
const TInt CZipEntry::iOffset = _FOFF(CZipEntry, iLink);
const TUint16 KFZipVersion = 0x0014;
TInt KWindowBits = -CEZCompressor::EMaxWBits; // negative value is an undocumented feature of our zlib implementation, ensuring we do not include the zlib header in the compressed data (which we must do for zip-file format support)
const TInt KCompressedSizeRelativePosition = 18; // iCompressedSize is 18 bytes in from the current entry's LFH

//
// CBufferManager
//
CBufferManager *CBufferManager::NewLC(RFileReadStream& aInput, RFileWriteStream& aOutput, TInt aInputStreamBytes, TInt aBufferSize)
	{
	CBufferManager *bm = new (ELeave) CBufferManager(aInput, aOutput, aInputStreamBytes, aBufferSize);
	CleanupStack::PushL(bm);
	bm->ConstructL();
	return bm;
	}

CBufferManager::CBufferManager(RFileReadStream& aInput, RFileWriteStream& aOutput, TInt aInputStreamBytes, TInt aBufferSize)
:iInput(aInput), iOutput(aOutput), iInputStreamBytes(aInputStreamBytes), iBufferSize(aBufferSize), iInputDescriptor(NULL,0), iOutputDescriptor(NULL,0)
	{
	}

CBufferManager::~CBufferManager()
	{
	delete[] iInputBuffer;
	delete[] iOutputBuffer;
	}

void CBufferManager::ConstructL()
	{
	if (iInputStreamBytes < iBufferSize)
		iBufferSize = iInputStreamBytes;
	if (iBufferSize <=0)
		{
		User::Leave(KErrUnderflow);
		}
	
	iInputBuffer = new (ELeave) TUint8[iBufferSize];
	iOutputBuffer = new (ELeave) TUint8[iBufferSize];
	
	iInputDescriptor.Set(iInputBuffer,0,iBufferSize);
	iOutputDescriptor.Set(iOutputBuffer,0,iBufferSize);
	
	iReadLength = iBufferSize;
	}
	
void CBufferManager::InitializeL(CEZZStream &aZStream)
	{
	ReadInputL();
	aZStream.SetInput(iInputDescriptor);
	aZStream.SetOutput(iOutputDescriptor);
	}

void CBufferManager::NeedInputL(CEZZStream &aZStream)
	{
	ReadInputL();
	aZStream.SetInput(iInputDescriptor);
	}

void CBufferManager::NeedOutputL(CEZZStream &aZStream)
	{
	WriteOutputL(aZStream);
	aZStream.SetOutput(iOutputDescriptor);
	}

void CBufferManager::FinalizeL(CEZZStream &aZStream)
	{
	WriteOutputL(aZStream);
	}

void CBufferManager::ReadInputL()
	{
	iInput.ReadL(iInputDescriptor, iReadLength);
	iInputStreamBytes -= iReadLength;
	if (iInputStreamBytes < iBufferSize)
		{
		iReadLength = iInputStreamBytes;
		}
	if (iReadLength < 0)
		{
		User::Leave(KErrOverflow);
		}
	}

void CBufferManager::WriteOutputL(CEZZStream& aZStream)
	{
	TPtrC8 od = aZStream.OutputDescriptor();
	iOutput.WriteL(od);
	iBytesWritten += od.Size();
	}

//
// CZipEntry
// 
CZipEntry* CZipEntry::NewLC(RFs& aFs, const TDesC& aFilename, const TInt aUid)
	{
	CZipEntry* self = new (ELeave) CZipEntry(aFs, aUid);
	CleanupStack::PushL(self);
	self->ConstructL(aFilename);
	return self;
	}

CZipEntry::CZipEntry(RFs& aFs, const TInt aUid):
iFs(aFs), iUid(aUid)
	{
	iLFH.iCRC32 = crc32(iLFH.iCRC32, NULL, 0);
	}

CZipEntry::~CZipEntry()
	{
	if (iFileData)
		delete iFileData;
	if (iInput.SubSessionHandle() > 0)
		iInput.Close();
	}
	
void CZipEntry::ConstructL(const TDesC& aFilename)
	{
	User::LeaveIfError(iInput.Open(iFs, aFilename, EFileRead | EFileShareReadersOnly));

	// ensure aFileName is stored in accordance with zip file-format specification
	TFileName2 file(aFilename);
	if (file.HasDriveLetter())
		{
		file.Delete(0, 3); // remove '<drive>:\'
		}
	if (file.HasLeadingSlash())
		{
		file.Delete(0, 1); // remove '\'
		}
	iAsciiName.Copy(file);
	TUint8* ptr = const_cast<TUint8*> (iAsciiName.Ptr());
	TInt count = 0;
	do
		{
		if (*ptr == '\\')
			{
			*ptr = '/';
			}
		ptr++;
		} while (count++ < iAsciiName.Length());
	
	// config. local file header
	iLFH.iSignature = KLocalHeaderSignature;
	iLFH.iVersionNeeded = KFZipVersion;
	iLFH.iFlags = 0x0002; // best (-exx/-ex) compression was used
	iLFH.iCompressionMethod = EDeflated;
	TTime time;
	User::LeaveIfError(iInput.Modified(time));
	TDateTime td = time.DateTime();
	TUint16 param1 = td.Year() - 1980;
	TUint16 param2 = td.Month() + 1;
	TUint16 param3 = td.Day() + 1;
	iLFH.iLastModifiedFileDate = (param1 << 9) | (param2 << 5) | param3;
	param1 = td.Hour() + 1;
	param2 = td.Minute() + 1;
	param3 = td.Second() + 1;
	iLFH.iLastModifiedFileTime = (param1 << 11) | (param2 << 5) | (param3 >> 1);
	TInt uSize = 0;
	User::LeaveIfError(iInput.Size(uSize));
	if (uSize <= 0)
		{
		User::Leave(KErrAbort);
		}
	iLFH.iUncompressedSize = uSize;
	iLFH.iCompressedSize = uSize; // note: to be adjusted later once the compression is performed
	CalcCRCL();
	iLFH.iFileNameLength = iAsciiName.Size();
	iLFH.iExtraFieldLength = 0;
	
	// config. file header
	iFH.iSignature = KCentralDirectoryHeaderSignature;
	iFH.iMadeBy = KFZipVersion;
	iFH.iRequired = KFZipVersion;
	iFH.iFlags = iLFH.iFlags;
	iFH.iCompressionMethod = iLFH.iCompressionMethod;
	iFH.iLastModifiedFileTime = iLFH.iLastModifiedFileTime;
	iFH.iLastModifiedFileDate = iLFH.iLastModifiedFileDate;
	iFH.iCRC32 = iLFH.iCRC32;
	iFH.iCompressedSize = iLFH.iCompressedSize;	// note: to be adjusted later once the compression is performed
	iFH.iUncompressedSize = iLFH.iUncompressedSize;
	iFH.iFileNameLength = iLFH.iFileNameLength;
	iFH.iExtraFieldLength = iLFH.iExtraFieldLength;
	iFH.iFileCommentLength = 0x0000;
	iFH.iDiskNumberStart = KDefaultDiskNumber;
	iFH.iInternalFileAttributes = 0x0001;
	iFH.iExternalFileAttributes = 0x00000020;	// hardcoded to 'Archive'
	iFH.iLocalHeaderOffset = 0x00000000; // to be adjusted later (if necessary)
	}

void CZipEntry::SetOffset(TUint32 aOffset)
	{
	iFH.iLocalHeaderOffset = aOffset;
	}

TUint32 CZipEntry::ReturnOffset()
	{
	TUint32 result = iFH.iLocalHeaderOffset; // start with the offset of the LFH
	result += KLocalHeaderFixedLength; // add the size of the LFH itself
	result += iAsciiName.Size(); // add the size of the filename
	result += iLFH.iCompressedSize; // add any compressed data
	return result;
	}

TUint32 CZipEntry::FileHeaderSize()
	{
	TUint32 result = KCentralDirectoryHeaderFixedLength;
	result += iAsciiName.Size();
	return result;
	}

//
// CZipEntry::CalcCRCL
// mem. efficient means of calculating the crc on uncompressed data
// we require a maximum of KDefaultZipBufferLength heap space regardless size of data
//
void CZipEntry::CalcCRCL()
	{
	ASSERT(iInput.SubSessionHandle() > 0);
	TInt bytesRead = 0;
	TInt length = KDefaultZipBufferLength; // 32Kb
	if (iLFH.iUncompressedSize < KDefaultZipBufferLength)
		{
		length = iLFH.iUncompressedSize;
		}
	
	iLFH.iCRC32 = crc32(0, NULL, 0);
	HBufC8* crc = HBufC8::NewLC(length);
	TPtr8 crcPtr = crc->Des();
	User::LeaveIfError(iInput.Seek(ESeekStart, bytesRead));
	do
		{
		User::LeaveIfError(iInput.Read(crcPtr, length));
		iLFH.iCRC32 = crc32(iLFH.iCRC32, crcPtr.Ptr(), length);
		bytesRead += length;
		if ((iLFH.iUncompressedSize - bytesRead) < KDefaultZipBufferLength)
			{
			length = iLFH.iUncompressedSize - bytesRead;
			}
		} while (length > 0);
	CleanupStack::PopAndDestroy(1); // crc	
	}

//
// CZipItUp
// 
CZipItUp* CZipItUp::NewLC(RFs& aFs, TDesC& aArchive)
	{
	CZipItUp* self = new (ELeave) CZipItUp(aFs, aArchive);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}		

CZipItUp::CZipItUp(RFs& aFs, TDesC& aArchive):
iFs(aFs), iFileName(aArchive), iZipMemberLinkedList(CZipEntry::iOffset), iZipMemberLinkedListIter(iZipMemberLinkedList)
	{
	}

CZipItUp::~CZipItUp()
	{
	if (iFile.SubSessionHandle() > 0)
		{
		iFile.Close();
		}
	while (!iZipMemberLinkedList.IsEmpty())
		{
		CZipEntry* entry = iZipMemberLinkedList.First();
		iZipMemberLinkedList.Remove(*entry);
		delete entry;	
		}
	iZipMemberLinkedList.Reset();
	}
	
void CZipItUp::ConstructL()
	{
	User::LeaveIfError(iFile.Create(iFs, iFileName, EFileStream | EFileShareExclusive));
	}
	
//
// CZipItUp::AddFileL
//
void CZipItUp::AddFileL(const TDesC& aFile)
	{
	if (!DuplicateEntryL(aFile))
		{
		// spawn a new zip entry, compress the file, add it to the list of zip file contained in the archive
		CZipEntry* zipEntry = CZipEntry::NewLC(iFs, aFile, ++iEntryUid);
		AddEntryL(*zipEntry);
		CleanupStack::Pop(zipEntry);
		}
	}

//
// CZipItUp::DuplicateEntryL
// run a check against the zip archive's current entries (if any!) for duplicate files
// return ETrue if a duplicate is found
//
TBool CZipItUp::DuplicateEntryL(const TDesC& aFile)
	{
	if (iZipMemberLinkedList.IsEmpty())
		{
		return EFalse;
		}
	
	// turn aFile into a zip-file format filename
	TFileName2 file(aFile);
	if (file.HasDriveLetter())
		{
		file.Delete(0, 3); // remove '<drive>:\'
		}
	if (file.HasLeadingSlash())
		{
		file.Delete(0, 1); // remove '\'
		}
	TBuf8<KMaxFileName> ascii;
	ascii.Copy(file);
	TUint8* ptr = const_cast<TUint8*> (ascii.Ptr());
	TInt count = 0;
	do
		{
		if (*ptr == '\\')
			{
			*ptr = '/';
			}
		ptr++;
		} while (count++ < ascii.Length());
	
	// iterate through zip archive entries looking for a duplicate 
	iZipMemberLinkedListIter.SetToFirst();
	CZipEntry* entry = iZipMemberLinkedListIter++;
	while (entry != NULL)
		{
		if (entry->iAsciiName == ascii)
			{
			return ETrue;
			}
		entry = iZipMemberLinkedListIter++;
		}
	return EFalse;
	}

//
// CZipItUp::AddEntryL
// Add an entry to the zip archive.
//
void CZipItUp::AddEntryL(CZipEntry& aEntry)
	{
	// append the new entry to the queue
	if (!iZipMemberLinkedList.IsEmpty())
		{
		// prior entries in the list therefore need to adjust this entry's local file header offset
		CZipEntry* lastEntry = iZipMemberLinkedList.Last();
		User::LeaveIfNull(lastEntry);
		}
	iZipMemberLinkedList.AddLast(aEntry);
	}

//
// CZipItUp::CreateZipL
// Creates the actual file, including all the previously compressed zip files in the archive
//
void CZipItUp::CreateZipL()
	{
	ASSERT(iFile.SubSessionHandle() > 0);
	if (iZipMemberLinkedList.IsEmpty())
		{
		User::Leave(KErrGeneral);
		}
	RFileWriteStream stream(iFile);
	stream.PushL();
	
	// initialise central directory header
	iCDT.iSignature = CZipArchive::KCentralDirectorySignature;
	iCDT.iDiskNumber = KDefaultDiskNumber;
	iCDT.iStartDiskNumber = KDefaultDiskNumber;
	iCDT.iLocalEntryCount = 0;
	iCDT.iTotalEntryCount = 0;
	iCDT.iSize = 0;
	iCDT.iCommentLength = 0;
	
	// iterate through each zip entry, appending local file header (LFH), file data (FD) to file
	iZipMemberLinkedListIter.SetToFirst();
	CZipEntry* entry = iZipMemberLinkedListIter++;
	CZipEntry* prior = NULL;
	User::LeaveIfNull(entry);
	do
		{
		if (prior)
			{
			// adjust file header local offset if necessary
			entry->SetOffset(prior->ReturnOffset());
			}

		// insert entry's Local File Header
		AppendLocalFileHeaderL(stream, *entry);
		
		// insert entry's File Data
		AppendCompressedDataL(stream, *entry);
		
		// adjust cdt parameters
		iCDT.iLocalEntryCount++;
		iCDT.iTotalEntryCount++;
		iCDT.iSize += entry->FileHeaderSize();
		
		// move onto the next entry
		prior = entry;
		entry = iZipMemberLinkedListIter++;
		} while (entry != NULL);
		
	// adjust cdt offset parameter
	iCDT.iOffset = iZipMemberLinkedList.Last()->ReturnOffset();
	
	// iterate through each zip entry, appending the central directory File Headers to file (these must go after LFH, FD entries)
	iZipMemberLinkedListIter.SetToFirst();
	entry = iZipMemberLinkedListIter++;
	
	do
		{
		// central directory File Header
		AppendCentralDirectoryFileHeaderL(stream, *entry);
		
		// move onto the next entry
		entry = iZipMemberLinkedListIter++;
		} while (entry != NULL);
	
	// append the central directory trailer
	AppendCentralDirectoryTrailerL(stream);	
	
	// commit the transaction
	stream.CommitL();
	stream.Pop();
	stream.Close();
	
	// go back through & adjust compressed size values for each zip entry post-mortem
	ASSERT(iFile.SubSessionHandle() == 0);
	User::LeaveIfError(iFile.Open(iFs, iFileName, EFileWrite | EFileShareExclusive));
	TInt pos = 0;
	const TInt remainder = 8; // CZipArchive::KLocalHeaderFixedLength - KCompressedSizeRelativePosition
	User::LeaveIfError(iFile.Seek(ESeekStart, pos));
	iZipMemberLinkedListIter.SetToFirst();
	entry = iZipMemberLinkedListIter++;
	do
		{
		pos += KCompressedSizeRelativePosition; // step to iCompressedSize location in the current entry's LFH
		TUint32 cs = entry->iLFH.iCompressedSize;
		TUint32 KMyByte = 0x000000FF;
		TInt bytecount = 0;
		TBuf8<8> compSize;
		do
			{
			TUint32 csChar((cs & KMyByte));
			compSize.Append(csChar);
			cs >>= 8;
			} while (++bytecount < 4); // zip-file format allows us 4 bytes only for the compressed size value
		User::LeaveIfError(iFile.Write(pos, compSize));
		pos += bytecount; // the 4 bytes we just wrote
		
		// step over remaing bytes to the beginning of the next entry's LFH
		pos += remainder; 
		pos += entry->iAsciiName.Size();
		pos += entry->iLFH.iCompressedSize;
		
		// we're now positioned at the next entry's LFH
		entry = iZipMemberLinkedListIter++;
		} while (entry != NULL);
	}

void CZipItUp::AppendLocalFileHeaderL(RFileWriteStream& aStream, CZipEntry& aZipEntry)
	{
	aStream.WriteUint32L(aZipEntry.iLFH.iSignature);
	aStream.WriteUint16L(aZipEntry.iLFH.iVersionNeeded);
	aStream.WriteUint16L(aZipEntry.iLFH.iFlags);
	aStream.WriteUint16L(aZipEntry.iLFH.iCompressionMethod);
	aStream.WriteUint16L(aZipEntry.iLFH.iLastModifiedFileTime);
	aStream.WriteUint16L(aZipEntry.iLFH.iLastModifiedFileDate);
	aStream.WriteUint32L(aZipEntry.iLFH.iCRC32);
	aStream.WriteUint32L(aZipEntry.iLFH.iCompressedSize);
	aStream.WriteUint32L(aZipEntry.iLFH.iUncompressedSize);
	aStream.WriteUint16L(aZipEntry.iLFH.iFileNameLength);
	aStream.WriteUint16L(aZipEntry.iLFH.iExtraFieldLength);
	aStream.WriteL(aZipEntry.iAsciiName);
	}

void CZipItUp::AppendCompressedDataL(RFileWriteStream& aStream, CZipEntry& aZipEntry)
	{
	// compress data & stream it to the zip archive (aStream)
	RFileReadStream inStream;
	inStream.Attach(aZipEntry.iInput); // note takes ownership of iInput subsession handle
	CBufferManager* fb = CBufferManager::NewLC(inStream, aStream, aZipEntry.iLFH.iUncompressedSize, KDefaultZipBufferLength);
	CEZCompressor* compressor = CEZCompressor::NewLC(*fb, CEZCompressor::EBestCompression, KWindowBits, CEZCompressor::EDefMemLevel, CEZCompressor::EDefaultStrategy);
	while (compressor->DeflateL()){/* do nothing */}
	
	// update the entry's LocalFileHeader and FileHeader
	aZipEntry.iLFH.iCompressedSize = fb->TotalBytesOut();
	aZipEntry.iFH.iCompressedSize = aZipEntry.iLFH.iCompressedSize;
	
	// cleanup
	CleanupStack::PopAndDestroy(2, fb); // compressor, fb
	inStream.Close();
	}

void CZipItUp::AppendCentralDirectoryFileHeaderL(RFileWriteStream& aStream, CZipEntry& aEntry)
	{
	aStream.WriteUint32L(aEntry.iFH.iSignature);
	aStream.WriteUint16L(aEntry.iFH.iMadeBy);
	aStream.WriteUint16L(aEntry.iFH.iRequired);
	aStream.WriteUint16L(aEntry.iFH.iFlags);
	aStream.WriteUint16L(aEntry.iFH.iCompressionMethod);
	aStream.WriteUint16L(aEntry.iFH.iLastModifiedFileTime);
	aStream.WriteUint16L(aEntry.iFH.iLastModifiedFileDate);
	aStream.WriteUint32L(aEntry.iFH.iCRC32);
	aStream.WriteUint32L(aEntry.iFH.iCompressedSize);
	aStream.WriteUint32L(aEntry.iFH.iUncompressedSize);
	aStream.WriteUint16L(aEntry.iFH.iFileNameLength);
	aStream.WriteUint16L(aEntry.iFH.iExtraFieldLength);
	aStream.WriteUint16L(aEntry.iFH.iFileCommentLength);
	aStream.WriteUint16L(aEntry.iFH.iDiskNumberStart);
	aStream.WriteUint16L(aEntry.iFH.iInternalFileAttributes);
	aStream.WriteUint32L(aEntry.iFH.iExternalFileAttributes);
	aStream.WriteUint32L(aEntry.iFH.iLocalHeaderOffset);
	aStream.WriteL(aEntry.iAsciiName);
	}

void CZipItUp::AppendCentralDirectoryTrailerL(RFileWriteStream& aStream)
	{
	aStream.WriteUint32L(iCDT.iSignature);
	aStream.WriteUint16L(iCDT.iDiskNumber);
	aStream.WriteUint16L(iCDT.iStartDiskNumber);
	aStream.WriteUint16L(iCDT.iLocalEntryCount);
	aStream.WriteUint16L(iCDT.iTotalEntryCount);
	aStream.WriteUint32L(iCDT.iSize);
	aStream.WriteUint32L(iCDT.iOffset);
	aStream.WriteUint16L(iCDT.iCommentLength);	
	}
