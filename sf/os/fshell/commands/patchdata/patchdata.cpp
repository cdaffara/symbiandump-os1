// patchdata.cpp
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

#include <fshell/ioutils.h>
#include <f32image.h>
#include "sf_deflate.h"
#include <e32rom.h>
#include <fshell/memoryaccesscmd.h>

using namespace IoUtils;

IMPORT_C extern const TInt KHeapMinCellSize;

class CCmdPatchdata : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdPatchdata();
private:
	CCmdPatchdata();
	void HandleXipL();

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	// The stuff from elsewhere
	void LoadFileInflateL(E32ImageHeaderComp* aHeader,TUint8* aRestOfFileData,TUint32 aRestOfFileSize);
	void LoadFileNoCompressL(E32ImageHeaderComp* aHeader,TUint8* aRestOfFileData,TUint32 aRestOfFileSize);
	TInt LoadFile(TUint32 aCompression,E32ImageHeaderComp* aHeader,TUint8* aRestOfFileData,TUint32 iRestOfFileSize);	

private:
	HBufC* iDll;
	TInt iOrdinal;
	TInt iNewValue;
	TFileName iPath;
	TFileName iNewPath;
	RFile iFile;
	TBool iVerbose;
	};


CCommandBase* CCmdPatchdata::NewLC()
	{
	CCmdPatchdata* self = new(ELeave) CCmdPatchdata();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdPatchdata::~CCmdPatchdata()
	{
	delete iDll;
	iFile.Close();
	}

CCmdPatchdata::CCmdPatchdata()
	{
	}

const TDesC& CCmdPatchdata::Name() const
	{
	_LIT(KName, "patchdata");	
	return KName;
	}

void CCmdPatchdata::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendStringL(iDll, _L("dll-name"));
	aArguments.AppendIntL(iOrdinal, _L("ordinal"));
	aArguments.AppendUintL((TUint&)iNewValue, _L("value"));
	}

void CCmdPatchdata::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, _L("verbose"));
	}

EXE_BOILER_PLATE(CCmdPatchdata)

