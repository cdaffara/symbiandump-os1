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
// Mobility Meta Connection Provider definition
// 
//

/**
 @file
 @internalComponent
*/

#ifndef MOBILITYMCPR_H
#define MOBILITYMCPR_H

#include <comms-infras/coremcpr.h>

namespace MobilityMCprActivities
{
	class CMobilityActivity;
}

//
//CCoreMobileMetaConnectionProvider
class CMobilityMetaConnectionProvider : public CCoreMetaConnectionProvider
    {
    friend class MobilityMCprActivities::CMobilityActivity;

public:
	IMPORT_C virtual ~CMobilityMetaConnectionProvider();
	TBool IsHandshakingNow() const; //Used by other activities to check the mobility status

protected:
    IMPORT_C CMobilityMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory,
                                             const ESock::TProviderInfo& aProviderInfo,
                                             const MeshMachine::TNodeActivityMap& aActivityMap);

    IMPORT_C void Received(MeshMachine::TNodeContextBase& aContext);

private:
	TBool iIsHandshakingNow : 1; //Synchronisation flags, private by design, can be modified only by CMobilityActivity
    };

#endif // MOBILITYMCPR_H

