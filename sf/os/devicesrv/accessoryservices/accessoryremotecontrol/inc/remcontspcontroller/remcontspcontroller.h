/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements Target Selector Plugin (TSP) used by Symbian's 
*				 Remote Control Framework. TSP controls sending of operations
*			     between Series 60 components and remote targets.
*
*/


#ifndef CREMCONTSPCONTROLLER_H
#define CREMCONTSPCONTROLLER_H

//  INCLUDES
#include <e32base.h>
#include <remcontargetselectorplugin.h>
#include <remcontargetselectorplugininterface.h>
#include <e32property.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CRemConLogger;
class TClientInfo;

// CLASS DECLARATION

/**
*  Controls delivering of commands between Series 60 components and remote 
*  targets.
*
*  @lib RemConTspController.dll
*  @since Series 60 3.0
*/
class CRemConTspController : public CRemConTargetSelectorPlugin,
			     public MRemConTargetSelectorPluginInterfaceV2
	{
	public:		// Constructors and destructor
		
		/**
        * Two-phased constructor.
        */
		static CRemConTspController* NewL( 
			MRemConTargetSelectorPluginObserver& aObserver );
		
		/**
        * Destructor.
        */
		virtual ~CRemConTspController();
	
	public: 	// Functions from base classes
	
	protected:  // New functions
	
	protected:  // Functions from base classes
	
	private:
		
		/**
        * C++ default constructor.
        */
        CRemConTspController( MRemConTargetSelectorPluginObserver& aObserver );
                
	private: // from CRemConTargetSelectorPlugin
		
		/**
        * Called by RemCon server to get a pointer to an object which 
        * implements the TSP interface with UID aUid. This is a mechanism 
        * for allowing future change to the TSP API without breaking BC 
        * in existing (non-updated) TSPs.
        * @since Series 60 3.0
        * @param aUid	Queried TSP interface version.
        * @return TAny	A pointer to implementation of this TSP version, if 
        *				quaried TSP interface version is the one implemented 
        *				by this TSP. Otherwise a NULL will be returned.	
        */
		TAny* GetInterface( TUid aUid );

	private: // from MRemConTargetSelectorPluginInterface
	
		/**
		* Called by RemCon server to get the TSP to address an outgoing 
		* command (from a connectionless controller client) to zero or 
		* more remote targets.
		* @since Series 60 3.0
		* @param aInterfaceUid 		The UID of the client interface.
		* @param aOperationId 		The operation ID of the command.
		* @param aSender The 		TClientInfo of the sending session.
		* @param aConnections 		An empty collection of connections. 
		*							TRemConAddresses must be made on the heap.
		*							RemCon takes ownership of any items in the 
		*							collection. 
		* @params aBearerSecurity 	Contains all the bearer security policies.
		* @return TAny				void 
		*/
		void AddressOutgoingCommand(
							TUid aInterfaceUid,
							TUint aOperationId, 
							const TClientInfo& aSender,
							TSglQue<TRemConAddress>& aConnections,
							TSglQue<TBearerSecurity>& aBearerSecurity );
		
		/**
		* Called by RemCon server to find out from the TSP whether the given 
		* connection-oriented controller client is permitted to send the given 
		* command to remote target.
		* @since Series 60 3.0
		* @param aInterfaceUid 		The UID of the client interface.
		* @param aOperationId 		The operation ID of the command.
		* @param aSender 			The TClientInfo of the sending session.
		* @param aConnection 		The remote the command will be sent over if
		*							permission is granted.
		* @return void				 
		*/	
		void PermitOutgoingCommand(
							TUid aInterfaceUid,
							TUint aOperationId, 
							const TClientInfo& aSender,
							const TRemConAddress& aConnection );
		
		/** 
		* Called by RemCon server to cancel the current AddressOutgoingCommand 
		* or PermitOutgoingCommand command.
		* @since Series 60 3.0
		* @return void 
		*/					
		void CancelOutgoingCommand();
	
		/**
		* Called by RemCon server to get the TSP to address an incoming command 
		* (from a remote target to zero or more Series 60 components). There's 
		* no 'cancel' method for AddressIncomingCommand.
		* @since Series 60 3.0
		* @param aInterfaceUid 	The UID of the client interface.
		* @param aOperationId 	The operation ID of the command.
		* @param aClients 		A collection of existing target clients. TSP's 
		*						responsibility is to modify this parameter, if 
		*						it wants that command will not be sent to all 
		*						proposed clients. Thus, when this method has 
		*						been finished, aClients will include those 
		*						target clients to whom command should be sent.
		* @return void  
		*/
		void AddressIncomingCommand(
							TUid aInterfaceUid,
							TUint aOperationId, 
							TSglQue<TClientInfo>& aClients );
	
	/**
	* Called by RemCon to get the TSP to decide which client should be
	* allowed to respond to a command.
	* @param aInterfaceUid The UID of the client interface.
	* @param aOperationId The operation ID of the command.
	* @param aClient The client which is trying to send a response
	* @param aClients A list of clients which are still expected to respond,
	*         including the one specified in aClient
	*/
	void PermitOutgoingResponse(
			TUid aInterfaceUid,
			TUint aOperationId,
			const TClientInfo& aClient,
			TClientInfoConstIter& aClients); 

	/** 
	* Called by RemCon to cancel the current PermitOutgoingResponse request.
	*/
	void CancelOutgoingResponse(); 
	
	/**
	* Called by RemCon to get the TSP to address an incoming notify (from a 
	* remote to zero or more target clients). 
	* @param aInterfaceUid The UID of the client interface.
	* @param aOperationId The operation ID of the command.
	* @param aClients A collection of existing target clients. The implementor 
	*         may wish to start a target client and call IncomingNotifyAddressed
	*         with a new TClientInfo. The new TClientInfo item must be made on the stack. 
	*         Note when creating the new TClientInfo, only the process ID needs to 
	*         be correctly populated.
	*/
	void AddressIncomingNotify(
			TUid aInterfaceUid,
			TUint aOperationId,
			TSglQue<TClientInfo>& aClients); 

	public:     // Data
	
	protected:  // Data
	
	private:    // Data
		
		// Interface to P&S key that returns call state
        RProperty iProperty;
	
	public:     // Friend classes
	
	protected:  // Friend classes
	
	private:    // Friend classes

	};

#endif		// CREMCONTSPCONTROLLER_H
