// ltkutils.h
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
#ifndef FSHELL_UTILS
#define FSHELL_UTILS

#include <e32base.h>
class RIoWriteHandle;
class RFs;

namespace LtkUtils
	{
	/* This function makes the object allocated at aPtr be ignored by __UHEAP_MARK/__UHEAP_MARK_END etc
	 * This is useful for objects whose lifetime is carefully managed (ie they really aren't leaked) but have
	 * no suitable place to be cleaned up. Objects stored in TLS are a good example.
	 * Has no effect if run against a UREL euser.dll
	 */
	IMPORT_C void MakeHeapCellInvisible(TAny* aPtr);

	IMPORT_C TInt InjectRawKeyEvent(TInt aScanCode, TInt aModifiers, TInt aRepeatCount);

	IMPORT_C HBufC* HexDumpL(const TDesC8& aBuf);
	IMPORT_C void HexDumpToOutput(const TDesC8& aBuf, RIoWriteHandle& aHandle);
	IMPORT_C void HexDumpToOutput(const TDesC8& aBuf, RIoWriteHandle& aHandle, TInt& aOffset);
	IMPORT_C void HexDumpToRDebug(const TDesC8& aBuf);
	IMPORT_C void HexDumpToRDebug(const TDesC8& aBuf, TInt& aOffset);
	IMPORT_C void RawPrint(const TDesC8& aDes); // 9.1 doesn't have the 8-bit overload

	IMPORT_C void GetFriendlyThreadName(RThread aThread, TFullName& aFriendly);
	IMPORT_C void MakeThreadNameFriendly(TFullName& aFullThreadName);
	IMPORT_C void GetFriendlyProcessName(const RProcess& aProcess, TDes& aFriendly);
	IMPORT_C void MakeProcessNameFriendly(TDes& aFullProcessName);

	IMPORT_C TUint HexLexL(TLex16& aLex);
	IMPORT_C TInt HexLex(TLex16& aLex, TUint& aResult);
	IMPORT_C TUint HexLexL(TLex8& aLex);
	IMPORT_C TInt HexLex(TLex8& aLex, TUint& aResult);

	IMPORT_C TBool HasPrefix(const TDesC16& aDes, const TDesC16& aPrefix);
	IMPORT_C TBool HasPrefix(const TDesC8& aDes, const TDesC8& aPrefix);

	/* Replace all occurences of aFrom with aTo, in aDescriptor. Returns number of replacements,
	 * or KErrOverflow if aDescriptor was not big enough to hold all the replacements
	 */
	IMPORT_C TInt ReplaceText(TDes& aDescriptor, const TDesC& aFrom, const TDesC& aTo);

	IMPORT_C HBufC8* Utf8L(const TDesC& aString); // For more control see LtkUtils::RLtkBuf8::CopyAsUtf8L()
	IMPORT_C HBufC16* DecodeUtf8L(const TDesC8& aUtf8EncodedText); // For more control see LtkUtils::RLtkBuf16::AppendUtf8L()

	IMPORT_C void FormatSize(TDes16& aBuf, TInt64 aSizeInBytes);
	IMPORT_C void FormatSize(TDes8& aBuf, TInt64 aSizeInBytes);

	/* Some cleanup stack helper functions (defined in ltkutils.inl)
	 */
	template <class T>
	inline void CleanupResetAndDestroyPushL(T& aRef);
	template <class T>
	inline void CleanupResetPushL(T& aRef);


	IMPORT_C char GetSystemDrive(); // 9.1 doesn't have RFs::GetSystemDrive()

	/****
	 * The functions below are not available if you're linking against ltkutils-tcb.dll. They
	 * rely on being able to link against All -TCB dlls.
	 ****/

	/* W32CrackL loads our btrace-compatible wserv logging engine, dlogev.dll, into the existing wserv process. 
	 * This enables Autometric to capture i/o events like key & pointer presses, and text drawn to screen.
	 * It also enables btrace logging of windowgroup names. Does nothing if dlogev is already loaded.
	 */
	const TUid KW32CrackCategory = { 0x10003B20 };
	const TInt KW32CrackKey = 0x102864C5;
	IMPORT_C void W32CrackL();
	IMPORT_C TBool W32CrackIsEnabled();

	/* These functions convert a resource identifier (of the format given in the rez.exe docs) into
	 * the localised equivalent.
	 */
	IMPORT_C HBufC* RezL(const TDesC& aIdentifier, RFs* aFs = NULL, HBufC** aError=NULL);
	IMPORT_C HBufC* RezLC(const TDesC& aIdentifier, RFs* aFs = NULL, HBufC** aError=NULL);
	IMPORT_C HBufC8* Rez8L(const TDesC& aIdentifier, RFs* aFs = NULL, HBufC** aError=NULL);
	IMPORT_C HBufC8* Rez8LC(const TDesC& aIdentifier, RFs* aFs = NULL, HBufC** aError=NULL);

	IMPORT_C void CopyToClipboardL(const TDesC& aText, RFs* aFs = NULL);
	IMPORT_C HBufC* GetFromClipboardL(RFs* aFs = NULL);

	
	/* Programatically cause a breakpoint to occur. On emulator this translates to a call to __BREAKPOINT,
	 * on target this will break if fdb is installed, otherwise will crash the thread.
	 */
	IMPORT_C void Breakpoint();

	IMPORT_C void BreakpointPushL(); // Push a TCleanupItem onto the CleanupStack that calls LtkUtils::Breakpoint() if a leave occurs
	IMPORT_C TLinAddr BreakpointAddr(); // Internal helper function, only for use by fdb.exe
	}

#include <fshell/ltkutils.inl>

#endif
