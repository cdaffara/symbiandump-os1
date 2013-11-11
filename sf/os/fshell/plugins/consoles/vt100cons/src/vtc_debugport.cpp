// vtc_debugport.cpp
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32twin.h>
#include <fshell/vtc_base.h>

class CReaderObject;

NONSHARABLE_CLASS(CVtcDebugPortConsole) : public CVtcConsoleBase
	{
public:
	CVtcDebugPortConsole();
	~CVtcDebugPortConsole();
	void ReadComplete(TInt aErr, TKeyCode aCode);

protected: // From CVtcConsoleBase.
	virtual void ConstructL(const TDesC& aTitle);
	TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
private: // From MConsoleOutput.
	virtual TInt Output(const TDesC8& aDes);
private: // From MConsoleInput.
	virtual void Input(TDes8& aDes, TRequestStatus& aStatus);
	virtual void CancelInput(TRequestStatus& aStatus);
private:
	friend class CReaderObject;
	RConsole iConsole;
	CReaderObject* iReader;
	TRequestStatus* iClientStatus;
	TDes8* iClientDes;
	};

class CReaderObject : public CActive
	{
public:
	CReaderObject(CVtcDebugPortConsole* aParent) : CActive(CActive::EPriorityStandard), iParent(aParent) { CActiveScheduler::Add(this); }
	void RunL() { iParent->ReadComplete(iStatus.Int(), iKey.Code()); }
	void DoCancel() { iParent->iConsole.ReadCancel(); }
	void Request()
		{
		iParent->iConsole.Read(iKey, iStatus);
		SetActive();
		}
	
	CVtcDebugPortConsole* iParent;
	TConsoleKey iKey;
	};
	
extern "C" EXPORT_C TAny* NewConsole()
	{
	return new CVtcDebugPortConsole;
	}

CVtcDebugPortConsole::CVtcDebugPortConsole()
	{
	}

CVtcDebugPortConsole::~CVtcDebugPortConsole()
	{
	if (iReader)
		{
		iReader->Cancel();
		delete iReader;
		}
	if (iConsole.Handle())
		{
		iConsole.Destroy(); // Needed?
		}
	iConsole.Close();
	}

void CVtcDebugPortConsole::ConstructL(const TDesC& aTitle)
	{
	iReader = new(ELeave) CReaderObject(this);
	User::LeaveIfError(iConsole.Create());
	User::LeaveIfError(iConsole.Init(_L("fshell"), TSize(KConsFullScreen,KConsFullScreen)));
	CVtcConsoleBase::ConstructL(aTitle, EFalse);
	}
	
TInt CVtcDebugPortConsole::Output(const TDesC8& aDes)
	{
	TBuf<128> buf;
	TPtrC8 remainder(aDes);
	while (remainder.Length())
		{
		TPtrC8 frag(remainder.Left(Min(remainder.Length(), buf.MaxLength())));
		remainder.Set(remainder.Mid(frag.Length()));
		buf.Copy(frag);
		RDebug::RawPrint(buf);
		}
	return KErrNone;
	}

void CVtcDebugPortConsole::Input(TDes8& aDes, TRequestStatus& aStatus)
	{
	iClientDes = &aDes;
	iClientStatus = &aStatus;
	iReader->Request();
	//iCommPort.ReadOneOrMore(aStatus, aDes);
	}

void CVtcDebugPortConsole::CancelInput(TRequestStatus&)
	{
	//iCommPort.ReadCancel();
	iReader->Cancel();
	User::RequestComplete(iClientStatus, KErrCancel);
	}

void CVtcDebugPortConsole::ReadComplete(TInt aErr, TKeyCode aCode)
	{
	iClientDes->SetLength(1);
	(*iClientDes)[0] = aCode;
	User::RequestComplete(iClientStatus, aErr);
	}

TInt CVtcDebugPortConsole::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	if (aExtensionId == ConsoleMode::KSetConsoleModeExtension)
		{
		return KErrNotSupported; // The text windowserver swallows certain keypresses so we can't support binary mode
		}
	else
		{
		return CVtcConsoleBase::Extension_(aExtensionId, a0, a1);
		}
	}
