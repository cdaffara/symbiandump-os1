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
 @publishedPartner
 @deprecated Emulator Deprecated
*/

#ifndef __DISPLAY_DEV_H__
#define __DISPLAY_DEV_H__

/**
  Logical Device (factory class) for display
*/
class DDisplayDevice : public DLogicalDevice
	{
public:
	DDisplayDevice();
	~DDisplayDevice();
	TInt Install();
	void GetCaps(TDes8& aDes) const;
	TInt Create(DLogicalChannelBase*& aChannel);
	};

#endif

