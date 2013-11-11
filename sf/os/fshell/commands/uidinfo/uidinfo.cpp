// uidinfo.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <apgcli.h>
#include <fshell/ltkutils.h>
#include <fshell/qr3dll.h>

using namespace IoUtils;

class CCmdUidInfo : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdUidInfo();
private:
	CCmdUidInfo();
	TInt PrintUid(TUint aUid);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TUint iUid;
	TBool iStdin;
	RBuf iBuf;
	};

CCommandBase* CCmdUidInfo::NewLC()
	{
	CCmdUidInfo* self = new(ELeave) CCmdUidInfo();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdUidInfo::~CCmdUidInfo()
	{
	iBuf.Close();
	}

CCmdUidInfo::CCmdUidInfo()
	{
	}

const TDesC& CCmdUidInfo::Name() const
	{
	_LIT(KName, "uidinfo");	
	return KName;
	}

void CCmdUidInfo::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendUintL(iUid, _L("uid"));
	}

void CCmdUidInfo::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iStdin, _L("stdin"));
	}


EXE_BOILER_PLATE(CCmdUidInfo)

void CCmdUidInfo::DoRunL()
	{
	if (iStdin)
		{
		Stdin().SetReadModeL(RIoReadHandle::ELine);
		TBuf<512> lineBuf; // Note, the code below doesn't properly handle the situation where a given line of input in longer than this buffer.
		while (ETrue)
			{
			TInt err = Stdin().Read(lineBuf);
			if (err == KErrEof)
				{
				break;
				}
			else if (err)
				{
				User::Leave(err);
				}
			if (lineBuf.Length() < 2) continue;
			// In this case we assume hex even without an 0x
			if (lineBuf.Left(2).CompareF(_L("0x")) != 0) lineBuf.Insert(0, _L("0x"));
			TLex lex(lineBuf);
			TUint uid = 0;
			TRAP(err, uid = LtkUtils::HexLexL(lex));
			if (err)
				{
				PrintWarning(_L("Failed to find a uid on line '%S'"), &lineBuf);
				continue;
				}
			err = PrintUid(uid);
			if (err)
				{
				PrintWarning(_L("Couldn't find exe for uid 0x%08x, err = %d\r\n"), uid, err);
				}
			}
		}
	else if (!iArguments.IsPresent(&iUid))
		{
		LeaveIfErr(KErrArgument, _L("You must specify either a uid or the --stdin option."));
		}
	else
		{
		LeaveIfErr(PrintUid(iUid), _L("Couldn't find exe for uid 0x%08x"), iUid);
		}
	}

TInt CCmdUidInfo::PrintUid(TUint aUid)
	{
	TInt err = ExeNameForSid(aUid, iBuf);
	if (err) return err;
	Printf(_L("0x%x %S\r\n"), aUid, &iBuf);
	return KErrNone;
	}
