// friendlynames.cpp
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

#include <fshell/ltkutils.h>

EXPORT_C void LtkUtils::GetFriendlyThreadName(RThread aThread, TFullName& aFriendly)
	{
	aFriendly = aThread.FullName();
	MakeThreadNameFriendly(aFriendly);
	}
		
_LIT(KDoubleColon, "::");
_LIT(KMain, "main");
_LIT(KExtExe, ".exe");

const static TInt KProcessUidLength = 8;
const static TInt KProcessSequenceLength = 4;
const static TInt KProcessSuffixLength = 2+KProcessUidLength+KProcessSequenceLength; // including []
	
EXPORT_C void LtkUtils::MakeThreadNameFriendly(TFullName& aFullThreadName)
	{
	TInt colonPos = aFullThreadName.Find(KDoubleColon);
	TBool removeThreadName = EFalse;
	TBool haveExeExt = EFalse;
	
	if (colonPos==KErrNotFound) return;
	if (colonPos < KProcessSuffixLength) return;
	if (aFullThreadName[colonPos-KProcessSuffixLength] != '[') return;
	if (aFullThreadName[colonPos-KProcessSequenceLength-1] != ']') return;
	
	TPtrC sequenceNoStr(aFullThreadName.Mid(colonPos-KProcessSequenceLength, KProcessSequenceLength));
	TLex lex(sequenceNoStr);
	TInt sequenceNo;
	if (lex.Val(sequenceNo)!=KErrNone) return;
	if (lex.Remainder().Length()!=0) return;
	
	// see if the thread name is 'main'
	TPtrC threadName(aFullThreadName.Mid(colonPos+2));
	if (threadName.CompareF(KMain)==0)
		{
		removeThreadName = ETrue;
		}
		
	// see if we have a .exe extension on the process
	TPtrC processName(aFullThreadName.Left(colonPos - KProcessSuffixLength));
	if (processName.Right(KExtExe().Length()).CompareF(KExtExe)==0)
		{
		haveExeExt = ETrue;
		processName.Set(processName.Left(processName.Length() - KExtExe().Length()));
		}
	if (processName.CompareF(threadName)==0)
		{
		removeThreadName = ETrue;
		}
	
	// we have now finished examining the name - remove from things from it
	// starting from the end, so that we don't screw up the indices we're 
	// about to use.
	
	if (removeThreadName)
		{
		aFullThreadName.SetLength(colonPos); // remove :: and thread name
		}

	// delete the process suffix
	aFullThreadName.Delete(colonPos-KProcessSuffixLength, KProcessSuffixLength);
	if (sequenceNo>1)
		{
		TBuf<7> sequenceStr;
		sequenceStr.AppendFormat(_L("(%d)"), sequenceNo);
		aFullThreadName.Insert(colonPos-KProcessSuffixLength, sequenceStr);
		}
	
	if (haveExeExt)
		{
		aFullThreadName.Delete(colonPos-KProcessSuffixLength-KExtExe().Length(), KExtExe().Length());
		}
	}

EXPORT_C void LtkUtils::GetFriendlyProcessName(const RProcess& aProcess, TDes& aFriendly)
	{
	aFriendly = aProcess.FullName();
	MakeProcessNameFriendly(aFriendly);
	}


EXPORT_C void LtkUtils::MakeProcessNameFriendly(TDes& aFullProcessName)
	{
	TInt colonPos = aFullProcessName.Length();
	TBool haveExeExt = EFalse;
	
	if (colonPos==KErrNotFound) return;
	if (colonPos < KProcessSuffixLength) return;
	if (aFullProcessName[colonPos-KProcessSuffixLength] != '[') return;
	if (aFullProcessName[colonPos-KProcessSequenceLength-1] != ']') return;
	
	TPtrC sequenceNoStr(aFullProcessName.Mid(colonPos-KProcessSequenceLength, KProcessSequenceLength));
	TLex lex(sequenceNoStr);
	TInt sequenceNo;
	if (lex.Val(sequenceNo)!=KErrNone) return;
	if (lex.Remainder().Length()!=0) return;
	
	// see if we have a .exe extension on the process
	TPtrC processName(aFullProcessName.Left(colonPos - KProcessSuffixLength));
	if (processName.Right(KExtExe().Length()).CompareF(KExtExe)==0)
		{
		haveExeExt = ETrue;
		processName.Set(processName.Left(processName.Length() - KExtExe().Length()));
		}
	
	// we have now finished examining the name - remove from things from it
	// starting from the end, so that we don't screw up the indices we're 
	// about to use.
	
	// delete the process suffix
	aFullProcessName.Delete(colonPos-KProcessSuffixLength, KProcessSuffixLength);
	if (sequenceNo>1)
		{
		TBuf<7> sequenceStr;
		sequenceStr.AppendFormat(_L("(%d)"), sequenceNo);
		aFullProcessName.Insert(colonPos-KProcessSuffixLength, sequenceStr);
		}
	
	if (haveExeExt)
		{
		aFullProcessName.Delete(colonPos-KProcessSuffixLength-KExtExe().Length(), KExtExe().Length());
		}
	}
