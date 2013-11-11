/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CHWRMPluginTransactionList::GetFirstItem
* Make inline?
*
*/



inline THWRMPluginTransactionListItem* CHWRMPluginTransactionList::GetFirstItem()
    {
    return iTransactionData;
    }
    
// -----------------------------------------------------------------------------
// CHWRMPluginTransactionList::Count
// Make inline?
// -----------------------------------------------------------------------------
//
inline TInt CHWRMPluginTransactionList::Count() const
    {
    return iCount;
    }
    
            
// End of File
