// heap.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <fshell/qr3dll.h>
#include <fshell/memoryaccess.h>
#include <f32file.h>

EXPORT_C RProxyHeap::RProxyHeap(RMemoryAccess& aMem, TUint aThreadId)
	: RHeap(), iMem(aMem), iThreadId(aThreadId)
	{
	}

EXPORT_C void /*CHeapAnalyser::*/ GetHeapDetailsL(THeapDetails& aDetails, RProxyHeap& heap)
	{
	// This function retained for BC reasons
	heap.GetHeapDetailsL(aDetails);
	}

EXPORT_C void RProxyHeap::GetHeapDetailsL(THeapDetails& aDetails)
	{
#ifdef FSHELL_ANALYSEHEAP_SUPPORT
	RProxyHeap& heap = *this;
	RMemoryAccess& iMemoryAccess = heap.iMem;
	TUint aThreadId = heap.iThreadId;

	TUint8* allocatorAddress;
	User::LeaveIfError(iMemoryAccess.GetAllocatorAddress(aThreadId, allocatorAddress));
	
	TThreadMemoryAccessParams params;
	params.iId = aThreadId;
	params.iAddr = allocatorAddress;
	params.iSize = sizeof(RHeap);
	TPckg<RHeap> heapPckg(heap);
	User::LeaveIfError(iMemoryAccess.GetThreadMem(params, heapPckg));
	
	// Base address of the heap
	aDetails.iBase = heap.Base(); // inline, returns member variable
	// First free cell address in the heap
	aDetails.iFirstFree = (TUint8*)heap.iFree.next; // direct read from member variable
	// Top address of the heap
	aDetails.iTop = heap.iTop;
	// Size of heap
	aDetails.iSize = heap.Size(); // inline, returns member variables (after simple maths)
	
	// Heap cell header size
	#ifdef UREL_E32
	aDetails.iCellHeaderSize = sizeof(RHeap::SCell*);
	#elif UDEB_E32
	aDetails.iCellHeaderSize = sizeof(RHeap::SDebugCell); // If allocator's UDEB-ness matches ours
	#else // match E32 and our own UDEB/URELness
	aDetails.iCellHeaderSize = RHeap::EAllocCellSize; // If allocator is urel and we're not
	#endif
	
	aDetails.iTotalAllocSize = heap.iTotalAllocSize;

	aDetails.iCellCount = heap.iCellCount;
#else
	User::Leave(KErrNotSupported);
#endif
	}

//BEGIN pinched directly from anaylseheapremote

_LIT(KLitUnderscore, "_");
_LIT(KLitHeap, ".heap");
_LIT(KLitDash, "-");
_LIT(KLitIllegals, ":@[]{}%");
	
#define AH_DEBUG(x)
#define AH_DEBUGINT(x,y)

EXPORT_C void RProxyHeap::DumpHeapToSuitableFileInDirectoryL(TFileName& aName)
	{
	TUint aThreadId = iThreadId;

	AH_DEBUG("Dumping to file");
	RThread theThread;
	User::LeaveIfError(theThread.Open(aThreadId));
	CleanupClosePushL(theThread);
	AH_DEBUG("Opened thread");

	//BaflUtils::
	// create a filename <location>\threadName_threadId.heap
	TFileName& theFileName(aName);
	const TInt fullNameLengthToTake = theFileName.MaxLength() - theFileName.Length() - KLitHeap().Length() - KLitUnderscore().Length() - 4; // 4 for thread ID number
	TFullName cleanedName = theThread.FullName().Left(fullNameLengthToTake);
	for (TInt whichIllegal = 0;whichIllegal < KLitIllegals().Length(); whichIllegal++)
		{
		TChar thisIllegal = KLitIllegals()[whichIllegal];
		TInt where;
		while ((where = cleanedName.Locate(thisIllegal)) != KErrNotFound)
			cleanedName.Replace(where,1,KLitDash());
		}
	theFileName.Append(cleanedName);
	theFileName.Append(KLitUnderscore);
	theFileName.AppendNum(theThread.Id());
	theFileName.Append(KLitHeap);
	//RDebug::Print(_L("The filename is %S"), &theFileName);
	CleanupStack::PopAndDestroy(); // theThread
	AH_DEBUG("Constructed filename");
	DumpHeapToFileL(theFileName);
	}

EXPORT_C void RProxyHeap::DumpHeapToFileL(const TDesC& aFileName)
	{
	AH_DEBUG("Dumping to arbitrary file");
	// Open the file
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	AH_DEBUG("Connected to F32");
	TInt err = fs.Delete(aFileName);
	if (err != KErrNotFound && err != KErrNone)
		User::Leave(err);
	AH_DEBUG("File deleted");
	RFile f;
	User::LeaveIfError(f.Create(fs,aFileName,EFileWrite));
	CleanupClosePushL(f);
	AH_DEBUG("File created");
	DumpHeapL(f);
	AH_DEBUG("Heap dump appeared to succeed");
	User::LeaveIfError(f.Flush());
	AH_DEBUG("File flushed");
	CleanupStack::PopAndDestroy(2); // f, fs
	}

static void DumpCodeSegsL(RMemoryAccess& aMem, RFile& aDumpFile);

