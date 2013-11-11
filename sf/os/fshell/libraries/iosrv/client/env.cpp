// env.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "ioutils.h"
#include "command_base.h"
#include <fshell/stringhash.h>
#include <fshell/ltkutils.h>
#include <fshell/descriptorutils.h>

using namespace IoUtils;
using LtkUtils::RStringHash;
using LtkUtils::TStringHashIter;

#define iVars (*reinterpret_cast<RStringHash<HBufC*>*>(iVarsImpl))
#define ConstVarsFor(x) (*reinterpret_cast<const RStringHash<HBufC*>*>((x)->iVarsImpl))
#define iConstVars ConstVarsFor(this)

//
// Constants.
//

_LIT(KEnvPwd, "PWD");
_LIT(KDefaultPwd, "c:\\");
_LIT(KChildError, "?");
_LIT(KEnvEscapeChar, "ESCAPE");

//
// CEnvironment.
//

EXPORT_C CEnvironment* CEnvironment::NewL()
	{
	CEnvironment* self = new(ELeave) CEnvironment();
	CleanupStack::PushL(self);
	self->ConstructL();
	self->SetPwdL(KDefaultPwd);
	self->SetL(KChildError, KErrNone);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CEnvironment* CEnvironment::NewL(const CEnvironment& aEnv)
	{
	CEnvironment* self = CEnvironment::NewL();
	CleanupStack::PushL(self);
	self->CopyL(aEnv);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CEnvironment* CEnvironment::CreateSharedEnvironmentL()
	{
	CEnvironment* newenv = new(ELeave) CEnvironment(this);
	CleanupStack::PushL(newenv);
	newenv->ConstructL();
	CleanupStack::Pop(newenv);
	return newenv;
	}

EXPORT_C CEnvironment::~CEnvironment()
	{
	RemoveAll();
	iLock.Close();
	}

EXPORT_C void CEnvironment::RemoveAll()
	{
	// Note this doesn't affect iParentEnv
	if (iLock.Handle())
		{
		iLock.Wait();
		TStringHashIter<HBufC*> iter(iVars);
		while (iter.NextValue() != NULL)
			{
			delete *iter.CurrentValue();
			}
		iVars.Close();
		iLock.Signal();
		}
	}

CEnvironment::CEnvironment()
	{
	__ASSERT_COMPILE(sizeof(iVarsImpl) == sizeof(RStringHash<HBufC*>));
	// Have to placement new iVars because we hide the type
	new(iVarsImpl) RStringHash<HBufC*>;
	}

CEnvironment::CEnvironment(CEnvironment* aParentEnv)
	: iParentEnv(aParentEnv)
	{
	new(iVarsImpl) RStringHash<HBufC*>;
	}

void CEnvironment::ConstructL()
	{
	User::LeaveIfError(iLock.CreateLocal());
	}

void CEnvironment::Lock() const
	{
	iLock.Wait();
	if (iParentEnv) iParentEnv->iLock.Wait();
	}

void CEnvironment::Unlock() const
	{
	if (iParentEnv) iParentEnv->iLock.Signal();
	iLock.Signal();
	}

void CEnvironment::WaitLC() const
	{
	Lock();
	CleanupStack::PushL(TCleanupItem(Signal, const_cast<CEnvironment*>(this)));
	}

void CEnvironment::Signal(TAny* aSelf)
	{
	static_cast<CEnvironment*>(aSelf)->Unlock();
	}

EXPORT_C void CEnvironment::SetL(const TDesC& aKey, TInt aValue)
	{
	TBuf<32> buf;
	buf.AppendNum(aValue);
	SetL(aKey, buf);
	}

EXPORT_C void CEnvironment::SetL(const TDesC& aKey, const TDesC& aValue)
	{
	WaitLC();

	HBufC** valPtr = iVars.Find(aKey);
	HBufC* currentValue = NULL;
	if (valPtr) currentValue = *valPtr;

	if (valPtr == NULL && iParentEnv)
		{
		// If we don't have it, and we have a parent env, we should pass the request through to it.
		iParentEnv->SetL(aKey, aValue);
		}
	else
		{
		if (currentValue)
			{
			// If we already have a value in the hash, just update it if possible
			TPtr ptr = currentValue->Des();
			if (ptr.MaxLength() >= aValue.Length())
				{
				ptr.Copy(aValue);
				CleanupStack::PopAndDestroy(); // Release lock
				return;
				}
			}
		}
	
	HBufC* newVal = aValue.AllocLC();
	iVars.InsertL(aKey, newVal);
	delete currentValue;
	CleanupStack::Pop(newVal);
	CleanupStack::PopAndDestroy(); // Release lock
	}

HBufC* CEnvironment::Get(const TDesC& aKey) const
	{
	HBufC*const* valPtr = iConstVars.Find(aKey);
	if (valPtr == NULL && iParentEnv)
		{
		return iParentEnv->Get(aKey);
		}
	else if (valPtr)
		{
		return *valPtr;
		}
	else
		{
		return NULL;
		}
	}

HBufC* CEnvironment::GetL(const TDesC& aKey) const
	{
	HBufC* var = Get(aKey);
	if (var == NULL) User::Leave(KErrNotFound);
	return var;
	}

EXPORT_C TInt CEnvironment::GetAsInt(const TDesC& aKey) const
	{
	Lock();
	HBufC* var = Get(aKey);
	__ASSERT_ALWAYS(var, Panic(EEnvVarNotFound1));
	TLex lex(*var);
	TInt result = 0;
	lex.Val(result);
	Unlock();
	return result;
	}

EXPORT_C TInt CEnvironment::GetAsIntL(const TDesC& aKey) const
	{
	WaitLC();
	HBufC* var = GetL(aKey);
	TLex lex(*var);
	TInt ret = 0;
	lex.Val(ret);
	CleanupStack::PopAndDestroy(); // Release lock
	return ret;
	}

EXPORT_C const TDesC& CEnvironment::GetAsDes(const TDesC& aKey) const
	{
	Lock();
	HBufC* var = Get(aKey);
	__ASSERT_ALWAYS(var, Panic(EEnvVarNotFound2));
	Unlock();
	return *var;
	}

EXPORT_C const TDesC& CEnvironment::GetAsDesL(const TDesC& aKey) const
	{
	WaitLC();
	const TDesC& des = *GetL(aKey);
	CleanupStack::PopAndDestroy(); // Release lock
	return des;
	}

EXPORT_C TInt CEnvironment::Remove(const TDesC& aKey)
	{
	Lock();
	HBufC** valPtr = iVars.Find(aKey);

	TInt ret = KErrNone;
	if (valPtr == NULL && iParentEnv)
		{
		// Not in ours, look at parent env
		ret = iParentEnv->Remove(aKey);
		}
	else if (valPtr && *valPtr)
		{
		// In ours
		delete *valPtr;
		if (iParentEnv)
			{
			// Need to remember we've removed it
			*valPtr = NULL;
			}
		else
			{
			iVars.Remove(aKey);
			}
		}
	else
		{
		// Not in ours and no parent env, or in ours and null
		ret = KErrNotFound;
		}

	Unlock();
	return ret;
	}

EXPORT_C void CEnvironment::RemoveL(const TDesC& aKey)
	{
	User::LeaveIfError(Remove(aKey));
	}

void CEnvironment::CopyL(const CEnvironment& aEnv)
	{
	RemoveAll();
	WaitLC();
	aEnv.WaitLC();

	RPointerArray<HBufC> keys;
	LtkUtils::CleanupResetAndDestroyPushL(keys);
	aEnv.GetKeysL(keys);
	for (TInt i = 0; i < keys.Count(); i++)
		{
		SetL(*keys[i], aEnv.GetAsDes(*keys[i]));
		}

	CleanupStack::PopAndDestroy(3); // keys, WaitLC x 2.
	}

class TDesRead
	{
public:
	TDesRead(const TDesC8& aDes);
	TInt32 ReadInt32();
	TPtrC ReadDes();
private:
	const TDesC8& iDes;
	TInt iPos;
	};
class TDesWrite
	{
public:
	TDesWrite(LtkUtils::RLtkBuf8& aDes);
	void WriteL(TInt32 aInt);
	void WriteL(const TDesC& aDes);
private:
	LtkUtils::RLtkBuf8& iDes;
	};

EXPORT_C void CEnvironment::InternalizeL(const TDesC8& aDes)
	{
	WaitLC();

	TDesRead desRead(aDes);
	TInt varCount = desRead.ReadInt32();
	while (varCount--)
		{
		TPtrC key = desRead.ReadDes();
		TPtrC val = desRead.ReadDes();
		SetL(key, val);
		}

	CleanupStack::PopAndDestroy(); // Release lock
	}

EXPORT_C HBufC8* CEnvironment::ExternalizeLC() const
	{
	WaitLC();
	LtkUtils::RLtkBuf8 buf;
	CleanupClosePushL(buf);
	TDesWrite desWrite(buf);

	RPointerArray<HBufC> keys;
	LtkUtils::CleanupResetAndDestroyPushL(keys);
	GetKeysL(keys);
	desWrite.WriteL(keys.Count());
	for (TInt i = 0; i < keys.Count(); i++)
		{
		desWrite.WriteL(*keys[i]);
		desWrite.WriteL(GetAsDes(*keys[i]));
		}
	CleanupStack::PopAndDestroy(&keys);
	CleanupStack::Pop(&buf);
	CleanupStack::PopAndDestroy(); // Release lock
	HBufC8* res = buf.ToHBuf();
	CleanupStack::PushL(res);
	return res;
	}

TInt StringCompare(const HBufC& aLeft, const HBufC& aRight)
	{
	return aLeft.Compare(aRight);
	}

EXPORT_C void CEnvironment::GetKeysL(RPointerArray<HBufC>& aResult) const
	{
	WaitLC();
	aResult.ResetAndDestroy();
	TStringHashIter<HBufC*> iter(iConstVars);
	while (iter.NextValue() != NULL)
		{
		if (*iter.CurrentValue() != NULL)
			{
			HBufC* key = iter.CurrentKey()->AllocLC();
			aResult.AppendL(key);
			CleanupStack::Pop(key);
			}
		}
	
	if (iParentEnv)
		{
		RPointerArray<HBufC> parentKeys;
		LtkUtils::CleanupResetAndDestroyPushL(parentKeys);
		iParentEnv->GetKeysL(parentKeys);
		for (TInt i = parentKeys.Count()-1; i >= 0; i--)
			{
			HBufC* key = parentKeys[i];
			if (iConstVars.Find(*key) == NULL)
				{
				// Only add stuff in parent that isn't also in ours
				aResult.AppendL(key);
				parentKeys.Remove(i);
				}
			}
		CleanupStack::PopAndDestroy(&parentKeys);
		}

	// Make sure the resulting array is alphabetic, as RStringHash doesn't guarantee that (unlike RVarSet)
	aResult.Sort(TLinearOrder<HBufC>(&StringCompare));
	CleanupStack::PopAndDestroy(); // Release lock
	}

EXPORT_C TBool CEnvironment::IsDefined(const TDesC& aKey) const
	{
	iLock.Wait();
	TBool ret = (Get(aKey) != NULL);
	iLock.Signal();
	return ret;
	}

EXPORT_C TBool CEnvironment::IsInt(const TDesC& aKey) const
	{
	iLock.Wait();
	HBufC* val = Get(aKey);
	iLock.Signal();

	if (val == NULL) return EFalse;
	
	TLex lex(*val);
	TBool atLeastOneDigit(EFalse);
	while (!lex.Eos())
		{
		if (lex.Get().IsDigit())
			{
			atLeastOneDigit = ETrue;
			}
		else
			{
			return EFalse;
			}
		}
	return atLeastOneDigit;
	}

EXPORT_C TBool CEnvironment::IsDes(const TDesC& aKey) const
	{
	return IsDefined(aKey);
	}

EXPORT_C TInt CEnvironment::Count() const
	{
	iLock.Wait();
	TInt ret = iConstVars.Count();
	iLock.Signal();
	return ret;
	}

EXPORT_C void CEnvironment::SetPwdL(const TDesC& aPwd)
	{
	SetL(KEnvPwd, aPwd);
	}

EXPORT_C const TDesC& CEnvironment::Pwd() const
	{
	return GetAsDes(KEnvPwd);
	}

EXPORT_C TChar CEnvironment::EscapeChar() const
	{
	iLock.Wait();
	HBufC* var = Get(KEnvEscapeChar);
	TUint16 escape = '^';
	if (var)
		{
		escape = (*var)[0];
		}
	iLock.Signal();
	return TChar(escape);
	}

// Declare aKey so that changes aren't reflected in the parent environment
EXPORT_C void CEnvironment::SetLocalL(const TDesC& aKey)
	{
	if (iParentEnv == NULL)
		{
		// Nothing needed if there is no parent, then everything is effectively local anyway
		return;
		}

	WaitLC();
	HBufC** currentValPtr = iVars.Find(aKey);
	if (currentValPtr)
		{
		// It's already local
		CleanupStack::PopAndDestroy(); // Release lock
		return;
		}

	// Local vals start out undefined, regardless of what the parent's value is
	iVars.InsertL(aKey, NULL);

	CleanupStack::PopAndDestroy(); // Release lock
	}

//
// TDesRead.
//

TDesRead::TDesRead(const TDesC8& aDes)
	: iDes(aDes), iPos(0)
	{
	}

TInt32 TDesRead::ReadInt32()
	{
	TInt32 int32;
	TPckg<TInt32> intPckg(int32);
	intPckg.Copy(iDes.Mid(iPos, sizeof(TInt32)));
	iPos += sizeof(TInt32);
	return int32;
	}

TPtrC TDesRead::ReadDes()
	{
	const TInt length = ReadInt32();
	const TInt size = length * 2;
	if (iPos & 1) iPos++; // 16-bit descriptors are 2-byte aligned
	TPtrC8 ptr8 = iDes.Mid(iPos, size);
	TPtrC result((const TUint16*)ptr8.Ptr(), length);
	iPos += size;
	return result;
	}


//
// TDesWrite.
//

TDesWrite::TDesWrite(LtkUtils::RLtkBuf8& aDes)
	: iDes(aDes)
	{
	}

void TDesWrite::WriteL(TInt32 aInt)
	{
	TPckgC<TInt32> intPckg(aInt);
	iDes.AppendL(intPckg);
	}

void TDesWrite::WriteL(const TDesC& aDes)
	{
	WriteL(aDes.Length());
	if (iDes.Size() & 1) iDes.AppendL('.'); // Pad so the desc is 2-byte aligned
	TPtrC8 ptr((TUint8*)aDes.Ptr(), aDes.Size());
	iDes.AppendL(ptr);
	}

