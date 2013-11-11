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

#include"c32startshared.h"

inline const TCFGroupNameF& CModuleRef::GroupName() const
	{ 
	return static_cast<const TCFGroupNameF&>(iConfigParams.iGroupName);
	}

inline HBufC8* CModuleRef::GetIniData() const
	{
	return iIniData;
	}

inline TBool CModuleRef::IsOnDemand() const
	{
	return iConfigParams.iOnDemand;
	}

inline void CModuleRef::SetRMessage(const RMessage2& aMessage)
	{
	iRMessage = aMessage;
	}

inline void CBindingRef::SetState(TBindingState aState) 
	{
	iState=aState;
	}
	



