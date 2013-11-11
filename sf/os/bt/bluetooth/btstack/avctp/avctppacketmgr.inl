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

#include "avctpmuxer.h"
	
inline CAvctpTransport& CAvctpPacketMgr::Transport() const
	{
	return iTransport;
	}

inline const TBTDevAddr& CAvctpPacketMgr::DevAddr() const
	{
	return iTransport.DevAddr();
	}
	
inline CAvctpProtocol& CAvctpPacketMgr::Protocol() const
	{
	return iProtocol;
	}

inline TBool CAvctpPacketMgr::IsClearToSend(TInt aChannel) const
	{
	return iTransport.IsClearToSend(aChannel);
	}
