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

#ifndef __EXTENSION_H__
#define __EXTENSION_H__

#include <networking/module_if.h>	// ..for TExtensionData

class TContextConfig;

class RExtensionData : public TExtensionData
	{
public:
	~RExtensionData();
	void Close();
	void SetUmtsType();
	void SetSblpType();
	TInt SetErrorCode(TInt aErrorCode);
	TInt CreateExtension(const TContextConfig& aRel99, TInt aErrorCode);
	RExtensionData();
private:
	RBuf8 iBuf;
	};

#endif
