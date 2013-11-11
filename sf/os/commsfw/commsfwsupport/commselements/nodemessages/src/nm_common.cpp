// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
*/

#include <elements/nm_common.h>
#include <elements/nm_messages_base.h>
#include <elements/nm_log.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemNodeMessCmnC, "ElemNodeMessCmnC");
#endif

using namespace Messages;

EXPORT_C CGlobals& TlsGlobals::InstallGlobalsL()
	{
	CGlobals* gl = new (ELeave) CGlobals;
	CleanupStack::PushL(gl);
	gl->ConstructL();
	CleanupStack::Pop(gl);
	__ASSERT_DEBUG(Dll::Tls()==NULL, User::Panic(KSpecAssert_ElemNodeMessCmnC, 1));
	Dll::SetTls(gl);
	return *gl;
	}

EXPORT_C void TlsGlobals::UnInstallGlobals()
	{
	CGlobals* gl = reinterpret_cast<CGlobals*>(Dll::Tls());
	Dll::SetTls(NULL);
	__ASSERT_DEBUG(gl!=NULL, User::Panic(KSpecAssert_ElemNodeMessCmnC, 2));
	delete gl;
	}

EXPORT_C CGlobals& TlsGlobals::Get()
	{
	CGlobals* gl = reinterpret_cast<CGlobals*>(Dll::Tls());
	__ASSERT_DEBUG(gl!=NULL, User::Panic(KSpecAssert_ElemNodeMessCmnC, 3));
	return *gl;
	}



EXPORT_C CGlobals::CGlobals()
  : CGlobalsInternal()
	{
	}

EXPORT_C CGlobals::~CGlobals()
	{
	TEBase::DeRegister(*this);
	}

EXPORT_C void CGlobals::ConstructL()
	{
	CGlobalsInternal::ConstructL();
	TEBase::RegisterL(*this);
	}



EXPORT_C CGlobalsInternal::CGlobalsInternal()
	{
	}

EXPORT_C CGlobalsInternal::~CGlobalsInternal()
	{
	delete iVirtCtor;
	}

EXPORT_C void CGlobalsInternal::ConstructL()
	{
	iVirtCtor = new (ELeave) Meta::CMetaDataVirtualCtorInPlace;
	}

EXPORT_C void CGlobalsInternal::AddPersistentItf(const TNodeId& aNodeId)
	{
	(void) aNodeId;
	NM_LOG((KNodeMessagesSubTag, _L8("CSockManData::AddPersistentItf(%08x) [#%d]"), aNodeId.Ptr(), iPersistentItfCount));
	++iPersistentItfCount;
	}

