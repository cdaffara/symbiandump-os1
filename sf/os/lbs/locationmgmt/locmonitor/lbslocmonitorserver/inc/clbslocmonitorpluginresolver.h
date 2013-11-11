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
* Description: Declaration of CLbsLocMonitorPluginResolver class
*
*/

#ifndef CLBSLOCMONITORPLUGINRESOLVER_H_
#define CLBSLOCMONITORPLUGINRESOLVER_H_

#include <e32base.h>
#include <ecom/ecom.h>

/**
 * This class is responsible for choosing the default converter plugin based
 * on vendorid and plugin priority.
 * 
 */
class CLbsLocMonitorPluginResolver : public CActive
    {
public:
    /**
     * Constructs new object of CLbsLocMonitorPluginResolver.
     */
    static CLbsLocMonitorPluginResolver* NewL();
    
    /**
     * Destructor. 
     */
    ~CLbsLocMonitorPluginResolver();
    
    /**
     * Returns uid of default converter. 
     * @param[out] aConverterPluginUid Default converter plugin id.
     * @return KErrNotFound If none of converter plugin available.
     */
    TInt DefaultConverterUid( TUid& aConverterPluginUid ); 
    
protected: // From CActive
    void RunL();
    void DoCancel();
    

private:
    /**
     * Default constructor. 
     */
    CLbsLocMonitorPluginResolver();
    
    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();
    
    /**
     * Finds default converter plugin
     */
    void FindDefaultConverterPluginL(); 
    
    /**
     * 
     */
    TInt PriorityInfo( const TDesC8& aOpaqueData );
private: // Member variables
    
    /**
     * Pointer to ecom session 
     */
    REComSession* iEcomSession;
    
    /**
     * Default converter plugin uid.
     */
    TUid iDefaultConverterPluginId;
    };
#endif /* CLBSLOCMONITORPLUGINRESOLVER_H_ */