void CCmdPatchdata::DoRunL()
	{
	if (!iDll)
		{
		// Just print some standard ones
		Printf(_L("KHeapMinCellSize: %d\r\n"), KHeapMinCellSize);
		return;
		}

	TBool scan = !iArguments.IsPresent(1);
	TBool change = iArguments.IsPresent(2);

	// First figure out where the dll is - easiest way is to load it
#ifdef __WINS__
	PrintWarning(_L("On WINS, exe-name must be a complete path to a E32 DLL"));
	iPath = *iDll;
#else
	// Can't use RLibrary because we have all caps and the DLL won't
	TFindFile find(FsL());
	TInt found = find.FindByDir(*iDll, _L("Y:\\sys\\bin\\"));
	LeaveIfErr(found, _L("Couldn't find DLL %S"), iDll);
	iPath = find.File();

	if (FsL().IsFileInRom(iPath) != NULL)
		{
		HandleXipL();
		return;
		}
#endif

	iNewPath = iPath;
	TUint fileopenmode = EFileRead|EFileStream|EFileShareAny;

	if (change) fileopenmode = EFileWrite|EFileStream|EFileShareAny;

	if (change && iPath[0] != 'c')
		{
		// If file is already on c then we just patch in place

		// We have a new value to set - so copy the DLL to C in preparation for patching it
		iNewPath[0] = 'c';

		//CleanupStack::PushL(TCleanupItem(&DeleteModifiedBinary, this));

		TInt err = FsL().MkDirAll(iNewPath); // In case C:\sys\bin doesn't exist yet
		if (err && err != KErrAlreadyExists)
			{
			PrintError(err, _L("Couldn't create C:\\sys\\bin"));
			User::Leave(err);
			}

		CFileMan* fm = CFileMan::NewL(Fs());
		CleanupStack::PushL(fm);

		LeaveIfErr(fm->Copy(iPath, iNewPath), _L("Couldn't copy file from %S to %S"), &iPath, &iNewPath);
		// Clear the read-only bit in the case where we've copied from ROM
		LeaveIfErr(Fs().SetAtt(iNewPath, 0, KEntryAttReadOnly), _L("Couldn't unset read-only flag"));
		CleanupStack::PopAndDestroy(fm);
		}

	// Now start fiddling
	LeaveIfErr(iFile.Open(FsL(), iNewPath, fileopenmode), _L("Couldn't open file %S"), &iNewPath);

	///// Begin code lifted from chkdeps
	E32ImageHeaderV* imageHeader=new(ELeave)E32ImageHeaderV;
	CleanupStack::PushL(imageHeader);
	TPckg<E32ImageHeaderV> ptr(*imageHeader);
	LeaveIfErr(iFile.Read(ptr, sizeof(E32ImageHeaderV)), _L("Couldn't read E32ImageHeader"));

	if (!scan && (iOrdinal <= 0 || iOrdinal-1 >= imageHeader->iExportDirCount)) // -1 because ordinals are 1-based in DEF files
		{
		LeaveIfErr(KErrArgument, _L("ordinal out of range: only %d exports in file"), imageHeader->iExportDirCount);
		}
	if (imageHeader->HeaderFormat() >= KImageHdrFmt_V && imageHeader->iExportDescType != KImageHdr_ExpD_NoHoles)
		{
		LeaveIfErr(KErrNotSupported, _L("Don't understand files whose export table format isn't KImageHdr_ExpD_NoHoles (format is %d)"), imageHeader->iExportDescType);
		}

	TInt exportOffset = imageHeader->iExportDirOffset;

	// Decompress rest of image
	TUint32 compression = imageHeader->CompressionType();
	TInt restOfFileSize=0;
	TUint8* restOfFileData=NULL;
	//detect the size of import information
	if (compression != KFormatNotCompressed)
		{
		// Compressed executable
		// iCodeOffset	= header size for format V or above
		//				= sizeof(E32ImageHeader) for format J
		restOfFileSize = imageHeader->UncompressedFileSize() - imageHeader->iCodeOffset;
		}
	else
		{
		TInt FileSize;
		iFile.Size(FileSize); 		
		restOfFileSize = FileSize-imageHeader->TotalSize();
		}	
	//restOfFileSize -= imageHeader->iCodeSize; // the size of the exe less header & code
	
	//allocate memory for rest of file
	if (restOfFileSize >0)
		{
		restOfFileData = (TUint8*)User::AllocLC(restOfFileSize );		
		}

	LeaveIfErr(LoadFile(compression,imageHeader,restOfFileData,restOfFileSize), _L("Failed to load file data")); // Read import information in

	///// End code lifted from chkdeps

	const TInt headerSize = imageHeader->iCodeOffset;
	exportOffset -= headerSize; // we are indexing into restoffile which doesn't have the header

	TInt* exports = (TInt*)(restOfFileData + exportOffset);

	if (scan)
		{
		//Printf(_L("TextSize = %d\r\n"), imageHeader->iTextSize);
		//Printf(_L("CodeSize = %d\r\n"), imageHeader->iCodeSize);
		//Printf(_L("dataoffset = %d\r\n"), imageHeader->iDataOffset);
		//Printf(_L("importoffset= %d\r\n"), imageHeader->iImportOffset);
		//Printf(_L("exportdiroffset= %d\r\n"), imageHeader->iExportDirOffset);
		for (TInt i = 0; i < imageHeader->iExportDirCount; i++)
			{
			// No direct way of figuring out where code stops and const data starts. Export table sits between them though
			// so use this to distiguish. (Don't know if my logic here is right but it seems to mainly work...)
			TInt valoffset = exports[i] - imageHeader->iCodeBase;
			if (iVerbose)
				{
				Printf(_L("export[%d] = %d\r\n"), i, valoffset + headerSize);
				}
			if (valoffset >= (TInt)imageHeader->iExportDirOffset-headerSize)
				{
				TInt val = *(TInt*)(restOfFileData + valoffset);
				Printf(_L("Ordinal %d: 0x%08x (%d)\r\n"), i+1, val, val);
				}
			}
		}
	else
		{
		TInt valoffset = exports[iOrdinal-1] - imageHeader->iCodeBase; // Subtract one from ordinal as they're 1-based in DEF files
		TInt& val = *(TInt*)(restOfFileData + valoffset);

		if (change)
			{
			TInt oldVal = val;
			val = iNewValue; // This updates restOfFileData
			// Now write everything back to file. The data has to be written uncompressed as there is no compression
			// code we can easily rip off. We have to update the header irrespective to increment the version
 			iFile.SetSize(headerSize);
			
			imageHeader->iCompressionType = KFormatNotCompressed;
			// Update the version to sort out linking bug (probably not an issue for most uses of patchable data, but it could be in theory)
			if ((imageHeader->iModuleVersion & 0x0000ffffu) == 0x0000ffffu)
				{
				// Don't update if version is XXXXffff as incrementing it would raise the major version - which is considered incompatible
				PrintWarning(_L("Couldn't update DLL's minor version as it is already at its maximum value"));
				}
			else
				{
				imageHeader->iModuleVersion++;
				}

			// Update e32 checksum
			imageHeader->iHeaderCrc = KImageCrcInitialiser;
			TUint32 crc = 0;
			Mem::Crc32(crc, imageHeader, imageHeader->TotalSize());
			imageHeader->iHeaderCrc = crc;
			LeaveIfErr(iFile.Write(0, ptr), _L("Couldn't write updated header back to file"));

			TPtrC8 data(restOfFileData, restOfFileSize);
			LeaveIfErr(iFile.Write(headerSize, data), _L("Couldn't write patched DLL code back to file"));

			Printf(_L("Ordinal %d: old value was 0x%08x, new value is 0x%08x"), iOrdinal, oldVal, iNewValue);
			}
		else
			{
			Printf(_L("Ordinal %d: 0x%08x (%d)\r\n"), iOrdinal, val, val);
			}
		}

	CleanupStack::PopAndDestroy(2, imageHeader); // imageHeader, restOfFileData
	}

