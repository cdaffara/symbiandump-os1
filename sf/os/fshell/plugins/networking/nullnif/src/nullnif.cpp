// nullnif.cpp
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

#include <in_sock.h>
#include <in_iface.h>
#include <comms-infras/nifif.h>


NONSHARABLE_CLASS(CNullNif) : public CNifIfLink
	{
public:
	CNullNif(CNifIfFactory& aFactory);
	~CNullNif();
	virtual void BindL(TAny *aId);
	virtual TInt State();
	virtual TInt Control(TUint aLevel,TUint aName,TDes8& aOption, TAny* aSource=0);
	virtual TInt Send(RMBufChain& aPdu, TAny* aSource=0);
	virtual TInt Start();
	virtual void Stop(TInt aReason, MNifIfNotify::TAction aAction);
	virtual CNifIfBase* GetBinderL(const TDesC& aName);
	virtual void Info(TNifIfInfo& aInfo) const;
	virtual TInt Notification(TAgentToNifEventType aEvent, void* aInfo);
	virtual void Restart(CNifIfBase* aIf);
	static void FillInInfo(TNifIfInfo& aInfo);
	};

CNullNif::CNullNif(CNifIfFactory& aFactory)
	: CNifIfLink(aFactory)
	{
	}

CNullNif::~CNullNif()
	{
	}

void CNullNif::Info(TNifIfInfo& aInfo) const
	{
	FillInInfo(aInfo);
	}

void CNullNif::FillInInfo(TNifIfInfo& aInfo)
	{
	aInfo.iVersion = TVersion(0, 0, 0);
	aInfo.iFlags = KNifIfIsBase | KNifIfUsesNotify | KNifIfIsLink | KNifIfCreatedByFactory;
	_LIT(KNullNif, "nullnif");
	aInfo.iName = KNullNif;
	aInfo.iProtocolSupported = KProtocolInetIp;
	}

void CNullNif::BindL(TAny*)
	{
	}

TInt CNullNif::Send(RMBufChain&, TAny*)
	{
	return 0;
	}

TInt CNullNif::State()
	{
    return EIfUp;
	}

TInt CNullNif::Control(TUint aLevel, TUint aName, TDes8& aOption, TAny* /*aSource*/)
	{
	if (aLevel != KSOLInterface)
		{
		return KErrNotSupported;
		}

	switch (aName)
		{
		case KSoIfInfo:
			{
			TSoIfInfo& opt = *(TSoIfInfo*)aOption.Ptr();
			TNifIfInfo info;
			FillInInfo(info);
			opt.iName = info.iName;
			return KErrNone;
			}
		}
		
	return KErrNotSupported;
	}

CNifIfBase* CNullNif::GetBinderL(const TDesC&)
	{
	return NULL;
	}

TInt CNullNif::Start()
	{
	iNotify->IfProgress(KLinkLayerOpen, KErrNone);
	iNotify->LinkLayerUp();
	return KErrNone;
	}

void CNullNif::Stop(TInt aReason, MNifIfNotify::TAction aAction)
	{
	iNotify->LinkLayerDown(aReason, aAction);
	iNotify->IfProgress(EIfProgressLinkDown, aReason);
	}

TInt CNullNif::Notification(TAgentToNifEventType, void*)
	{
	return KErrNone;
	}

void CNullNif::Restart(CNifIfBase*)
	{
	}


NONSHARABLE_CLASS(CNullNifIfFactory) : public CNifIfFactory
	{
protected:
	virtual void InstallL();
	virtual CNifIfBase* NewInterfaceL(const TDesC& aName);
	TInt Info(TNifIfInfo& aInfo, TInt aIndex) const;
	};

TInt CNullNifIfFactory::Info(TNifIfInfo& aInfo, TInt) const
	{
	CNullNif::FillInInfo(aInfo);
	return 1;
	}

void CNullNifIfFactory::InstallL()
	{
	}

CNifIfBase* CNullNifIfFactory::NewInterfaceL(const TDesC& aName)
	{
	_LIT(KNullNif, "nullnif");
	if (aName.CompareF(KNullNif))
		{
		User::Leave(KErrNotSupported);
		}

	return new (ELeave) CNullNif(*this);
	}
	
// Force export of non-mangled name
extern "C" { IMPORT_C CNifFactory *Install(); }

EXPORT_C CNifFactory *Install()
	{
	return new (ELeave) CNullNifIfFactory;
	}

