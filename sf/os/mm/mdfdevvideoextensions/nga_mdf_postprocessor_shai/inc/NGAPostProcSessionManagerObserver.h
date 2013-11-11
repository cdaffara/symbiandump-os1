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
* Description:
*
*/


#ifndef __NGAPOSTPROCSESSIONMANAGEROBSERVER_H__
#define __NGAPOSTPROCSESSIONMANAGEROBSERVER_H__

/**
@internal

The class is a mixin and is intended to support for communicating buffers availablity 
after buffer has been released by GCE. 
*/
class MNGAPostProcSessionManagerObserver
{
public:
    /**
    This method can be called from session manager, whenever a buffer is released by GCE.

    @param  "aBufId Id of the buffer that got released by GCE. This buffer can be used to get 
    		 more data from decoder"
    */
    virtual void BufferAvailable(TInt aBufId, TInt aStatus) = 0;
    
};    

#endif //__NGAPOSTPROCSESSIONMANAGEROBSERVER_H__