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

#ifndef __TC_INL__
#define __TC_INL__

//lint -e{1763} would like return to be const (now 'indirectly modifies')
inline CPdpContext* CFlowData::PdpContext() const
	{ return iContext; }

inline const CFlowContext& CFlowData::FlowContext() const
	{ return iFlowContext; };

inline CNif& CFlowData::Nif() const
	{ return iNif; };

#endif
