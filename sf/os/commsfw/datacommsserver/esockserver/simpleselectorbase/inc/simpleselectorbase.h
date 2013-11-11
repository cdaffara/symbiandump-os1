// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/


#ifndef SYMBIAN_SIMPLE_PROVIDER_SELECTOR_BASE_H
#define SYMBIAN_SIMPLE_PROVIDER_SELECTOR_BASE_H

#include <comms-infras/ss_tiermanager.h>
#include <comms-infras/ss_connselect.h>
#include <comms-infras/ss_metaconnprov_internal.h>
#include <agentdialog.h> //RGenConAgentDialogServer

namespace CommsDat
{
	class CMDBSession;
	class CCDTierRecord;
}

namespace ESock
{
	class CMetaConnectionFactoryContainer;
}

//
//ASimpleSelectorBase
class ASimpleSelectorBase : public ESock::MProviderSelector
/** Simple connection selector base.

@internalComponent
*/
	{
public:
	TUint TypeId() { return iTypeId; }

protected:
	IMPORT_C ASimpleSelectorBase();
	IMPORT_C explicit ASimpleSelectorBase(const Meta::SMetaData& aSelectionPreferences);
	IMPORT_C explicit ASimpleSelectorBase(const TUint aTypeId, const Meta::SMetaData& aSelectionPreferences);
	IMPORT_C virtual ~ASimpleSelectorBase();
	IMPORT_C virtual void Cancel();

	//Default (simple) provider factory fn. Overriden by various legacy selectors.
	IMPORT_C ESock::CMetaConnectionProviderBase* FindOrCreateProviderL(TUint aAccessPoint, TAny* aInstance = NULL);//Can't return NULL
	IMPORT_C ESock::CMetaConnectionProviderBase* FindProviderL(TUint aAccessPoint, TAny* aInstance = NULL);//Can't return NULL

public:
	ESock::TSelectionPrefs iSelectionPrefs;
	CommsDat::CCDTierRecord* iTierRecord;
	CommsDat::CMDBSession* iDbs;
	ESock::CMetaConnectionFactoryContainer& iMetaContainer;
	TUint iTypeId;
	};

//
//CSimplePromptingSelectorBase
class CSimplePromptingSelectorBase : public CActive, public ASimpleSelectorBase
/** Base for simple selectors which may use dialog to process their selection requests.

@internalComponent
*/
	{
protected:
	/* I put this default ctor here because the AIpProtoSeletorBase class,
	 * which is inheriting from this class, has a default ctor as well. So
	 * it's possible that this class doesn't receive anything from the class
	 * inheriting from this.
	 */
	IMPORT_C CSimplePromptingSelectorBase();	
	IMPORT_C explicit CSimplePromptingSelectorBase(const TUint aTypeId, const Meta::SMetaData& aSelectionPreferences);
	IMPORT_C explicit CSimplePromptingSelectorBase(const Meta::SMetaData& aSelectionPreferences);
	
	IMPORT_C virtual ~CSimplePromptingSelectorBase();
	/* If there is a 'prompting AP' on the IPProto level the prompting logic 
	 * has to follow the logic implemented in 9.4 and before. 
	 */
	IMPORT_C void PromptUserL(ESock::ISelectionNotify& aSelectionNotify,
							  const TConnectionPrefs& aPrefs);
	IMPORT_C void PromptUserL(ESock::ISelectionNotify& aSelectionNotify);
	IMPORT_C virtual void DoCancel(); // From CActive

protected:
	ESock::ISelectionNotify iSelectionNotify; //Needed as a member because of the asynch selection
	RGenConAgentDialogServer iDlgServ;
	TUint32 iSelectedAPId;
	};

#endif //SYMBIAN_SIMPLE_PROVIDER_SELECTOR_BASE_H

