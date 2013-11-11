// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the l2cap L2CapEntity Config object owned by the L2CAP linkSignalhandler
// 
//

#include <bluetooth/logger.h>
#include "l2capEntityConfig.h"

#include "L2CapChannelConfig.h"
#include "l2capLinkSignalHandler.h"
#include "l2util.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

TL2CAPEntityConfig::TL2CAPEntityConfig(CL2CapLinkSignalHandler& aLinkSH)
 : iLinkSH(aLinkSH)
 	{
 	LOG_FUNC
 	}

TInt TL2CAPEntityConfig::PeerL2CapSupportedFeatures(TL2CapEntityInfo& aPeerL2CapEntityInfo )
/** Returns peer's L2Cap supported features if already received. Otherwise requests peer's L2Cap supported features.
	
	Called by the CL2CapSapSignalHandler. If the peer request is necessary, sets link info state to "requested".

	@internalComponent
	@param aPeerLinkInfo To be filled with cached peer's L2Cap config info.
*/
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	if (iPeerL2CapEntityInfo.LinkInfoState() == EL2CapEntityInfoUndef)
		{
		//Ask CL2CapSignalHandler for a InformationRequest
		iPeerL2CapEntityInfo.SetLinkInfoState(EL2CapEntityInfoRequested);
		rerr = iLinkSH.ConstructInformationRequest(EExtendedFeaturesSupported);
		}
	aPeerL2CapEntityInfo = iPeerL2CapEntityInfo;
	return rerr;
	}

void TL2CAPEntityConfig::UpdatePeerL2CapSupportedFeatures(const TL2CapEntityInfo& aPeerL2CapEntityInfo)
/** Called by the CL2CapSignalHandler when an information request has been responded to

	@internalComponent
	@param aRemoteL2CapEntityInfo Config Info from the remote device's L2Cap.
*/	
	{
	LOG_FUNC
	iPeerL2CapEntityInfo = aPeerL2CapEntityInfo;
	iPeerL2CapEntityInfo.SetLinkInfoState(EL2CapEntityInfoDefined);
	}

TBool TL2CAPEntityConfig::PeerInfoDefined() const
	{
	LOG_FUNC
	return iPeerL2CapEntityInfo.LinkInfoState() == EL2CapEntityInfoDefined;
	}

TBool TL2CAPEntityConfig::GetPeerL2CapEntityInfo(TL2CapEntityInfo& aPeerL2CapEntityInfo) const
/** A simple getter for peer's L2Cap supported features. No side effects, as opposed
	to PeerL2CapSupportedFeatures.
	@internalComponent
	@param aPeerL2CapEntityInfo To be filled with cached peer's L2Cap config info.
	@return ETrue if peer's config info is defined (and thus aPeerL2CapEntityInfo will
	    be filled), EFalse otherwise.
*/ 
	{
	LOG_FUNC
	if (iPeerL2CapEntityInfo.LinkInfoState() == EL2CapEntityInfoDefined)
		{
		aPeerL2CapEntityInfo = iPeerL2CapEntityInfo;
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
