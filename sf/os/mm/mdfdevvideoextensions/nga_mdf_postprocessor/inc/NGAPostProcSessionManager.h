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


#ifndef __NGAPOSTPROCSESSIONMANAGER_H__
#define __NGAPOSTPROCSESSIONMANAGER_H__

#include <graphics/surface.h> //TSurfaceId
#include <graphics/surfacemanager.h> //RSurfaceManager
#include <e32std.h>
#include <e32base.h>

class RSurfaceUpdateSession;
class MNGAPostProcSessionManagerObserver;
class CNGAPostProcNotifier;

class CNGAPostProcSessionManager
{

public: 
    //  === Constructors and destructor ===
    /**
    * Two-phased constructor.   
    * @return pointer to an instance of CNGAPostProcSessionManager
    */
    static CNGAPostProcSessionManager* NewL();

    /**
    * Destructor.
    */
    ~CNGAPostProcSessionManager();

public: 
    
    /**
    submits buffer to GCE for displaying.

    @param  "aBufId"   "ID of the buffer that needs to be displayed."
    @pre    "This method can only be called before the hwdevice has been initialized with Initialize()."
    */
    void PostPicture(const TSurfaceId& aSurfaceId, TInt aBufId, TInt aNumOfBuf, TBool aNotifyRequired);

    /**
    Sets the device output format.
    */
    void CancelUpdate();
    
    /**
	Removes display from the system, if it is already existing. 

    @param  "aDisplayId"  "ID of the display that needs to be removed from the system"
    */
    void HandleBufferRelease(TInt aBufId, TInt aStatus);
    
    /**
	Removes display from the system, if it is already existing. 

    @param  "aDisplayId"  "ID of the display that needs to be removed from the system"
    */
    void SetObserver(MNGAPostProcSessionManagerObserver& aObserver);
	    
protected:
	CNGAPostProcSessionManager();
    /**
     *  Symbian 2nd phase constructor .
     */
    void ConstructL();

    
private: 

    RSurfaceUpdateSession*      			iSurfaceUpdateSession;
    MNGAPostProcSessionManagerObserver*		iObserver;
    RArray<CNGAPostProcNotifier*>			iAOList;
};    

#endif //__NGAPOSTPROCSESSIONMANAGER_H__