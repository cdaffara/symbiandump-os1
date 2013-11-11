// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// coretiermanagersuperselector.cpp
// 
//

/**
 @file
 @internalTechnology
*/

#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <comms-infras/ss_metaconnprov.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include <commsdattypesv1_1.h> // CommsDat
#include <es_connpref.h>	//TConnIdList
#include <connpref.h>		//TConnPrefList
#include <comms-infras/esock_params.h> //TConnAPPref
#include <comms-infras/coretiermanageractivities.h>
#include <comms-infras/simpleselectorbase.h>

using namespace ESock;
using namespace CommsDat;


class CCoreProviderSelectorSuper : public ASimpleSelectorBase
/** Simple selector object.
	May need to prompt the user in the future, currently not prompting.

@internalComponent
*/
	{
public:
	explicit CCoreProviderSelectorSuper(TUid tierId, const RConnPrefList& aConnPrefList)
	:	ASimpleSelectorBase()
		{
		iTierId = tierId;
		iConnPrefList = aConnPrefList;
		}

private:
	virtual void SelectL(ESock::ISelectionNotify& aSelectionNotify);

	RConnPrefList iConnPrefList;
	TUid iTierId;
	};


