// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file SS_COMMSPROV.CPP
*/

#include "ss_commsprov.h"
#include <comms-infras/ss_log.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
// UNCOMMENT IF INTRODUCING AN ASSERT
//_LIT(KSpecAssert_ESockSSockscmspr, "ESockSSockscmspr");
#endif

using namespace ESock;

//
//TProviderInfo
namespace Mem4NullProviderInfo
{
static const TUint8 mem[sizeof(TProviderInfo)] = {0};
}

const TProviderInfo* const TProviderInfo::KNullProviderInfo = reinterpret_cast<const TProviderInfo*>(Mem4NullProviderInfo::mem);
EXPORT_C const TProviderInfo& TProviderInfo::NullProviderInfo() {return *TProviderInfo::KNullProviderInfo;};


//
// TLayerSelectionInfo
EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TProviderInfoExt, TProviderInfoExt::EUid, TProviderInfoExt::ETypeId)
// No attributes even though there are data members. The structure is not serialised
END_ATTRIBUTE_TABLE()

//
// TLayerSelectionInfo
EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TLayerConfig, TLayerConfig::EUid, TLayerConfig::ETypeId)
// No attributes even though there are data members. The structure is not serialised
END_ATTRIBUTE_TABLE()


//
// TLayerSelectionInfo
EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TLayerSelectionInfo, TLayerSelectionInfo::EUid, TLayerSelectionInfo::ETypeId)
// No attributes even though there are data members. The structure is not serialised
END_ATTRIBUTE_TABLE()


//
// TAccessPointPriority
EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TAccessPointPriority, TAccessPointPriority::EUid, TAccessPointPriority::ETypeId)
// No attributes even though there are data members. The structure is not serialised
END_ATTRIBUTE_TABLE()


//
// TAppSidConfig
EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TAppSidConfig, TAppSidConfig::EUid, TAppSidConfig::ETypeId)
// No attributes even though there are data members. The structure is not serialised
END_ATTRIBUTE_TABLE()



EXPORT_C RMetaExtensionContainerC& CCommsProviderBase::AccessPointConfig() const
	{
	return iAccessPointConfig;
	}

EXPORT_C CCommsProviderBase::~CCommsProviderBase()
    {
    // Release the reference if we have one
    iAccessPointConfig.Close();
    }

