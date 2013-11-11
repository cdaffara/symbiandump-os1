// keycapture.cpp
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

class RWsSession;
class RWindowGroup;

#include <fshell/common.mmh>
#ifdef FSHELL_WSERV_SUPPORT
#include <w32std.h>
#endif

//#define KEYCAPTURE_DEBUG(args...) RDebug::Printf(args)
#define KEYCAPTURE_DEBUG(args...) 

enum
	{
	ELongCapture = 1,
	ECaptureIsDummy = 2,
	ECaptureIsKeyUpDown = 4,
	};

struct SCapture
	{
	MCaptureInterface* iClient;
	TInt iIdentifier;
	TInt iOriginalCode; // This is the code we were actually passed in.
	TInt iCaptureCode; // This is the key code we call capture on. This will be the same as iOriginalCode for a normal capture. For a long capture, it will be a virtual key. For an up-and-down, it will be the scan code equivalent of iOriginalCode
	TInt32 iCaptureHandle;
	TUint32 iFlags;
	TInt iPriority;
	};

#define iWs (*reinterpret_cast<RWsSession*>(iOpaque1))
#define iWg (*reinterpret_cast<RWindowGroup*>(iOpaque2))
#define iCaptures (*reinterpret_cast<CArrayFixFlat<SCapture>**>(&iOpaque3))
#define iPushList (*reinterpret_cast<CArrayPtrFlat<CArrayFixFlat<SCapture> >**>(&iOpaque4))
#define KSpareKeysMask 0xF

#define KSpareKeyRange EKeyF9 // We use the range F9-F12 inclusive as that seems to be about the only range that I can find not being used by UIQ or licensees

CKeyCapturer::CKeyCapturer()
	: CActive(CActive::EPriorityUserInput)
	, iUsedSpareKeys(0)
	{
	}

void CKeyCapturer::ConstructL()
	{
#ifdef FSHELL_WSERV_SUPPORT
	__ASSERT_COMPILE(sizeof(RWsSession) == EOpaquePadding1);
	__ASSERT_COMPILE(sizeof(RWindowGroup) == EOpaquePadding2);
	User::LeaveIfError(iWs.Connect());
	iWg = RWindowGroup(iWs);
	User::LeaveIfError(iWg.Construct((TUint32)this, EFalse));
	iWg.SetOrdinalPosition(-1,-1); // This appears to be needed to solve a race condition whereby someone creates a new window (or window group, or something) at roughly the same time as we do, which prevents the other window from coming to the foreground
	User::LeaveIfError(iWg.SetName(_L("CKeyCapturerWindowGroup"))); // To ease debugging in wserv

	iCaptures = new(ELeave) CArrayFixFlat<SCapture>(8);
	iPushList = new(ELeave) CArrayPtrFlat<CArrayFixFlat<SCapture> >(8);

	CActiveScheduler::Add(this);
	iWs.EventReady(&iStatus);
	SetActive();
#else
	User::Leave(KErrNotSupported);
#endif
	}

EXPORT_C void CKeyCapturer::AddCaptureL(MCaptureInterface* aClient, TInt aIdentifier, TInt aKeyCode)
	{
	AddCaptureL(aClient, aIdentifier, aKeyCode, 0, KMaxTInt);
	}

