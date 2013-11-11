// cenrep.cpp
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

#include <fshell/ioutils.h>
#include <centralrepository.h>
#include <fshell/ltkutils.h>
#include <fshell/descriptorutils.h>

using namespace IoUtils;

class CCmdCenrep : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdCenrep();
private:
	CCmdCenrep();
	void PrintKeyL(TUint aKey, RBuf8& valDes);
	template <class KEYTYPE>
	void SetKeyL(const KEYTYPE& aVal);
	void ListRepositoriesL();
	void ForceSetKeyL(TInt aVal);
	void ForceSetKeyL(const TDesC& aVal);
	void SudoL(TRefByValue<const TDesC> aFmt, ...);
	void AddFilesL(RArray<TUint>&, const TDesC& aMatch, const TDesC& aDir);

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	// Arguments
	TUint iRepository;
	TUint iKey;
	// Options
	HBufC* iStringVal;
	TInt iIntVal;
	TBool iForce;
	TBool iDelete;
	TBool iReset;
	CRepository* iRep;
	};

CCommandBase* CCmdCenrep::NewLC()
	{
	CCmdCenrep* self = new(ELeave) CCmdCenrep();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdCenrep::~CCmdCenrep()
	{
	delete iStringVal;
	delete iRep;
	}

CCmdCenrep::CCmdCenrep()
	{
	}

const TDesC& CCmdCenrep::Name() const
	{
	_LIT(KName, "cenrep");	
	return KName;
	}

void CCmdCenrep::DoRunL()
	{
	//__DEBUGGER();
	if (iRepository == 0)
		{
		ListRepositoriesL();
		return;
		}

	TRAPD(err, iRep = CRepository::NewL(TUid::Uid(iRepository)));
	if (err && iForce)
		{
		// force create a repository?
		}
	else
		{
		if (err == KErrNotFound)
			{
			PrintError(err, _L("Repository 0x%x not found"), iRepository);
			}
		User::LeaveIfError(err);
		}

	if (iDelete)
		{
		if (!iArguments.IsPresent(1))
			{
			// Delete all
			if (iForce)
				{
				SudoL(_L("--delete"));
				}
			else
				{
				TUint32 errorKey = 0;
				err = iRep->Delete(0, 0, errorKey);
				if (err && errorKey && errorKey != NCentralRepositoryConstants::KUnspecifiedKey)
					{
					PrintError(err, _L("Delete failed due to key 0x%x"), errorKey);
					}
				}
			}
		else
			{
			if (iForce)
				{
				SudoL(_L("--delete %d"), iKey);
				}
			else
				{
				err = iRep->Delete(iKey);
				}
			}
		if (err == KErrPermissionDenied)
			{
			PrintError(err, _L("Delete failed, retry with --force"));
			}
		User::LeaveIfError(err);
		}
	else if (iReset)
		{
		if (iForce)
			{
			delete iRep;
			iRep = NULL;
			LtkUtils::RLtkBuf persistsName;
			CleanupClosePushL(persistsName);
			persistsName.AppendFormatL(_L("C:\\private\\10202be9\\persists\\%08x.cre"), iRepository);
			err = FsL().Delete(persistsName);
			LeaveIfErr(err, _L("Couldn't delete repository file %S"), &persistsName);
			CleanupStack::PopAndDestroy(&persistsName);
			}
		else
			{
			User::LeaveIfError(iRep->Reset());
			}
		}
	else if (iOptions.IsPresent(&iIntVal))
		{
		// Set int
		SetKeyL(iIntVal);
		}
	else if (iOptions.IsPresent(&iStringVal))
		{
		// Set string
		//TPtrC8 data((TUint8*)iStringVal->Ptr(), iStringVal->Size());
		//SetKeyL(data);
		SetKeyL(*iStringVal);
		}
	else if (!iArguments.IsPresent(1))
		{
		// Get all
		if (iForce)
			{
			SudoL(KNullDesC);
			}
		else
			{
			RBuf8 valDes;
			CleanupClosePushL(valDes);
			valDes.CreateL(NCentralRepositoryConstants::KMaxBinaryLength);

			RArray<TUint32> keys;
			CleanupClosePushL(keys);
			iRep->FindL(0, 0, keys);
			for (TInt i = 0; i < keys.Count(); i++)
				{
				PrintKeyL(keys[i], valDes);
				}
			CleanupStack::PopAndDestroy(2, &valDes); // keys, valDes
			}
		}
	else
		{
		// Get
		RBuf8 valDes;
		CleanupClosePushL(valDes);
		valDes.CreateL(NCentralRepositoryConstants::KMaxBinaryLength * 3); // Because we also use this for hexifying
		PrintKeyL(iKey, valDes);
		CleanupStack::PopAndDestroy(&valDes);
		}
	}

template <class KEYTYPE>
void CCmdCenrep::SetKeyL(const KEYTYPE& aVal)
	{
	if (!iArguments.IsPresent(1))
		{
		LeaveIfErr(KErrArgument, _L("You must specify a key to set"));
		}
	if (iForce)
		{
		ForceSetKeyL(aVal);
		}
	else
		{
		TInt err = iRep->Set(iKey, aVal);
		if (err == KErrArgument)
			{
			PrintError(err, _L("Key does not appear to be of the right type"));
			}
		User::LeaveIfError(err);
		}
	}

void CCmdCenrep::ForceSetKeyL(TInt aVal)
	{
	SudoL(_L("%d --set-int %d"), iKey, aVal);
	}

void CCmdCenrep::ForceSetKeyL(const TDesC& aVal)
	{
	SudoL(_L("%d --set-string '%S'"), iKey, &aVal);
	}

void CCmdCenrep::PrintKeyL(TUint aKey, RBuf8& valDes)
	{
	TInt valInt;
	valDes.Zero();
	enum TType { EUnknown, EInt, EDes };
	TType type = EUnknown;
	TInt reallen = 0;

	TInt err = KErrNotFound;

	if (iForce)
		{
		SudoL(_L("%d"), aKey);
		}
	else
		{
		// Guess the value type
		// Int?
		if (type == EUnknown)
			{
			err = iRep->Get(aKey, valInt);
			if (err != KErrArgument)
				{
				type = EInt;
				}
			}
		
		if (type == EUnknown)
			{
			// Des?
			err = iRep->Get(aKey, valDes, reallen);
			if (err != KErrArgument)
				{
				type = EDes;
				}
			}
		}

	switch(err)
		{
		case KErrNotFound:
			PrintError(err, _L("Key not found"));
			break;
		case KErrArgument:
			PrintError(err, _L("Unknown key type, not int, des8 or des16"));
			break;
		case KErrPermissionDenied:
			PrintError(err, _L("Permission denied, retry with --force"));
			break;
		/* Won't happen because we now set our buffers to be the max size cenrep can handle
		case KErrOverflow:
			valDes16.ReallocL(reallen);
			valDes8.ReallocL(reallen);
			if (type == EDes8)
				{
				err = rep->Get(iKey, valDes8, reallen); // Better not fail this time!
				}
			else
				{
				err = rep->Get(iKey, valDes16, reallen); // Better not fail this time!
				}
		*/
		case KErrNone:
			// do nothing
			break;
		default:
			PrintError(err, _L("Unrecognised error returned from CRepository"));
			break;
		}

	if (err == KErrNone)
		{
		switch (type)
			{
			case EInt:
				Printf(_L("Key 0x%08x TInt: %d (0x%x)\r\n"), aKey, valInt, valInt);
				break;
			/*case EDes8:
				valDes16.Copy(valDes8);
				Printf(_L("Key 0x%x TDesC8 length=%d: %S\r\n"), aKey, valDes16.Length(), &valDes16);
				break;*/
			case EDes:
				{
				TInt len = valDes.Length();

				// Figure out if this string is likely to be 16bit ASCII, 8bit ASCII or just binary data
				TBool wide = ETrue;
				TBool ascii = ETrue;
				if (len&1) wide = EFalse; // Odd number of bytes can't be 16-bit
				for (TInt i = 0; i < len; i++)
					{
					TChar c(valDes[i]);
					if ((i&1) && valDes[i] != 0) wide = EFalse;
					if (!(c.IsPrint() && c < 127))
						{
						if (!(i&1)) wide = EFalse;
						ascii = EFalse;
						}
					}

				if (wide)
					{
					TPtr16 widePtr((TUint16*)valDes.Ptr(), len/2, valDes.MaxLength()/2);
					Printf(_L("Key 0x%08x TDesC16 length=%d: %S\r\n"), aKey, widePtr.Length(), &widePtr);
					}
				else if (ascii)
					{
					Printf(_L8("Key 0x%08x TDesC8  length=%d: %S\r\n"), aKey, len, &valDes);
					}
				else
					{
					Printf(_L("Key 0x%08x TDesC8  hex dump:\r\n"), aKey);
					LtkUtils::HexDumpToOutput(valDes, Stdout());
					}
				}
				break;
			default:
				break;
			}
		}
	}

void CCmdCenrep::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendUintL(iRepository, _L("repository_uid"));
	aArguments.AppendUintL(iKey, _L("key"));
 	}

