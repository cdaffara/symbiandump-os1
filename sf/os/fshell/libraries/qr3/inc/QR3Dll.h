// QR3Dll.h
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
#ifndef QR3DLL_H
#define QR3DLL_H

#include <e32base.h>
#include <e32property.h>
#include <fshell/common.mmh>

class RFile;
class RMemoryAccess;


// Deprecated, use RProxyAllocatorHelper instead
// Break dependancy on analyseheap
class THeapDetails
	{
public:
	/** Base address of the heap. Not very useful. */
	TUint8* iBase;
	/** First free cell on the heap. Not very useful either. */
	TUint8* iFirstFree;
	/** Top address of the heap. Not very useful. */
	TUint8* iTop;
	/** Size of heap. This includes free space. */
	TInt iSize;
	/** Size of a heap cell header. This is not directly useful, but is necessary
	    when calculating the amount of free space. */
	TInt iCellHeaderSize;
	/** Total allocated space in the heap. This excludes cell headers. */
	TInt iTotalAllocSize;
	/** Number of allocated cells on the heap. */
	TInt iCellCount;
	/** Returns the amount of free space on the heap. This is the size of the heap,
	    minus the free space, minus the space occupied by the heap cell headers. */
	inline TInt FreeSpace() 
		{
		return iSize - iTotalAllocSize - (iCellHeaderSize * iCellCount);
		}
	};

// Deprecated, use RProxyAllocatorHelper instead
class TTomsciHeapDetails : public THeapDetails
	{
public:
	TInt iMaxSize;
	TInt iMinSize;
	TInt iNumUnusedPages;
	TInt iFragmentedSpaceNotInPages; // size of the free space minus iNumUnusedPages*KPageSize
	};

// Deprecated, use RProxyAllocatorHelper instead
class RProxyHeap : public RHeap
	{
public:
#ifdef FSHELL_ANALYSEHEAP_SUPPORT
	TInt MinLength() { return iMinLength; }
	TInt MaxLength() { return iMaxLength; }
	using RHeap::iFree;
	using RHeap::iTop;
	using RHeap::GetAddress;
#endif
	using RAllocator::iCellCount;
	using RAllocator::iTotalAllocSize;

	IMPORT_C RProxyHeap(RMemoryAccess& aMem, TUint aThreadId);

	IMPORT_C void GetHeapDetailsL(THeapDetails& aDetails);
	IMPORT_C void DumpHeapToSuitableFileInDirectoryL(TFileName& aName); // pass in directory path in aName, it returns the file name
	IMPORT_C void DumpHeapToFileL(const TDesC& aFileName);
	IMPORT_C void DumpHeapL(RFile& aDumpFile);


public:
	RMemoryAccess& iMem;
	TUint iThreadId;
	TUint iPadding[6];
	};

IMPORT_C void GetHeapDetailsL(THeapDetails& aDetails, RProxyHeap& heap); // Deprecated, use RProxyAllocatorHelper instead

NONSHARABLE_CLASS(CPropertyManager) : public CActive
	{
public:
	IMPORT_C static CPropertyManager* NewL(TCallBack aCallback);
	IMPORT_C void SetPropertyFromStringL(const TDesC& aString);
	IMPORT_C HBufC* GetValueL();
	IMPORT_C TInt ChangeProperty(TUint aCategory, TUint aKey);

	inline TInt Category() { return iCat; }
	inline TInt Key() { return iKey; }

	~CPropertyManager();

private:
	void RunL();
	void DoCancel();
	CPropertyManager(TCallBack aCallback);
	void Watch();

private:
	RProperty iProperty;
	RProperty::TType iPropertyType;
	TUint iCat;
	TUint iKey;
	TCallBack iChangedFn;
	};

//////// Key capturing stuff

// The simple interface for people who only want simple callbacks equivalent to EEventKey events.
class MCaptureInterface
	{
public:
	virtual void CapturedKeyPressedL(TInt aIdentifier)=0;
	};

/*
class MDetailedCaptureInterface : public MCaptureInterface
	{
public:
	// If you implement both overloads of CapturedKeyPressedL, this one will be called {before,after} the simple one above
	virtual void CapturedKeyPressedL(TInt aIdentifier, const TKeyEvent& aKeyEvent, TEventCode aType)=0;
	};
*/

struct SCapture;