EXPORT_C void CKeyCapturer::AddCaptureL(MCaptureInterface* aClient, TInt aIdentifier, TInt aKeyCode, TInt aScanCode, TInt aCapturePriority)
	{
	if (aKeyCode == 0 || !aClient) User::Leave(KErrArgument);
	RemoveCapture(aClient, aIdentifier); // Just in case it's already been set

	SCapture cap;
	cap.iClient = aClient;
	cap.iIdentifier = aIdentifier;
	cap.iCaptureCode = aKeyCode;
	cap.iOriginalCode = aKeyCode;
	cap.iFlags = 0;
	cap.iPriority = aCapturePriority;

	AddCaptureL(cap);

	if (aScanCode)
		{
		// If we've been passed a scan code, use it to capture the keyup and keydown events too
		SCapture upanddownCap;
		upanddownCap.iClient = NULL;
		upanddownCap.iIdentifier = -1;
		upanddownCap.iCaptureCode = aScanCode;
		upanddownCap.iOriginalCode = aScanCode;
		upanddownCap.iFlags = ECaptureIsDummy | ECaptureIsKeyUpDown;
		upanddownCap.iPriority = aCapturePriority;

		TRAPD(err, AddCaptureL(upanddownCap));
		if (err)
			{
			CancelCapture(cap);
			iCaptures->Delete(iCaptures->Count()-1);
			User::Leave(err);
			}
		}
	}

EXPORT_C void CKeyCapturer::AddLongCaptureL(MCaptureInterface* aClient, TInt aIdentifier, TInt aKeyCode, TInt aScanCode)
	{
	AddLongCaptureL(aClient, aIdentifier, aKeyCode, aScanCode, KMaxTInt);
	}

EXPORT_C void CKeyCapturer::AddLongCaptureL(MCaptureInterface* aClient, TInt aIdentifier, TInt aKeyCode, TInt aScanCode, TInt aCapturePriority)
	{
	if (aKeyCode == 0 || aScanCode == 0 || !aClient) User::Leave(KErrArgument);
	RemoveCapture(aClient, aIdentifier); // Just in case it's already been set

	SCapture cap;
	cap.iClient = aClient;
	cap.iIdentifier = aIdentifier;
	cap.iCaptureCode = 0; // Is filled in later
	cap.iOriginalCode = aKeyCode;
	cap.iFlags = ELongCapture;
	cap.iPriority = aCapturePriority;

	AddCaptureL(cap);
	
	// In order to get repeat events (and, by extension, long key presses) the key events
	// must be going to the window group that's capturing the long press. Therefore,
	// we also need to capture the key events even if we don't have any client for that
	// key.
	//
	// Of course, this isn't documented anywhere.
	SCapture keyCap;
	keyCap.iClient = NULL; // So that no action is taken when a short press occurs
	keyCap.iIdentifier = -1;
	keyCap.iCaptureCode = aKeyCode;
	keyCap.iOriginalCode = aKeyCode;
	keyCap.iFlags = ECaptureIsDummy;
	keyCap.iPriority = aCapturePriority;
	
	TRAPD(err, AddCaptureL(keyCap));
	if (err)
		{
		CancelCapture(cap);
		iCaptures->Delete(iCaptures->Count()-1);
		User::Leave(err);
		}

	// And finally, do a upanddown capture as well, for good measure (TODO do this for normal captures as well, but need the scancode... grr.
	SCapture upanddownCap;
	upanddownCap.iClient = NULL;
	upanddownCap.iIdentifier = -1;
	upanddownCap.iCaptureCode = aScanCode;
	upanddownCap.iOriginalCode = aScanCode;
	upanddownCap.iFlags = ECaptureIsDummy | ECaptureIsKeyUpDown;
	upanddownCap.iPriority = aCapturePriority;

	TRAP(err, AddCaptureL(upanddownCap));
	if (err)
		{
		CancelCapture(keyCap);
		iCaptures->Delete(iCaptures->Count()-1);
		CancelCapture(cap);
		iCaptures->Delete(iCaptures->Count()-1);
		User::Leave(err);
		}
	}

