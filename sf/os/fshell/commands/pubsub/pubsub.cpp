// pubsub.cpp
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

#include <fshell/common.mmh>
#include <fshell/memoryaccesscmd.h>

#ifdef FSHELL_TRACE_SUPPORT
#include <fshell/extrabtrace.h>
#include <fshell/btrace_parser.h>
#endif

#include <fshell/ltkutils.h>
#include <fshell/qr3dll.h>

using namespace IoUtils;

#ifdef FSHELL_TRACE_SUPPORT
class CCmdPubsub : public CMemoryAccessCommandBase, public MBtracePubSubObserver
	{
private: // From MBtracePubSubObserver
	void HandlePropertyChangedL(const TBtraceTickCount& aTickCount, TUint aCategory, TUint aKey, TInt aNewValue);
	void HandlePropertyChangedL(const TBtraceTickCount& aTickCount, TUint aCategory, TUint aKey, const TDesC8& aNewValue);

#else
class CCmdPubsub : public CMemoryAccessCommandBase
	{
#endif
public:
	static CCommandBase* NewLC();
	~CCmdPubsub();

private:
	CCmdPubsub();
	void PrintKey(TUint aCategory, TUint aKey, TBool aFull=EFalse);
	template <class KEYTYPE>
	void SetKeyL(const KEYTYPE& aVal);
	static TInt PropertyChanged(TAny* aSelf);
	void GetAllL(TBool aNotify);
	TBool IsExcluded(TUint aCat, TUint aKey) const;

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	void DoCancel();
	void RunL();

private:
	// Arguments
	enum TCmd
		{
		EGet, ESet, EDefine, ENotify
		};
	TCmd iCommand;
	TUid iCategory;
	TUint iKey;
	// Options
	HBufC* iStringVal;
	TInt iIntVal;
	TBool iForce;
	TBool iRaw;

	TBool iNotify;
	TBool iDefine;

	// Other assorted stuff
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	TPropNotifyResult iNotifyResult;
#endif
#ifdef FSHELL_TRACE_SUPPORT
	CBtraceReader* iReader;
	CBtracePubSub* iPubSub;
#endif
	TBool iUseBtrace;
	CPropertyManager* iPropertyManager;
	RBuf8 iValDescription;
	};

CCommandBase* CCmdPubsub::NewLC()
	{
	CCmdPubsub* self = new(ELeave) CCmdPubsub();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdPubsub::~CCmdPubsub()
	{
	Cancel();
	delete iStringVal;
#ifdef FSHELL_TRACE_SUPPORT
	delete iPubSub;
	delete iReader;
#endif
	delete iPropertyManager;
	iValDescription.Close();
	}

CCmdPubsub::CCmdPubsub()
	: CMemoryAccessCommandBase(EManualComplete)
	{
	}

const TDesC& CCmdPubsub::Name() const
	{
	_LIT(KName, "pubsub");	
	return KName;
	}

void CCmdPubsub::DoRunL()
	{
#if defined (FSHELL_MEMORY_ACCESS_SUPPORT)
	if (iForce || iUseBtrace)
		{
		// In order to implement the --force option, we talk to the memaccess device driver, which uses an RPropertyRef
		// that isn't subject to security checks
		LoadMemoryAccessL();
		}
#endif

	iValDescription.CreateL(RProperty::KMaxLargePropertySize);

	/*
	if (iDelete)
		{
		if (!iArgList->IsPresent(1))
			{
			// Delete all not supported for pubsub
			PrintError(KErrArgument, _L("You must specify a key for the delete option"));
			User::Leave(KErrArgument);
			}
		else
			{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
			if (iForce)
				{
				err = iMemAccess.DeleteProperty(iCategory, iKey);
				}
			else
#endif
				{
				err = RProperty::Delete(iCategory, iKey);
				}
			}
		if (err == KErrPermissionDenied)
			{
			PrintError(err, _L("Delete failed, retry with --force"));
			}
		User::LeaveIfError(err);
		}
	*/
	if (iCommand == EDefine) iDefine = ETrue;
	if (iCommand == ENotify) iNotify = ETrue;

	if ((iCommand == ESet) || iDefine)
		{
		if (iOptions.IsPresent(&iIntVal))
			{
			// Set int
			SetKeyL(iIntVal);
			}
		else if (iOptions.IsPresent(&iStringVal))
			{
			// Set string
			TPtrC8 data((TUint8*)iStringVal->Ptr(), iStringVal->Size());
			SetKeyL(data);
			}
		else
			{
			LeaveIfErr(KErrArgument, _L("set/define command requires --int or --string arguments"));
			}
		}
	else if ((iCommand == EGet) || iNotify)
		{
		// Get
		if (!iArguments.IsPresent(1))
			LeaveIfErr(KErrArgument, _L("get command requires key and category"));
		PrintKey(iCategory.iUid, iKey, ETrue);

		if (iNotify)
			{
#if defined (FSHELL_MEMORY_ACCESS_SUPPORT)
			if (iForce || iUseBtrace)
				{
				//TPckgBuf<SKeyPair> key;
				//key().iCat = iCategory.iUid;
				//key().iKey = iKey;
				//LeaveIfErr(iMemAccess.SetupMassPropertyNotify(key), _L("Couldn't set up notification"));
				LeaveIfErr(iMemAccess.SubscribeToProperty(iCategory, iKey, iUseBtrace), _L("Couldn't subscribe to property"));
#ifdef FSHELL_TRACE_SUPPORT
				if (iUseBtrace)
					{
					const TInt KFlushInterval = 1000000; // 1s
					const TInt KBtraceBufferSize = 1 * 1024 * 1024;
					TRAPL(iReader = CBtraceReader::NewL(CBtraceReader::EFlushOnBtraceThreshold, KBtraceBufferSize, KBtraceBufferSize / 2), _L("Couldn't create btrace reader"));
					TRAPL(iPubSub = CBtracePubSub::NewL(*iReader), _L("Couldn't create CBtracePubSub"));
					TBtraceTickCount now;
					now.SetToNow();
					iReader->Start(now, KFlushInterval);
					iPubSub->NotifyPropertyChangedL(*this);
					return; // iPubSub will take it from here
					}
				else
#endif
					{
					iMemAccess.NotifyPropertyChange(iNotifyResult, iStatus);
					SetActive();
					}
				}
			else
#endif
				{
				iPropertyManager = CPropertyManager::NewL(TCallBack(&PropertyChanged, this));
				iPropertyManager->ChangeProperty(iCategory.iUid, iKey);
				}
			return; // Don't complete
			}
		}
	Complete();
	}

template <class KEYTYPE>
void CCmdPubsub::SetKeyL(const KEYTYPE& aVal)
	{
	if (!iArguments.IsPresent(1))
		{
		LeaveIfErr(KErrArgument, _L("You must specify a key to set"));
		}
	TInt err = KErrNone;
#if defined (FSHELL_MEMORY_ACCESS_SUPPORT)
	if (iForce)
		{
		err = iMemAccess.SetProperty(iCategory, iKey, aVal);
		}
	else
#endif
		{
		err = RProperty::Set(iCategory, iKey, aVal);
		if (err == KErrArgument)
			{
			LeaveIfErr(err, _L("Key does not appear to be of the right type"));
			}
		}

#if defined (FSHELL_MEMORY_ACCESS_SUPPORT)
	if (err == KErrNotFound && iDefine)
		{
		if (!iForce)
			{
			LeaveIfErr(err, _L("Key does not exist, cannot define it unless --force is also specified"));
			}
		err = iMemAccess.SetProperty(iCategory, iKey, aVal, ETrue);
		}
#endif
	if (err)
		{
		LeaveIfErr(err, _L("Error setting key"));
		}
	}

void CCmdPubsub::PrintKey(TUint aCategory, TUint aKey, TBool aFull)
	{
	TUid cat = { aCategory };
	TInt valInt;
	RBuf8& valDes = iValDescription;
	valDes.Zero();
	enum TType { EUnknown, EInt, EDes };
	TType type = EUnknown;
	TInt reallen = 0;

	TInt err = KErrNotFound;

#if defined (FSHELL_MEMORY_ACCESS_SUPPORT)
	if (iForce)
		{
		err = iMemAccess.GetProperty(cat, aKey, valInt);
		if (err == KErrNone)
			{
			type = EInt;
			}
		else
			{
			err = iMemAccess.GetProperty(cat, aKey, valDes, reallen);
			type = EDes;
			}
		}
	else
#endif
		{
		// Guess the value type
		// Int?
		if (type == EUnknown)
			{
			err = RProperty::Get(cat, aKey, valInt);
			if (err != KErrArgument)
				{
				type = EInt;
				}
			}
		
		if (type == EUnknown)
			{
			// Des?
			err = RProperty::Get(cat, aKey, valDes);
			if (err != KErrArgument)
				{
				type = EDes;
				}
			}
		}

	switch(err)
		{
		case KErrNotFound:
			PrintError(err, _L("Key 0x%x 0x%x not found"), aCategory, aKey);
			break;
		case KErrArgument:
			PrintError(err, _L("Unknown key type, not int, des8 or des16"));
			break;
		case KErrPermissionDenied:
			PrintError(err, _L("Permission denied on 0x%x 0x%x, retry with --force"), aCategory, aKey);
			break;
		case KErrNone:
			// do nothing
			break;
		default:
			PrintError(err, _L("Unrecognised error returned from RProperty"));
			break;
		}

	if (err == KErrNone)
		{
		switch (type)
			{
			case EInt:
				if (iRaw)
					{
					Printf(_L("%d"), valInt);
					}
				else
					{
					Printf(_L("0x%08x 0x%08x TInt: %d (0x%x)\r\n"), aCategory, aKey, valInt, valInt);
					}
				break;
			case EDes:
				{
				if (iRaw)
					{
					Printf(_L8("%S"), &valDes);
					}
				else
					{
					TPtrC8 des(valDes);
					if (!aFull) des.Set(valDes.Left(32)); // Don't print the whole thing, only 2 lines max
					Printf(_L("0x%08x 0x%08x TDesC8 hex dump:\r\n"), aCategory, aKey);
					LtkUtils::HexDumpToOutput(des, Stdout());
					if (des.Length() < valDes.Length()) Write(_L("...\r\n"));
					}
				break;
				}
			default:
				break;
			}
		}
	}
	
void CCmdPubsub::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iCommand, _L("command"));
	aArguments.AppendUintL((TUint&)iCategory.iUid, _L("category"));
	aArguments.AppendUintL(iKey, _L("key"));
	}

