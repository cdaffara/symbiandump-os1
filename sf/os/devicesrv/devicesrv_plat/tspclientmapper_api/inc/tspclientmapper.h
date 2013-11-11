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


#ifndef TSPCLIENTMAPPER_H
#define TSPCLIENTMAPPER_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>

// CLASS DECLARATION

/**
* The tsp client mapper component is used to create mappings between remote
* control clients. The tsp client mapper is used in such cases where a client
* could not be correctly mapped by it's default information. For example if a
* remote control event is listened by the active audio client but the actual
* remote control client is in a different process than the one that is
* listening the remote control events. In this case a mapping has to be created
* between the active audio process and the remote control client. This mapping
* is created simply by setting the process identifier of the remote control
* client of the active audio to this api.
* Mapping types define the different states for the tsp clients. The states are
* defined in TTspClientMappingType. After one state is completed the mapping
* should be removed or set to another state. In the following an example
* where a value is set when a player is registered.
* @code
* CTspClientMapper* mapper = CTspClientMapper::NewLC();
* RThread client;
* RMessage2& message = aMessage;
* // Message from the client
* RProcess process;
* client.Process( process );
* TProcessId processId = proceess.Id();
* mapper->SetTspTargetClient( processId,
*                             CTspClientMapper::ERegisteredClients );
* CleanupStack::PopAndDestroy( mapper );
* @endcode
* 
* An example of changing the state of the remote control client:
*
* @code
* CTspClientMapper* mapper = CTspClientMapper::NewLC();
* RThread client;
* RMessage2& message = aMessage;
* // Message from the client
* RProcess process;
* client.Process( process );
* TProcessId processId = proceess.Id();
* mapper->SetTspTargetClientToOtherType( processId,
*                                        CTspClientMapper::EPlayingClients );
* CleanupStack::PopAndDestroy( mapper );
* @endcode
*
* An example of removing a tsp mapping client:
*
* @code
* CTspClientMapper* mapper = CTspClientMapper::NewLC();
* RThread client;
* RMessage2& message = aMessage;
* // Message from the client
* RProcess process;
* client.Process( process );
* TProcessId processId = proceess.Id();
* mapper->RemoveTspTargetClient( processId,
*                                CTspClientMapper::EPlayingClients );
* CleanupStack::PopAndDestroy( mapper );
* @endcode
* 
* @since S60 5.0
*/

class CTspClientMapper : public CBase
  {
public:
    
    friend class CRemConTspController;
    
    /**
    * Enumerations for tsp mapping types.
    *
    * EPlayingClients is used when the client is in playing state.
    * ERegisteredClients is used when the client is in registered state.
    * EStoppedClients is used when the client is in stopped state.
    */
    enum TTspClientMappingType
        {
        EPlayingClients = 0,
        ERegisteredClients,
        EStoppedClients
        };
    
    /**
    * Symbian two phased constructor.
    */
    IMPORT_C static CTspClientMapper* NewL();
    
    /**
    * Symbian two phased constructor. Puts the instance to cleanup stack.
    */
    IMPORT_C static CTspClientMapper* NewLC();
    
public: 

    /**
    * This function is used to set a target client for TSP. This way Target
    * Selector Plugin creates a mapping to the correct client.
    *
    * @param    aMappingType @see TspClientMappingType
    * @param    aProcessId is the process identifier of the client. If the
    *           process id is already in the array the process id that needs
    *           to be set is moved to the first place in the array.
    */
    virtual void SetTspTargetClient( 
        TTspClientMappingType aMappingType,
        TProcessId aProcessId ) = 0;
    
    /**
    * This function is used to set a TSP target client for another type then it
    * was originally set for. This way targets don't have to be removed from
    * one type and then added to another.
    *
    * @param    aMappingType is the type this client has to be set for
    * @param    aProcessId is the process identifier of the client
    * @return   KErrNotSupported if invalid mapping type. KErrNotFound if this
    *           ProcessId is not found from any of the arrays.
    */
    virtual TInt SetTspTargetClientToOtherType( 
        TTspClientMappingType aMappingType,
        TProcessId aProcessId ) = 0;
    
    /**
    * This function is used to remove a target client from TSP. This way Target
    * Selector Plugin creates a mapping to the correct client. The process id's
    * that are set to the tsp client mapper must be removed from the array
    * after the state is finished with this function. The mappings aren't
    * destroyed automatically in the destructor.
    *
    * @param    aMappingType @see TspClientMappingType
    * @param    aProcessId is the process identifier of the client
    * @return   KErrNotSupported if invalid mapping type, KErrNotFound if
    *           process id is not found in the array.
    */
    virtual TInt RemoveTspTargetClient(  
        TTspClientMappingType aMappingType,
        TProcessId aProcessId ) = 0;
    
    
    
private:
    
    /**
    * This function is used to get the audio clients that are currently
    * playing audio. The function is used by TSP.
    *
    * @param    aMappingType @see TspClientMappingType
    * @param    aPidArray is given as a reference. The playing audio clients
    *           are added to this array.
    * @return   aPidArray is returned with the asked process identifiers.
    *           KErrNotSupported if invalid mapping type.
    */
    virtual TInt GetTspTargetClients(  
        TTspClientMappingType aMappingType,
        RArray<TProcessId>& aPidArray ) = 0;

protected:    
    
    /**
    * Default Constructor 
    */
    CTspClientMapper();
    
    };

#endif // TSPCLIENTMAPPER_H

// End of File
