/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* This file contains panic codes for PSD AGX specific panics
* 
*
*/



/**
 @file PSDPanic.H
 @internalComponent
*/

#ifndef __PSD_PANIC_H__
#define __PSD_PANIC_H__

/**
panic codes for PSD AGX specific panics
Not public panic codes
*/
enum TPsdAgt
	{
	EPsdBadDirection,			///< panic codes for Psd Bad Direction
	EPsdObjectShouldNotRun,		///< panic codes for ObjectShouldNotRun
	EPsdInvalidNtwkRegistration,///< panic codes for Invalid Network Registration
	EPsdUnknownContextState,	///< panic codes for Psd Unknown Context State
	EPsdExcessDataNotValid,		///< panic codes for Psd ExcessData Not Valid
	EPsdNotificationNotValid,	///< panic codes for PsdNotification Not Valid
	EPsdModeUnsupported			///< panic codes for PsdMode Unsupported
	};

/**
PSD AGX specific panics

@param aPanic
*/
GLDEF_C void PanicAgt(TPsdAgt aPanic);


#endif