void CCmdPubsub::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendIntL(iIntVal, _L("int"));
	aOptions.AppendStringL(iStringVal, _L("string"));
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	aOptions.AppendBoolL(iForce, _L("force"));
#endif
#ifdef FSHELL_TRACE_SUPPORT
	aOptions.AppendBoolL(iUseBtrace, _L("btrace"));
#endif
	aOptions.AppendBoolL(iRaw, _L("raw"));
	//aOptions.AppendStringL(iStringVal, '8', _L("set-data"), _L("Sets the specified key to this 8-bit value"));
	//aOptions.AppendBoolL(iDelete, 'l', _L("delete"), _L("Deletes the specified property"));
	}

TInt CCmdPubsub::PropertyChanged(TAny* aSelf)
	{
	CCmdPubsub* self = static_cast<CCmdPubsub*>(aSelf);
	self->PrintKey(self->iCategory.iUid, self->iKey);
	return 0;
	}

EXE_BOILER_PLATE(CCmdPubsub)

void CCmdPubsub::DoCancel()
	{
#if defined (FSHELL_MEMORY_ACCESS_SUPPORT)
	iMemAccess.CancelPropertyChange();
#endif
	}

void CCmdPubsub::RunL()
	{
#if defined (FSHELL_MEMORY_ACCESS_SUPPORT)

	TPropNotifyResult notifyResult = iNotifyResult;
	// rerequest asap
	if (iStatus == KErrNone)
		{
		iMemAccess.NotifyPropertyChange(iNotifyResult, iStatus);
		SetActive();
		}

	if (notifyResult.iMissedChanges)
		{
		PrintWarning(_L("Missed %d publish and subscribe notifications"), notifyResult.iMissedChanges);
		}

	if (notifyResult.iError)
		{
		Printf(_L("NotifyChange for 0x%08x 0x%x completed with error %d\r\n"), notifyResult.iCategory, notifyResult.iKey, notifyResult.iError);
		}
	else
		{
		//Printf(_L("P&S Key changed:\r\n"));
		PrintKey(notifyResult.iCategory, notifyResult.iKey);
		}
#endif
	}

#ifdef FSHELL_TRACE_SUPPORT
void CCmdPubsub::HandlePropertyChangedL(const TBtraceTickCount&, TUint aCategory, TUint aKey, TInt /*aNewValue*/)
	{
	PrintKey(aCategory, aKey);
	}

void CCmdPubsub::HandlePropertyChangedL(const TBtraceTickCount&, TUint aCategory, TUint aKey, const TDesC8& /*aNewValue*/)
	{
	PrintKey(aCategory, aKey);
	}
#endif
