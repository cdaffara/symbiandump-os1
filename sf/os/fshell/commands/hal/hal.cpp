// hal.cpp
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
#include <hal.h>
#include <fshell/common.mmh>
#include <fshell/ltkhal.h>
#include <fshell/ltkutils.h>

using namespace IoUtils;

_LIT(KDefaultDumpFileName, "c:\\private\\102825B1\\HAL.DAT");

class CCmdHal : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdHal();
private:
	CCmdHal();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	enum 
		{
		EList,
		EGet,
		ESet,
		EDumpFile
		} iOperation;
	HAL::TAttribute iAttribute;
	TInt iValue;
	TInt iDeviceNumber;
	TFileName2 iDumpFileName;
	};


CCommandBase* CCmdHal::NewLC()
	{
	CCmdHal* self = new(ELeave) CCmdHal();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdHal::~CCmdHal()
	{
	}

CCmdHal::CCmdHal() : iDumpFileName(KDefaultDumpFileName)
	{
	}

const TDesC& CCmdHal::Name() const
	{
	_LIT(KName, "hal");	
	return KName;
	}

void CCmdHal::DoRunL()
	{
	switch (iOperation)
		{
		case EGet:
			{
			if (!iArguments.IsPresent(1))
				{
				LeaveIfErr(KErrArgument, _L("Attribute to get not specified"));
				}
			LtkUtils::CHalAttribute* attrib = NULL;
			if (iOptions.IsPresent(&iDeviceNumber))
				{
				TRAPL(attrib = LtkUtils::GetHalInfoL(iDeviceNumber, iAttribute), _L("Couldn't get attribute value"));
				}
			else
				{
				TRAPL(attrib = LtkUtils::GetHalInfoL(iAttribute), _L("Couldn't get attribute value"));
				}
			Printf(_L("%S: %S\r\n"), &attrib->iAttributeName, attrib->iDescription);
			delete attrib;
			}
			break;
		case ESet:
			{
			if (!iArguments.IsPresent(1))
				{
				LeaveIfErr(KErrArgument, _L("Attribute to set not specified"));
				}
			if (!iArguments.IsPresent(2))
				{
				LeaveIfErr(KErrArgument, _L("Attribute value to set not specified"));
				}
			if (iOptions.IsPresent(&iDeviceNumber))
				{
				LeaveIfErr(HAL::Set(iDeviceNumber, iAttribute, iValue), _L("Couldn't set attribute"));
				}
			else
				{
				LeaveIfErr(HAL::Set(iAttribute, iValue), _L("Couldn't set attribute"));
				}
			}
			break;
		case EList:
			{
			RPointerArray<LtkUtils::CHalAttribute> attribs;
			LtkUtils::GetHalInfoL(attribs);
			for (TInt i = 0; i < attribs.Count(); ++i)
				{
				LtkUtils::CHalAttribute& attrib = *attribs[i];
				Printf(_L("%4d %S: %S\r\n"), attrib.iAttribute, &attrib.iAttributeName, attrib.iDescription);
				}
			attribs.ResetAndDestroy();
			}
			break;
		case EDumpFile:
			{
			if (!iOptions.IsPresent(&iDumpFileName))
				{
				iDumpFileName[0] = LtkUtils::GetSystemDrive();
				}
			RFile file;
			LeaveIfErr(file.Open(FsL(), iDumpFileName, EFileRead), _L("Couldn't open \"%S\" for reading"), &iDumpFileName);
			CleanupClosePushL(file);

			TInt fileSize;
			LeaveIfErr(file.Size(fileSize), _L("Couldn't find the size of \"%S\""), &iDumpFileName);

			RBuf8 buf;
			buf.Create(fileSize);
			buf.CleanupClosePushL();

			LeaveIfErr(file.Read(buf), _L("Couldn't read \"%S\""), &iDumpFileName);

			TInt32* p = (TInt32*)buf.Ptr();
			TInt32* pE = p + (fileSize / sizeof(TInt32)) - 1;
			Printf(_L("Machine UID: 0x%08x\r\n"), *p++);
			Printf(_L("Type prefix: 0x%08x\r\n"), *p++);

			while (p < pE)
				{
				TInt attribute = *p++;
				TInt deviceNumber = (TUint)attribute >> 24;
				attribute &= 0xFFFFFF;
				TInt value = *p++;
				LtkUtils::CHalAttribute* attrib = LtkUtils::GetHalInfoForValueL(deviceNumber, attribute, value);
				Printf(_L("%4d %S[%d]: %S\r\n"), attrib->iAttribute, &attrib->iAttributeName, deviceNumber, attrib->iDescription);
				delete attrib;
				}

			CleanupStack::PopAndDestroy(2, &file);
			}
			break;
		}
	}

void CCmdHal::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArgOperation, "operation");
	aArguments.AppendEnumL((TInt&)iOperation, KArgOperation);

	_LIT(KArgAttribute, "attribute");
	aArguments.AppendUintL((TUint&)iAttribute, KArgAttribute);

	_LIT(KArgValue, "value");
	aArguments.AppendIntL(iValue, KArgValue);
	}

void CCmdHal::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptDevice, "device");
	aOptions.AppendIntL(iDeviceNumber, KOptDevice);

	_LIT(KOptDumpFileName, "dump-file-name");
	aOptions.AppendFileNameL(iDumpFileName, KOptDumpFileName);
	}


EXE_BOILER_PLATE(CCmdHal)

