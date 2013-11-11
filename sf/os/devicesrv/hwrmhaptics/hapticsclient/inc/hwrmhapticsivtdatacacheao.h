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
* Description:  Header file of haptics client's IVT-data cache's internal 
*                Active Object.
*
*/


#ifndef HWRMHAPTICSIVTDATACACHEAO_H
#define HWRMHAPTICSIVTDATACACHEAO_H

#include <e32base.h>

class CHWRMHapticsIVTDataCache;
class RHWRMHapticsSession;

/**
 * Haptics client's IVT-data cache's internal active object.
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CHWRMHapticsIVTDataCacheAO ) : public CActive
    { 
public:
    
    /**
     * Symbian two-phased constructor.
     *
     * @param 
     * @return Instance of the CHWRMHapticsIVTDataCacheAO class.
     */ 
    static CHWRMHapticsIVTDataCacheAO* NewL(
        TInt aFileHandle,
        CHWRMHapticsIVTDataCache* aCache,
        TRequestStatus& aStatus );

    /**
     * Symbian two-phased constructor.
     *
     * @return Instance of the CHWRMHapticsIVTDataCacheAO class.
     */ 
    static CHWRMHapticsIVTDataCacheAO* NewLC(
        TInt aFileHandle,
        CHWRMHapticsIVTDataCache* aCache,
        TRequestStatus& aStatus  );

    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsIVTDataCacheAO();

    /**
     * Method for making the async play effect call to haptics server
     */
    void PlayEffectAsynch( RHWRMHapticsSession* aClientSession,
                           const TIpcArgs& aArgs );

public:

    /**
     * From CActive
     */
    virtual void RunL();
    
    /**
     * From CActive
     */
    virtual void DoCancel();

    /**
     * From CActive
     */
    virtual TInt RunError( TInt aError );

private:

    /**
     * C++ constructor.
     */
    CHWRMHapticsIVTDataCacheAO( TInt aFileHandle,
                                CHWRMHapticsIVTDataCache* aCache,
                                TRequestStatus& aStatus  );

    /**
     * Second phase construction.
     */
    void ConstructL();

private:

    /**
     * The file handle for the data which is being played.
     */
    TInt iFileHandle;
     
    /**
     * Pointer to the cache which pointer array this AO belongs to.
     * Not owned.
     */
    CHWRMHapticsIVTDataCache* iCache;
    
    /**
     * Pointer to the request status of the client of haptics (that issued
     * the asynch play method in the first place).
     * Not owned.
     */
    TRequestStatus* iClientStatus;
    };
    
#endif // HWRMHAPTICSIVTDATACACHEAO_H   
            
// End of File
