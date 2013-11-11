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
* Script Serial Comms I/O Functions inline functions.
* 
*
*/



/**
 @file Sio.inl
*/

#ifndef __SIO_INL__
#define __SIO_INL__

#include "SIO.H"

inline TBool CScriptIO::WritePending() const
/**
Return write status.
*/
	{
	return iWritePending;
	}

#endif
