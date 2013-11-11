/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Extended Version Info Plugin API declarations
*
*/


#ifndef EXTENDEDVERSIONINFOPLUGIN_H
#define EXTENDEDVERSIONINFOPLUGIN_H

#include <e32cmn.h>
#include <sysversioninfo.h>

/**
* Declarations for the DLL interface of the Extended Version Info Plugin API.
* 
* @since S60 3.2
* @lib None.  
*/
namespace ExtendedVersionInfoPluginApi
    {
    enum TPluginOrdinal
        {
        /**
        * EGetProductInformationFuncL function ordinal
        */
        EGetProductInformationFuncL = 1
        };
    
    /**
    * Retrieves a product information item.
    * No capabilities are needed from client.
    *
    * @since S60 3.2
    * 
    * @param TVersionInfoType  Value identifying which product information item should be retrieved.
    * @param TDes&             On return, contains the requested product information item. 
    *                            
    * @leave KErrNotSupported    The requsted data could not be retrieved because it is not supported.
    * @leave KErrTooBig          The requested data does not entirely fit into given descriptor. In this
    *                            case the descriptor is filled up to its maximum length with the return data.
    */
    typedef void (*GetProductInformationFuncL)( SysVersionInfo::TVersionInfoType, TDes& );
    }

#endif  // EXTENDEDVERSIONINFOPLUGIN_H