NONSHARABLE_CLASS(CKeyCapturer) : public CActive
	{
public:
	IMPORT_C static CKeyCapturer* GetCapturerL();
	IMPORT_C static void DeleteCapturer();
	IMPORT_C void AddCaptureL(MCaptureInterface* aClient, TInt aIdentifier, TInt aKeyCode);
	IMPORT_C void AddCaptureL(MCaptureInterface* aClient, TInt aIdentifier, TInt aKeyCode, TInt aScanCode, TInt aCapturePriority);
	IMPORT_C void AddLongCaptureL(MCaptureInterface* aClient, TInt aIdentifier, TInt aKeyCode, TInt aScanCode); // Annoying as it is, the scan code is required to sucessfully perform a long capture across window groups
	IMPORT_C void AddLongCaptureL(MCaptureInterface* aClient, TInt aIdentifier, TInt aKeyCode, TInt aScanCode, TInt aCapturePriority);
	//void AddCaptureL(MDetailedCaptureInterface* aClient, TInt aIdentifier, TInt aKeyCode); // Can't do a long capture and get detailed info back - it's too confusing to support
	IMPORT_C void RemoveCapture(MCaptureInterface* aClient, TInt aIdentifier);
	IMPORT_C TInt IsCaptured(TInt aIdentifier); // Returns the keycode it's captured as
	IMPORT_C TInt IsCaptured(TInt aIdentifier, TBool& aIsLong);
	IMPORT_C void PushCurrentCapturesL(); // Removes all current captures. They may be restored by calling PopCapturesL(). This function may be called repeatedly, you must call Pop once for each push
	IMPORT_C void PopCapturesL();
	IMPORT_C TInt GetScanCodeOfLastEvent(); // This is only usefully called from within CapturedKeyPressedL

	~CKeyCapturer();
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	static CKeyCapturer* NewLC();
	CKeyCapturer();
	void ConstructL();
	void CancelCapture(SCapture& cap);
	void AddCaptureL(SCapture& cap);
	void RemoveDummyForCapture(const SCapture& aCapture);

private:
	enum
		{
		EOpaquePadding1 = 12,
		EOpaquePadding2 = 8,
		};
	// Data structures are not exposed
	TUint8 iOpaque1[EOpaquePadding1];
	TUint8 iOpaque2[EOpaquePadding2];
	TAny* iOpaque3; 
	TAny* iOpaque4;
	TInt32 iUsedSpareKeys; // Used for doing long captures
	TInt iLastEventScanCode;
	};

#ifdef FSHELL_QR3_SUPPORT_LOGGINGALLOCATOR
#include <fshell/LoggingAllocator.h> // Old definition was in this header, so for compatibility pull it in
#endif

enum TKernelObjectType
	{
	EListProcess,
	EListThread,
	EListChunk,
	EListServer,
	EListCodeSeg,
	EListHal,
	EListWindowGroups,
	EListMimeTypes,
	EListOpenFiles,
	EListFeatureReg,
	EListMessageQueue,
	EListMutex,
	EListSemaphore,
	EListTimer,
	};

class MQikListBoxModel;
class CKernListBoxModel;
class RClogger;

NONSHARABLE_CLASS(CKernelObjectList) : public CBase
	{
public:
	IMPORT_C static CKernelObjectList* NewL(RMemoryAccess* aMemAccess); // aMemAccess can be null
	~CKernelObjectList();

	IMPORT_C void SetCurrentType(TKernelObjectType aType);
	IMPORT_C void RefreshDataL(TInt aIndex=-1); // -1 means refresh all
	IMPORT_C MQikListBoxModel* GetQikListBoxModel(); // Returns an object suitable for controlling a UIQ3 list box.
	IMPORT_C void SetInfoChangedCallback(TInt aIndex, const TCallBack& aCallback); // Sets up a callback that will be completed whenever the model detects that aIndex's entry is out of date. Clunky, should be replaced with something neater...

	IMPORT_C TInt Count() const;
	
	// Following two functions are deprecated in favour of GetInfoByIndexL
	IMPORT_C void GetInfoL(TInt aIndex, RBuf& aTitle, RBuf& aInfo); // aInfo is a one-liner
	IMPORT_C void GetVerboseInfoL(TInt aIndex, RBuf& aTitle, RBuf& aInfo); // aInfo is multi-line, formatted with LFs
	
	IMPORT_C TInt GetInfoByIndexL(TInt aIndex, TBool aVerbose, RBuf& aTitle, RBuf& aInfo); // Returns the item id if supported by the current type
	IMPORT_C void GetInfoByIdL(TInt aId, TBool aVerbose, RBuf& aTitle, RBuf& aInfo); // Looks up by id rather than index. Id is specific to the current type - for threads it is thread ID, for others it may be arbitrary

	IMPORT_C void DumpAllInfoL(RClogger& aClogger);

private:
	CKernelObjectList(RMemoryAccess* aMemAccess);
	void ConstructL();

private:
	RMemoryAccess* iMemAccess;
	CKernListBoxModel* iModel;
	TKernelObjectType iCurrent;
	};

IMPORT_C TInt StartNotifierProxyThreadFunction(TAny*);
IMPORT_C TInt ShutdownProxyNotifier();
IMPORT_C TBool NotifierProxyIsRunning();

IMPORT_C TInt ExeNameForSid(TUint aSid, RBuf& aBuf);

#endif
