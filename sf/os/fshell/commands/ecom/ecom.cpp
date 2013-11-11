// ecom.cpp
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

#include <fshell/memoryaccesscmd.h>
#include <ecom/ecom.h>
#include <fshell/ltkutils.h>

using namespace IoUtils;

class CCmdEcom : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdEcom();
private:
	CCmdEcom();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	enum TCmd
		{
		EList,
		ECreate,
		};
	TCmd iCmd;
	TUint iUid;
	TBool iVerbose;
	};


CCommandBase* CCmdEcom::NewLC()
	{
	CCmdEcom* self = new(ELeave) CCmdEcom();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdEcom::~CCmdEcom()
	{
	REComSession::FinalClose();
	}

CCmdEcom::CCmdEcom()
	{
	}

const TDesC& CCmdEcom::Name() const
	{
	_LIT(KName, "ecom");	
	return KName;
	}

void CCmdEcom::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iCmd, _L("command"));
	aArguments.AppendUintL(iUid, _L("uid"));
	}

void CCmdEcom::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, _L("verbose"));
	}


EXE_BOILER_PLATE(CCmdEcom)

void CCmdEcom::DoRunL()
	{
	TUid uid = TUid::Uid(iUid);
	if (iCmd == EList)
		{
		RImplInfoPtrArray impls;
		LtkUtils::CleanupResetAndDestroyPushL(impls);
		TRAPL(REComSession::ListImplementationsL(uid, impls), _L("Couldn't ListImplementations for 0x%x"), iUid);
		if (impls.Count() == 0)
			{
			Printf(_L("No implementations found."));
			}
		else
			{
			for (TInt i = 0; i < impls.Count(); i++)
				{
				const CImplementationInformation* info = impls[i];
				Printf(_L("0x%x: %S"), info->ImplementationUid(), &info->DisplayName());
				TInt version = info->Version();
				if (version != 1)
					{
					Printf(_L(" (v%d)"), version);
					}
				if (iVerbose)
					{
					Printf(_L("\r\nRomOnly=%d RomBased=%d Drive=%c Disabled=%d"), info->RomOnly(), info->RomBased(), 'A' + (TInt)info->Drive(), info->Disabled());
					if (info->DataType().Length())
						{
						Printf(_L8("\r\nDataType: %S"), &info->DataType());
						}
					if (info->OpaqueData().Length())
						{
						Printf(_L("\r\nOpaqueData:\r\n"));
						LtkUtils::HexDumpToOutput(info->OpaqueData(), Stdout());
						}
					Printf(_L("\r\n"));
					}
				Printf(_L("\r\n"));
				}
			}
		CleanupStack::PopAndDestroy(&impls);
		}
	else if (iCmd == ECreate)
		{
		TUid implKey; // We don't care about this
		TAny* impl = NULL;
		TRAPL(impl = REComSession::CreateImplementationL(uid, implKey), _L("Couldn't instanciate plugin uid 0x%x"), iUid);
		if (impl == NULL) LeaveIfErr(KErrGeneral, _L("Plugin returned NULL implementation pointer!"));
		Printf(_L("Instanciated plugin 0x%x ok.\r\n"), iUid);

#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
		if (iVerbose)
			{
			LoadMemoryAccessL();
			
			TAny* vtablePtr = *(TAny**)impl;
			TFullName8 dllName;
			TInt res = iMemAccess.FindAddressInCodeSegments(dllName, vtablePtr);
			if (res < 0)
				{
				PrintError(res, _L("Couldn't find vtable address in code segments"));
				}
			else
				{
				Printf(_L8("Implementation is probably from DLL %S at offset %d.\r\n"), &dllName, res);
				}
			}
#endif

		User::Exit(KErrNone); // Don't even try and clean up - the plugin would probably crash if we did
		}
	}
