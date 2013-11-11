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
// Inline implementations for: Parameter set base class for applying subconnection protocol attributes
// 
//

/**
 @file
 @internalTechnology
*/


#if !defined(__SS_PROTOCOLPARAMETERSET_INL__)
#define __SS_PROTOCOLPARAMETERSET_INL__

namespace ESock
{

CFlowRequestParameters::CFlowRequestParameters()
	{
	}

void CFlowRequestParameters::SetFlowParams(ESock::TFlowParams& aFlowParams)
	{
	iFlowParams = aFlowParams;
	}

ESock::TFlowParams& CFlowRequestParameters::GetFlowParams()
	{
	return iFlowParams;
	}

TUid CFlowRequestParameters::GetProtocolUid() const
	{
	switch (iFlowParams.iProtocol)
		{
		default:
			return TUid::Null();
		break;
		}
	}
}

#endif // __SS_PROTOCOLPARAMETERSET_INL__