void CKeyCapturer::AddCaptureL(SCapture& cap)
	{
#ifdef FSHELL_WSERV_SUPPORT
	TBool isLong = (cap.iFlags & ELongCapture);
	TBool isUpDown = (cap.iFlags & ECaptureIsKeyUpDown);
	//ASSERT(iFlags & (ELongCapture | ECaptureIsKeyUpDown) == 0); // Can't be both a long capture and an up-down

	if (isLong && iUsedSpareKeys == KSpareKeysMask)
		{
		// No more keys available to remap as long key presses
		User::Leave(KErrOverflow);
		}

	if (cap.iPriority == KMaxTInt) cap.iPriority = KMaxTInt-1; // Internally somewhere wserv adds one to the priority, and KMaxTInt+1 is KMinTInt...

	KEYCAPTURE_DEBUG("KC: AddCapture ident=%d on key=%d, long=%d, updown=%d, dummy=%d)", cap.iIdentifier, cap.iOriginalCode, isLong, isUpDown, cap.iFlags&ECaptureIsDummy);

	iCaptures->SetReserveL(iCaptures->Count() + 1); // So that the AppendL later cannot fail
	if (isLong)
		{
		TInt spareKey = -1; // There's a cleaner way to work this out, but since there's only 4, what's the point?
		if ((iUsedSpareKeys & 1) == 0) spareKey = 0;
		else if ((iUsedSpareKeys & 2) == 0) spareKey = 1;
		else if ((iUsedSpareKeys & 4) == 0) spareKey = 2;
		else if ((iUsedSpareKeys & 8) == 0) spareKey = 3;
		ASSERT(spareKey != -1); // Otherwise the check at the top should have caught this case
		cap.iCaptureCode = KSpareKeyRange + spareKey;
		
		cap.iCaptureHandle = iWg.CaptureLongKey(cap.iOriginalCode, cap.iCaptureCode, 0, 0, cap.iPriority, ELongCaptureWaitShort);
		User::LeaveIfError(cap.iCaptureHandle);
		iUsedSpareKeys |= 1 << spareKey;
		}
	else if (isUpDown)
		{
		cap.iCaptureHandle = iWg.CaptureKeyUpAndDowns(cap.iCaptureCode, 0, 0, cap.iPriority);
		User::LeaveIfError(cap.iCaptureHandle);
		}
	else
		{
		cap.iCaptureHandle = iWg.CaptureKey(cap.iCaptureCode, 0, 0, cap.iPriority);
		User::LeaveIfError(cap.iCaptureHandle);
		}
	iCaptures->AppendL(cap);
#else
	User::Leave(KErrNotSupported);
#endif
	}

EXPORT_C void CKeyCapturer::RemoveCapture(MCaptureInterface* aClient, TInt aIdentifier)
	{
	// Linear search thru iCaptures
	for (TInt i = 0; i < iCaptures->Count(); i++)
		{
		SCapture cap = iCaptures->operator[](i); // Copy cap so we can use it after it's been deleted below
		if (cap.iIdentifier == aIdentifier)
			{
			ASSERT(cap.iClient == aClient);
			(void)aClient; // For urel aClient is unused
			
			CancelCapture(cap);
			iCaptures->Delete(i);
			RemoveDummyForCapture(cap);
			break;
			}
		}
	}

void CKeyCapturer::CancelCapture(SCapture& cap)
	{
#ifdef FSHELL_WSERV_SUPPORT
	KEYCAPTURE_DEBUG("KC: CancelCapture ident=%d, captureCode=%d, flags=%d", cap.iIdentifier, cap.iCaptureCode, cap.iFlags);
	if (cap.iFlags & ELongCapture)
		{
		iWg.CancelCaptureLongKey(cap.iCaptureHandle);
		TInt bit = cap.iCaptureCode - KSpareKeyRange;
		iUsedSpareKeys &= ~(1<<bit);
		}
	else if (cap.iFlags & ECaptureIsKeyUpDown)
		{
		iWg.CancelCaptureKeyUpAndDowns(cap.iCaptureHandle);
		}
	else
		{
		iWg.CancelCaptureKey(cap.iCaptureHandle);
		}
#endif
	}

