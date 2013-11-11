// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Converter manager.
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <remcon/remconconverterplugin.h>
#include <remcon/remconconverterinterface.h>
#include "convertermanager.h"
#include "utils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

#ifdef _DEBUG
PANICCATEGORY("convman");
#endif

#ifdef __FLOG_ACTIVE
#define LOGCONVERTERS	LogConverters()
#else
#define LOGCONVERTERS
#endif

CConverterManager* CConverterManager::NewL()
	{
	LOG_STATIC_FUNC
	CConverterManager* self = new(ELeave) CConverterManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CLEANUPSTACK_POP1(self);
	return self;
	}

CConverterManager::~CConverterManager()
	{
	LOG_FUNC;
	LOGCONVERTERS;

	iConverterIfs.Reset();

	// Destroy all converter instances.
	iConverters.ResetAndDestroy();

	LOGCONVERTERS;
	}

CConverterManager::CConverterManager()
	{
	LOG_FUNC
	}

void CConverterManager::ConstructL()
	{
	LOG_FUNC;

	// Instantiate all converters at construction time. 
	LoadAllConvertersL();

	LOGCONVERTERS;
	}

void CConverterManager::LoadAllConvertersL()
	{
	LOG_FUNC;

	// Use ECOM to instantiate each implementation of the plugin interface.
	const TUid KUidRemoteControlConverterPluginInterface = TUid::Uid(KRemConConverterInterfaceUid);
	RImplInfoPtrArray implementations;
	const TEComResolverParams noResolverParams;
	REComSession::ListImplementationsL(KUidRemoteControlConverterPluginInterface, 
		noResolverParams, 
		KRomOnlyResolverUid, 
		implementations);
	CleanupResetAndDestroyPushL(implementations);
	const TUint count = implementations.Count();
	LOG1(_L("\tnumber of implementations of converter plugin interface: %d"), count);
	// It is legal to have no converters. Some bearers may do conversion 
	// internally. Others may have entirely state-dependent conversion.
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		CImplementationInformation* impl = implementations[ii];
		ASSERT_DEBUG(impl);
		LOG(_L("\tloading converter with:"));
		LOG1(_L("\t\timplementation uid 0x%08x"), impl->ImplementationUid());
		LOG1(_L("\t\tversion number %d"), impl->Version());
		TBuf8<KMaxName> buf8;
		buf8.Copy(impl->DisplayName());
		LOG1(_L8("\t\tdisplay name \"%S\""), &buf8);
		LOG1(_L("\t\tROM only %d"), impl->RomOnly());
		LOG1(_L("\t\tROM based %d"), impl->RomBased());

		CRemConConverterPlugin* conv = CRemConConverterPlugin::NewL(impl->ImplementationUid());
		CleanupStack::PushL(conv);
		LEAVEIFERRORL(iConverters.Append(conv));

		TInterfaceInfo info;
		info.iIf = reinterpret_cast<MRemConConverterInterface*>(conv->GetInterface(TUid::Uid(KRemConConverterInterface1)));
		// The converter plugins must currently support MRemConConverterInterface.
		ASSERT_DEBUG(info.iIf);
		info.iIfUid = TUid::Uid(KRemConConverterInterface1);
		info.iConverterUid = impl->ImplementationUid();
		TInt err = iConverterIfs.Append(info);
		if ( err != KErrNone )
			{
			iConverters.Remove(iConverters.Count() - 1);
			LEAVEIFERRORL(err);
			}
		CLEANUPSTACK_POP1(conv);
		}
	CleanupStack::PopAndDestroy(&implementations);
	}

#ifdef __FLOG_ACTIVE
void CConverterManager::LogConverters() const
	{
	const TUint count = iConverters.Count();
	LOG1(_L("\tNumber of converters = %d"), count);
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		const CRemConConverterPlugin* const conv = iConverters[ii];
		ASSERT_DEBUG(conv);
		LOG2(_L("\t\tconv %d [0x%08x]"), 
			ii, 
			conv);
		}
	}
#endif // __FLOG_ACTIVE

MRemConConverterInterface* CConverterManager::Converter(TUid aInterfaceUid, 
							   TUid aBearerUid) const
	{
	LOG_FUNC;
	LOG2(_L("\taInterfaceUid = 0x%08x, aBearerUid = 0x%08x"), aInterfaceUid, aBearerUid);

	MRemConConverterInterface* converter = NULL;

	const TUint count = iConverterIfs.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		const TInterfaceInfo& tmp = iConverterIfs[ii];
		ASSERT_DEBUG(tmp.iIf);
		if (tmp.iIf->SupportedUids(aInterfaceUid, aBearerUid))
			{
			converter = tmp.iIf;
			break;
			}
		}

	LOG1(_L("\tconverter = 0x%08x"), converter);
	return converter;
	}
	
MRemConConverterInterface* CConverterManager::Converter(const TDesC8& aInterfaceData, 
							   TUid aBearerUid) const
	{
	LOG_FUNC;

	MRemConConverterInterface* converter = NULL;

	const TUint count = iConverterIfs.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		const TInterfaceInfo& tmp = iConverterIfs[ii];
		ASSERT_DEBUG(tmp.iIf);
		if (tmp.iIf->SupportedInterface(aInterfaceData, aBearerUid))
			{
			converter = tmp.iIf;
			break;
			}
		}

	LOG1(_L("\tconverter = 0x%08x"), converter);
	return converter;
	}
