// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// HCI Autonomous Queue FIFO item basic functionality
// 
//

#include <bluetooth/hci/hciframe.h>
#include "linkqitem.h"

CACLDataItem* CACLDataItem::NewL()
	{
	CACLDataItem* self = new (ELeave) CACLDataItem;
	return self;
	}

CACLDataItem::CACLDataItem()
	{
	}

CACLDataItem::~CACLDataItem()
	{
	delete iFrame;
	}

CHctlAclDataFrame*& CACLDataItem::Frame()
	{
	return iFrame;
	}