CKeyCapturer::~CKeyCapturer()
	{
	Cancel();

	for (TInt i = 0; i < iCaptures->Count(); i++)
		{
		SCapture& cap = iCaptures->operator[](i);
		CancelCapture(cap);
		}
	delete iCaptures;
	iPushList->ResetAndDestroy();
	delete iPushList;
#ifdef FSHELL_WSERV_SUPPORT
	iWg.Close();
	iWs.Close();
#endif
	}

void CKeyCapturer::RunL()
	{
#ifdef FSHELL_WSERV_SUPPORT
	TWsEvent e;
	iWs.GetEvent(e);

	iWs.EventReady(&iStatus);
	SetActive();

	if (e.Type() == EEventKey || e.Type() == EEventKeyDown || e.Type() == EEventKeyUp)
		{
		TInt code = e.Key()->iCode;
		TInt scanCode = e.Key()->iScanCode;
		KEYCAPTURE_DEBUG("KC: Received key event (type=%d, key=%d, scan=%d, rep=%d)", e.Type(), code, scanCode, e.Key()->iRepeats);

		SCapture* foundDummyCapture = NULL;
		
		// Linear search thru iCaptures
		for (TInt i = 0; i < iCaptures->Count(); i++)
			{
			SCapture& cap = iCaptures->operator[](i);
			
			TBool match;
			if (e.Type() == EEventKeyDown || e.Type() == EEventKeyUp)
				{
				match = (scanCode == cap.iCaptureCode) && (cap.iFlags & ECaptureIsKeyUpDown);
				}
			else
				{
				match = (code == cap.iCaptureCode) && !(cap.iFlags & ECaptureIsKeyUpDown);
				}
				
			if (match)
				{
				TBool dummy = cap.iFlags & ECaptureIsDummy;
				if (cap.iClient && !dummy)
					{
					iLastEventScanCode = e.Key()->iScanCode;
					KEYCAPTURE_DEBUG("KC: Sending event ident=%d to client", cap.iIdentifier);
					cap.iClient->CapturedKeyPressedL(cap.iIdentifier);
					iLastEventScanCode = 0;
					}
				if (dummy)
					{
					// If it's a dummy capture tied to a particular long key press, then we want to keep looking just in case there's something else that has genuinely captured the key
					// Also we remember the dummy in case we don't find any other capture, because we need to forward on uncaptured dummies
					foundDummyCapture = &cap;
					}
				else
					{
					foundDummyCapture = NULL;
					break;
					}
				}
			}
		if (foundDummyCapture)
			{
			KEYCAPTURE_DEBUG("KC: Capture is dummy, passing on to frontmost windowgroup");
			TInt foc = iWs.GetFocusWindowGroup();
			if (foc != iWg.Identifier())
				{
				// Send the event on being sure not to pass it back to ourselves!
				iWs.SendEventToWindowGroup(foc, e);
				}
			}
		}
#endif
	}

TInt CKeyCapturer::RunError(TInt /*aError*/)
	{
	return KErrNone; // Don't care if our client left from its call to CapturedKeyPressedL
	}

void CKeyCapturer::DoCancel()
	{
#ifdef FSHELL_WSERV_SUPPORT
	iWs.EventReadyCancel();
#endif
	}

#ifdef __EXE__
	CKeyCapturer* gCapturer = NULL;
#endif

EXPORT_C CKeyCapturer* CKeyCapturer::GetCapturerL()
	{
	CKeyCapturer* ref = NULL;

#ifdef __EXE__
	if (!gCapturer)
		{
		gCapturer = CKeyCapturer::NewLC();
		CleanupStack::Pop();
		}
	ref = gCapturer;
#else
	ref = (CKeyCapturer*)Dll::Tls();
	if (!ref)
		{
		ref = CKeyCapturer::NewLC();
		User::LeaveIfError(Dll::SetTls(ref));
		CleanupStack::Pop(ref);
		}
#endif
	return ref;
	}