void CCmdCenrep::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendIntL(iIntVal, _L("set-int"));
	aOptions.AppendStringL(iStringVal, _L("set-string"));
	aOptions.AppendBoolL(iForce, _L("force"));
	aOptions.AppendBoolL(iDelete, _L("delete"));
	aOptions.AppendBoolL(iReset, _L("reset"));
	}

EXE_BOILER_PLATE(CCmdCenrep)

void CCmdCenrep::ListRepositoriesL()
	{
	RArray<TUint> uids;
	CleanupClosePushL(uids);
	AddFilesL(uids, _L("*.cre"), _L("Y:\\private\\10202be9\\persists\\"));
	AddFilesL(uids, _L("*.cre"), _L("Y:\\private\\10202be9\\"));
	AddFilesL(uids, _L("*.txt"), _L("Y:\\private\\10202be9\\"));

	for (TInt i = 0; i < uids.Count(); i++)
		{
		Printf(_L("%08x\r\n"), uids[i]);
		}
	CleanupStack::PopAndDestroy(&uids);
	}

void CCmdCenrep::AddFilesL(RArray<TUint>& aUids, const TDesC& aMatch, const TDesC& aDir)
	{
	TFindFile finder(FsL());
	CDir* files = NULL;
	TInt err = finder.FindWildByDir(aMatch, aDir, files);

	while (err == KErrNone)
		{
		for (TInt i = 0; i < files->Count(); i++)
			{
			TUint uid;
			TLex lex((*files)[i].iName);
			err = lex.Val(uid, EHex);
			if (!err)
				{
				err = aUids.InsertInOrder(uid);
				if (err == KErrAlreadyExists) err = KErrNone;
				}
			}
		delete files;
		files = NULL;
		err = finder.FindWild(files);
		}

	if (err && err != KErrNotFound) LeaveIfErr(err, _L("Couldn't read directory %S"), &aDir);
	}

void CCmdCenrep::SudoL(TRefByValue<const TDesC> aFmt, ...)
	{
	VA_LIST args;
	VA_START(args, aFmt);
	
	// We use sudo to give ourselves the SID of the repository, in the hope that will confer us more rights to access the data.
	// Some repos might be configured to deny access even from their own SID; there's nothing we can do in that case
	TBuf<256> commandLine;
	commandLine.AppendFormat(_L("--sid 0x%x cenrep.exe 0x%x "), iRepository, iRepository);
	commandLine.AppendFormatList(aFmt, args);
	VA_END(args);

	RChildProcess sudo;
	CleanupClosePushL(sudo);
	TRAPL(sudo.CreateL(_L("sudo.exe"), commandLine, IoSession(), Stdin(), Stdout(), Stderr()), _L("Couldn't create sudo process"));
	TRequestStatus logon;
	sudo.Run(logon);
	User::WaitForRequest(logon);
	LeaveIfErr(logon.Int(), _L("Error running sudo %S"), &commandLine);
	CleanupStack::PopAndDestroy(&sudo);
	}