void FileCleanup(TAny* aPtr)
	{
	TFileInput* f=(TFileInput*)aPtr;
	f->Cancel();
	delete f;
	}

void CCmdPatchdata::LoadFileInflateL(E32ImageHeaderComp* aHeader,TUint8* aRestOfFileData,TUint32 aRestOfFileSize)
	{
	TInt pos = aHeader->TotalSize();
	User::LeaveIfError(iFile.Seek(ESeekStart,pos)); // Start at beginning of compressed data

	TFileInput* file = new (ELeave) TFileInput(iFile);
	CleanupStack::PushL(TCleanupItem(&FileCleanup,file));
	CInflater* inflater=CInflater::NewLC(*file);
	
	/*if (aHeader->iCodeSize)
		{
		TUint8* CodeData = (TUint8*)User::AllocLC(aHeader->iCodeSize );
		TInt count=inflater->ReadL((TUint8*)CodeData ,aHeader->iCodeSize,&Mem::Move);
		if(count!=aHeader->iCodeSize)
			User::Leave(KErrCorrupt);
		CleanupStack::PopAndDestroy(CodeData);
		}*/
	
	if (aRestOfFileSize)
		{
		TUint32 count=inflater->ReadL(aRestOfFileData,aRestOfFileSize,&Mem::Move);
		if(count!=aRestOfFileSize)
			User::Leave(KErrCorrupt);
		}
	CleanupStack::PopAndDestroy(2,file);
	}

void CCmdPatchdata::LoadFileNoCompressL(E32ImageHeaderComp* aHeader,TUint8* aRestOfFileData,TUint32 aRestOfFileSize)
	{
	TInt pos = (aHeader->TotalSize() /*+aHeader->iCodeSize*/);
	if (aRestOfFileSize)
		{
		User::LeaveIfError(iFile.Seek(ESeekStart,pos));
		TPtr8 ptrToData((TText8*)(aRestOfFileData),aRestOfFileSize,aRestOfFileSize);
		User::LeaveIfError(iFile.Read(ptrToData, (TInt)aRestOfFileSize));	
		}
	}	
//function loads file's import information calling decompression routine if needed
TInt CCmdPatchdata::LoadFile(TUint32 aCompression,E32ImageHeaderComp* aHeader,TUint8* aRestOfFileData,TUint32 aRestOfFileSize)
	{
	TInt r=KErrNone;
	if(aCompression==KFormatNotCompressed)
		{
		TRAP(r,LoadFileNoCompressL(aHeader,aRestOfFileData,aRestOfFileSize));		
		}
	else if(aCompression==KUidCompressionDeflate)
		{
		TRAP(r,LoadFileInflateL(aHeader,aRestOfFileData,aRestOfFileSize));
		}
	else
		r=KErrNotSupported;

	return r;
	}

void CCmdPatchdata::HandleXipL()
	{
	TBool scan = !iArguments.IsPresent(1);
	TBool change = iArguments.IsPresent(2);
	TRomImageHeader* imageHeader = (TRomImageHeader*)FsL().IsFileInRom(iPath);

	if (!scan && (iOrdinal <= 0 || iOrdinal-1 >= imageHeader->iExportDirCount)) // -1 because ordinals are 1-based in DEF files
		{
		LeaveIfErr(KErrArgument, _L("ordinal out of range: only %d exports in file"), imageHeader->iExportDirCount);
		}

	TLinAddr* exports = (TLinAddr*)(imageHeader->iExportDir);

	if (scan)
		{
		for (TInt i = 0; i < imageHeader->iExportDirCount; i++)
			{
			TLinAddr valAddr = exports[i];
			if (iVerbose)
				{
				Printf(_L("export[%d] = 0x%08x\r\n"), i, valAddr);
				}
			valAddr &= ~3; // Clear thumb bit - code exports can have this set. No idea why some can have bit 1 set...
			// There really doesn't seem to be a way to figure out where code stops and const data starts, for core images. It generally looks to come after the export dir but it gives false positives
			if (valAddr > (TLinAddr)imageHeader)
				{
				TInt val = *(TInt*)valAddr;
				Printf(_L("Ordinal %d: 0x%08x (%d)\r\n"), i+1, val, val);
				}
			}
		}
	else
		{
		TLinAddr valAddr = exports[iOrdinal-1]; // Subtract one from ordinal as they're 1-based in DEF files
		TInt val = *(TInt*)(valAddr);

		if (change)
			{
			TInt oldVal = val;

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
			LoadMemoryAccessL();
			TPckg<TInt> valPkg(iNewValue);
			LeaveIfErr(iMemAccess.WriteShadowMemory(valAddr, valPkg), _L("Couldn't write shadow for new value"));
#else
			LeaveIfErr(KErrNotSupported, _L("Can't update patchdata for DLLs in core image without memoryaccess support"));
#endif


			Printf(_L("Ordinal %d: old value was 0x%08x, new value is 0x%08x"), iOrdinal, oldVal, iNewValue);
			}
		else
			{
			Printf(_L("Ordinal %d: 0x%08x (%d)\r\n"), iOrdinal, val, val);
			}
		}
	}

