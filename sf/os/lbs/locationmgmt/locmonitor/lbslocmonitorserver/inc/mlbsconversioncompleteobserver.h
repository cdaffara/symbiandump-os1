/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef MLBSCONVERSIONCOMPLETEOBSERVER_H_
#define MLBSCONVERSIONCOMPLETEOBSERVER_H_

// Forward declarations
class CLbsLocMonitorConversionHandler;

/**
 * Class that needs to implement this interface to listen
 * to conversion complete event from the handler.
 *
 */
class MLbsConversionCompleteObserver 
    {
public:
    virtual void HandleConversionComplete(CLbsLocMonitorConversionHandler*
                                           aConversionHandler) = 0;
    };


#endif /*MLBSCONVERSIONCOMPLETEOBSERVER_H_*/
