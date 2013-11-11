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
//

#ifndef RMOCKPOWER_H_
#define RMOCKPOWER_H_

#include "rmockhwrmplugin.h"

/**
Specific MockSY API for Power service plugin
*/
class RMockPower : public RMockHwrmPlugin
	{
protected:
	IMPORT_C virtual const TDesC& ServerName();
	};
	
#endif /*RMOCKPOWER_H_*/
