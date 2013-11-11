// propertywatcher.cpp
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

EXPORT_C CPropertyManager* CPropertyManager::NewL(TCallBack aCallback)
	{
	CPropertyManager* self = new(ELeave) CPropertyManager(aCallback);
	return self;
	}

EXPORT_C void CPropertyManager::SetPropertyFromStringL(const TDesC& /*aString*/)
	{
	}

EXPORT_C HBufC* CPropertyManager::GetValueL()
	{
	if (!iProperty.Handle()) User::Leave(KErrNotReady);

	TInt err = KErrNone;
	if (iPropertyType == RProperty::ETypeLimit)
		{
		// First time called, need to figure out what type the property is
		TInt propInt;
		//TBuf8<1> propBuf8;
		
		err = iProperty.Get(propInt);
		if (err == KErrNotFound)
			{
			return NULL;
			}
		if (err == KErrPermissionDenied)
			{
			User::Leave(err);
			}

		if (err != KErrArgument)
			{
			iPropertyType = RProperty::EInt;
			}
		else
			{
			//err = iProperty.Get(propBuf8);
			//if (err == 
			// Just assume it's binary, for now
			iPropertyType = RProperty::EByteArray;
			}
		}

	switch (iPropertyType)
		{
		case RProperty::EInt:
			{
			TInt val;
			TInt err = iProperty.Get(val);
			if (err == KErrNone)
				{
				HBufC* res = HBufC::NewL(15);
				res->Des().Format(_L("%i"), val);
				return res;
				}
			else if (err == KErrArgument)
				{
				// Someone's redefined the property as a different type
				iPropertyType = RProperty::ETypeLimit;
				return GetValueL();
				}
			else if (err == KErrNotFound)
				{
				// property deleted or undefined
				return NULL;
				}
			else
				{
				User::Leave(err);
				}
			break;
			}
		case RProperty::EByteArray:
			{
			for (TInt size = 512; /*Nothing*/; size *= 2)
				{
				HBufC8* val = HBufC8::NewL(size);
				TPtr8 ptr = val->Des();
				TInt err = iProperty.Get(ptr);
				if (err == KErrNone)
					{

					_LIT(KBinary, "Binary:");
					CleanupStack::PushL(val);
					HBufC* res = HBufC::NewL(val->Length() * 4 + KBinary().Length());
					TPtr ptr = res->Des();
					//ptr.Append(KBinary);
					for (TInt i = 0; i < val->Length(); i++)
						{
						if (i % 8 == 0) ptr.Append('\n');
						if (i % 4 == 0 && i != 0) ptr.Append(' ');
						TBuf<2> numBuf;
						numBuf.NumFixedWidthUC((*val)[i], EHex, 2);
						ptr.Append(numBuf);
						//ptr.AppendFormat(_L("%02x"), (TUint)(*val)[i]);
						}
					CleanupStack::PopAndDestroy(val);
					return res;
					}
				else
					{
					delete val;
					}

				if (err == KErrOverflow)
					{
					// Try with bigger buffer
					continue;
					}
				else if (err == KErrArgument)
					{
					// Someone's redefined the property as a different type
					iPropertyType = RProperty::ETypeLimit;
					return GetValueL();
					}
				else if (err == KErrNotFound)
					{
					// property deleted or undefined
					return NULL;
					}
				else
					{
					User::Leave(err);
					}
				}
			}
		default:
			break;
		}
	return NULL; // To satisfy compiler
	}

EXPORT_C TInt CPropertyManager::ChangeProperty(TUint aCategory, TUint aKey)
	{
	Cancel();
	iPropertyType = RProperty::ETypeLimit;
	iCat = aCategory;
	iKey = aKey;

	iProperty.Close();
	TInt err = iProperty.Attach(TUid::Uid(aCategory), aKey);
	if (!err)
		{
		Watch();
		}
	return err;
	}

void CPropertyManager::Watch()
	{
	if (!IsActive())
		{
		iProperty.Subscribe(iStatus);
		SetActive();
		}
	}

void CPropertyManager::RunL()
	{
	if (!iStatus.Int())
		{
		Watch();
		}
	iChangedFn.CallBack();
	}

void CPropertyManager::DoCancel()
	{
	iProperty.Cancel();
	}

CPropertyManager::CPropertyManager(TCallBack aCallback)
	: CActive(CActive::EPriorityStandard), iPropertyType(RProperty::ETypeLimit), iChangedFn(aCallback)
	{
	CActiveScheduler::Add(this);
	}

CPropertyManager::~CPropertyManager()
	{
	Cancel();
	iProperty.Close();
	}
