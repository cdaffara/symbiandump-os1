/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This API is used for creating mappings between remote
*                 control clients.
*
*/


#ifndef TSPCLIENTMAPPERIMPL_H
#define TSPCLIENTMAPPERIMPL_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <tspclientmapper.h>

// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
* @since S60 5.0
*/
NONSHARABLE_CLASS( CTspClientMapperImpl ): public CTspClientMapper 
  {
  
public:
    
    /**
    * Default Constructor 
    */
    CTspClientMapperImpl();
    
    
    
    /**
    * Default Destructor 
    */
    virtual ~CTspClientMapperImpl();
    
    /**
    * Symbian two phased constructor.
    */
    static CTspClientMapperImpl* NewL();
    
public: 

    /**
    * This function is used to set a target client for TSP. This way Target
    * Selector Plugin creates a mapping to the correct client.
    *
    * @param    aMappingType @see TspClientMappingType
    * @param    aProcessId is the process identifier of the client
    */
    void SetTspTargetClient( 
        TTspClientMappingType aMappingType,
        TProcessId aProcessId );
    
    /**
    * This function is used to set a TSP target client for another type then it
    * was originally set for. This way targets don't have to be removed from
    * one type and then added to another.
    *
    * @param    aMappingType is the type this client has to be set for
    * @param    aProcessId is the process identifier of the client
    * @leave    KErrNotSupported if invalid mapping type. KErrNotFound if this
    *           ProcessId is not found from any of the arrays.
    */
    TInt SetTspTargetClientToOtherType( 
        TTspClientMappingType aMappingType,
        TProcessId aProcessId );
    
    /**
    * This function is used to remove a target client from TSP. This way Target
    * Selector Plugin creates a mapping to the correct client.
    *
    * @param    aMappingType @see TspClientMappingType
    * @param    aProcessId is the process identifier of the client
    * @return   KErrNotSupported if invalid mapping type or KErrNotFound if
    *           this process id is not defined for this type.
    */
    TInt RemoveTspTargetClient(  
        TTspClientMappingType aMappingType,
        TProcessId aProcessId );
    
    
    
private:
    
    friend class CRemConTspController;
    
    /**
    * Default Constructor 
    */
    void ConstructL();
    
    /**
    * This function is used to get the audio clients that are currently
    * playing audio. The function is used by TSP.
    *
    * @param    aMappingType @see TspClientMappingType
    * @param    aPidArray is given as a reference. The playing audio clients
    *           are added to this array.
    * @return   aPidArray is returned with the asked process identifiers.
    */
    TInt GetTspTargetClients(  
        TTspClientMappingType aMappingType,
        RArray<TProcessId>& aPidArray );
    
    void OpenMutexL();
    
    void CloseMutex();
    
private:
    /**
    * Synchronizes access to client array.
    */
    RMutex iMutex;
    
    /**
    * A helper class for managing acces to mutex.
    */
    class TTspClientMapperSynch
        {
        public:
            TTspClientMapperSynch( RMutex& aMutex );
            ~TTspClientMapperSynch();
        private:
            TTspClientMapperSynch();
            RMutex& iMutex;
        };
    };

#endif // TSPCLIENTMAPPERIMPL_H

// End of File