EXPORT_C void RProxyHeap::DumpHeapL(RFile& aDumpFile)
	{
	TUint aThreadId = iThreadId;
	// get the thread
	RThread thread;
	AH_DEBUG("Opening thread");
	User::LeaveIfError(thread.Open(aThreadId));
	AH_DEBUG("Opened thread");
	
	THeapDetails heapDetails;
	GetHeapDetailsL(heapDetails);
	
	// write out the heap file in the version 3 format documented in the 'docs' folder.
	
	// 4 bytes: file format version number (Not present for version 1, number found would be heap base & therefore v.big, >1000)	
	TInt version = 3;
	AH_DEBUG("Dumping version");
	User::LeaveIfError(aDumpFile.Write(TPtrC8((TUint8*)&version, 4)));
	
	// 4 bytes: thread ID
	AH_DEBUG("Dumping thread ID");
	User::LeaveIfError(aDumpFile.Write(TPtrC8((TUint8*)&aThreadId, 4)));
	
	// 4 bytes: owning process - appears unused
	AH_DEBUG("Dumping owning process");
	TInt nothing = 0;
	User::LeaveIfError(aDumpFile.Write(TPtrC8((TUint8*)&nothing, 4)));
	
	// 4 bytes: length of thread name
	AH_DEBUG("Dumping thread name length");
	TName threadName = thread.FullName();
	TInt threadNameLength = threadName.Length();
	User::LeaveIfError(aDumpFile.Write(TPtrC8((TUint8*)&threadNameLength, 4)));
	
	// a bytes: thread name
	AH_DEBUG("Dumping thread name");
	HBufC8* asciiName = HBufC8::NewLC(threadNameLength);
	asciiName->Des().Copy(threadName);
	User::LeaveIfError(aDumpFile.Write(*asciiName));
	CleanupStack::PopAndDestroy(asciiName);
	

	// 4 bytes: base address of the heap
	User::LeaveIfError(aDumpFile.Write(TPtrC8((TUint8*)&heapDetails.iBase, 4)));
	// 4 bytes: first free cell address in the heap
	User::LeaveIfError(aDumpFile.Write(TPtrC8((TUint8*)&heapDetails.iFirstFree, 4)));
	// 4 bytes: top address of the heap
	User::LeaveIfError(aDumpFile.Write(TPtrC8((TUint8*)&heapDetails.iTop, 4)));
	// 4 bytes: cell header size
	User::LeaveIfError(aDumpFile.Write(TPtrC8((TUint8*)&heapDetails.iCellHeaderSize, 4)));
	// 4 bytes: heap size (n)
	User::LeaveIfError(aDumpFile.Write(TPtrC8((TUint8*)&heapDetails.iSize, 4)));
	
	// n bytes: heap contents
	// Need to read this across the process boundary
	// We need to copy in several chunks as memoryaccess_eka2 cannot currently
	// copy > 4096 bytes in one go.
	// But this number is not advertised by the memoryaccess_eka2 interface,
	// so we will have to be a bit careful in case it changes in future versions.
	const TInt KChunkSize = 4096;
	AH_DEBUG("Allocating heap storage buffer");
	HBufC8* heapBig = HBufC8::NewLC(KChunkSize);
	TPtr8 myPtr = heapBig->Des();
	TUint8* currentPointer = heapDetails.iBase;
	TUint8* endPointer = currentPointer + heapDetails.iSize;
	TThreadMemoryAccessParams paramsForHeap;
	paramsForHeap.iId = aThreadId;
	while (currentPointer < endPointer)
		{
		AH_DEBUGINT("Copying heap from %d", currentPointer);
		paramsForHeap.iAddr = currentPointer;
		paramsForHeap.iSize = Min(KChunkSize, (endPointer - currentPointer));
		AH_DEBUGINT("Length to copy %d", paramsForHeap.iSize);
		User::LeaveIfError(iMem.GetThreadMem(paramsForHeap, myPtr));
		AH_DEBUG("Writing some heap");	
		User::LeaveIfError(aDumpFile.Write(*heapBig));
		currentPointer += heapBig->Length();
		}
	CleanupStack::PopAndDestroy(heapBig);
	
	// Now output the code segment details
	iMem.AcquireCodeSegMutex();
	TRAPD(err,DumpCodeSegsL(iMem, aDumpFile));
	iMem.ReleaseCodeSegMutex();
	User::LeaveIfError(err);
	}
	
void DumpCodeSegsL(RMemoryAccess& aMem, RFile& aDumpFile)
	{
	TCodeSegKernelInfo info;
	TPckg<TCodeSegKernelInfo> infoPckg(info);
	while (aMem.GetNextCodeSegInfo(infoPckg))
		{
		// 4 bytes: Code segment run address
		AH_DEBUG("Dumping code seg run address");
		
		User::LeaveIfError(aDumpFile.Write(TPtrC8((TUint8*)&info.iRunAddress, 4)));
		// 4 bytes: Code segment size
		AH_DEBUG("Dumping code seg size");
		User::LeaveIfError(aDumpFile.Write(TPtrC8((TUint8*)&info.iSize, 4)));
		// 4 bytes: Code segment filename length
		AH_DEBUG("Dumping code seg filename length");
		TInt nameLength = info.iFileName.Length();
		User::LeaveIfError(aDumpFile.Write(TPtrC8((TUint8*)&nameLength, 4)));
		// 4 bytes: Code segment filename length
		AH_DEBUG("Dumping code seg filename length");
		User::LeaveIfError(aDumpFile.Write(info.iFileName));
		}
	}

//END nicked
