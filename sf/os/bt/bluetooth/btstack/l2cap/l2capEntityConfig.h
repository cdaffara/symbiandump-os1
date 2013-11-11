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
// Implements the l2cap L2Cap Entity Config object owned by the L2CAP linkSignalhandler
// 
//

#ifndef __L2CAPENTITYCONFIG_H__
#define __L2CAPENTITYCONFIG_H__

#include "L2types.h"

class CL2CapLinkSignalHandler;

NONSHARABLE_CLASS(TL2CAPEntityConfig)
	{
public:
	TL2CAPEntityConfig(CL2CapLinkSignalHandler& aLinkSH);

	TInt PeerL2CapSupportedFeatures(TL2CapEntityInfo& aPeerL2CapEntityInfo);
	void UpdatePeerL2CapSupportedFeatures(const TL2CapEntityInfo& aPeerL2CapEntityInfo);

	TBool PeerInfoDefined() const;
	TBool GetPeerL2CapEntityInfo(TL2CapEntityInfo& aPeerL2CapEntityInfo) const;
private:
	TL2CapEntityInfo iPeerL2CapEntityInfo;
	CL2CapLinkSignalHandler& iLinkSH;
	};

#endif  //__L2CAPENTITYCONFIG_H__





