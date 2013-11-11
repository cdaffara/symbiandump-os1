// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMF_DRM_CUSTOM_COMMANDS_ENUM_H__
#define __MMF_DRM_CUSTOM_COMMANDS_ENUM_H__


#include <e32base.h>
#include <mmf/common/mmcaf.h>
#include <mmf/common/mmfstandardcustomcommands.h>

/**
@internalComponent
*/
enum TMMFDRMControlMessages
	{
	EMMFDRMControlEvaluateIntent,
	EMMFDRMControlExecuteIntent,
	EMMFDRMControlDisableAutomaticIntent,
	EMMFDRMControlSetAgentProperty,
	EMMFDRMControlIsSupported
	};


#endif // __MMF_DRM_CUSTOM_COMMANDS_ENUM_H__