EXPORT_C void CKeyCapturer::DeleteCapturer()
	{
#ifdef __EXE__
	delete gCapturer;
	gCapturer = NULL;
#else
	CKeyCapturer* ref = (CKeyCapturer*)Dll::Tls();
	delete ref;
	Dll::SetTls(NULL);
	//Dll::FreeTls();
#endif
	}

CKeyCapturer* CKeyCapturer::NewLC()
	{
	CKeyCapturer* res = new(ELeave) CKeyCapturer;
	CleanupStack::PushL(res);
	res->ConstructL();
	return res;
	}

EXPORT_C void CKeyCapturer::PushCurrentCapturesL()
	{
	CArrayFixFlat<SCapture>* newList = new(ELeave) CArrayFixFlat<SCapture>(8);
	iPushList->AppendL(iCaptures);
	// Now cancel everything in currentList to leave a clean slate
	for (TInt i = 0; i < iCaptures->Count(); i++)
		{
		SCapture& cap = iCaptures->operator[](i);
		CancelCapture(cap);
		}
	iCaptures = newList;
	}

EXPORT_C void CKeyCapturer::PopCapturesL()
	{
	//__ASSERT_ALWAYS(iPushList->Count() >= 1, User::Invariant());
	KEYCAPTURE_DEBUG("KC: Popping captures");

	// Cancel everything in the current list
	for (TInt i = 0; i < iCaptures->Count(); i++)
		{
		SCapture& cap = iCaptures->operator[](i);
		CancelCapture(cap);
		}
	iCaptures->Reset(); // Transfer everything to iCaptures one by one, to allow for the (admittedly unlikely) possibility that any individual capture could fail

	CArrayFixFlat<SCapture>* newList = (*iPushList)[iPushList->Count()-1];
	for (TInt i = newList->Count() - 1; i >= 0 ; i--)
		{
		SCapture& cap = newList->operator[](i);
		AddCaptureL(cap);
		newList->Delete(i); // When it's been sucessfully captured into iCaptures, remove if from newList
		}
	iPushList->Delete(iPushList->Count() - 1);
	delete newList;
	KEYCAPTURE_DEBUG("KC: Finished restoring previous captures");
	}

EXPORT_C TInt CKeyCapturer::IsCaptured(TInt aIdentifier)
	{
	TBool dontCare;
	return IsCaptured(aIdentifier, dontCare);
	}

EXPORT_C TInt CKeyCapturer::IsCaptured(TInt aIdentifier, TBool& aIsLong)
	{
	for (TInt i = 0; i < iCaptures->Count(); i++)
		{
		SCapture& cap = iCaptures->operator[](i);
		if (cap.iIdentifier == aIdentifier && !(cap.iFlags & ECaptureIsDummy))
			{
			aIsLong = cap.iFlags & ELongCapture;
			return cap.iOriginalCode;
			}
		}
	return 0;
	}

EXPORT_C TInt CKeyCapturer::GetScanCodeOfLastEvent()
	{
	return iLastEventScanCode;
	}

void CKeyCapturer::RemoveDummyForCapture(const SCapture& aCapture)
	{
	for (TInt i = 0; i < iCaptures->Count(); i++)
		{
		SCapture cap = iCaptures->operator[](i);
		TUint32 matchFlags = ECaptureIsDummy;
		if (aCapture.iFlags & ELongCapture)
			{
			// Nothing to do - the match flags for a long press are ECaptureIsDummy and that's it
			}
		else
			{
			// A dummy for a short press is an upAndDown one
			matchFlags |= ECaptureIsKeyUpDown;
			}
		if ((cap.iFlags & matchFlags) && (cap.iOriginalCode == aCapture.iOriginalCode))
			{
			CancelCapture(cap);
			iCaptures->Delete(i);
			RemoveDummyForCapture(cap); // Just in case the long press had a dummy (being the short press) which would then ALSO have a dummy (the upanddown)
			break;
			}
		}
	}
