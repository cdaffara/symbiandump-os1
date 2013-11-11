// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains MIpcRequestCompleteObserver which store one and only one solicited active at command and a number of active 
// 

#ifndef ATIPCREQUESTCOMPLETEOBSERVER_H_
#define ATIPCREQUESTCOMPLETEOBSERVER_H_

class MIpcRequestCompleteObserver
    {
public:    
    virtual void NotifyIpcRequestComplete(TInt aIpcId) = 0;
    };
#endif /* ATIPCREQUESTCOMPLETEOBSERVER_H_ */
