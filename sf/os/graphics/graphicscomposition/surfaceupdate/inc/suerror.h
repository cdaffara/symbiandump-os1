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

#ifndef __SUERROR_H__
#define __SUERROR_H__

/**
@file

@publishedPartner
@prototype
*/

/**
Signifies that the submit update request should be submitted to all content update receivers registered with the SUS.
*/
const TInt KAllScreens = 0xffffff;  


/**
Surface Update Base error
*/
const TInt KErrSurfaceUpdateBase = -220;

/**
Indicates that content update receiver backend for specified screen hasn't been registered with the server
@deprecated use KErrUpdateReceiverNotAvailable
*/
const TInt KErrBackendNotAvailable = -221;
/**
Indicates that no content update receiver has been registered for specified screen with the server
*/
const TInt KErrUpdateReceiverNotAvailable = -221;

/**
Indicates that the surface hasn't been registered with the specific content update receiver
*/
const TInt KErrSurfaceNotRegistered = -222;


/**
Indicates that the surface is not visible
*/
const TInt KErrNotVisible = -223;

#endif
