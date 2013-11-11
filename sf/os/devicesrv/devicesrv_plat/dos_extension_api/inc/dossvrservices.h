/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
*    This file contains the neccessary clases to access to the Dos Server
*    Services. Client side API.  
*
*/


#ifndef __DOSSVRSERVICES_H__
#define __DOSSVRSERVICES_H__

// INCLUDES
#include <e32base.h>
#include <PSVariables.h>
#include <bttypes.h> // TBTDevAddr, THCIConnHandle

/**
* This represents parameter info and it is used when transfering
* paramenter data between client and server in case of extended
* functions i.e. usage of RDosExtension API.
*/
struct TExtensionPar 
{
	// Function flag used to identify between different RDosExtension
	// function calls (multiple extensions are supported at the same time).
	TInt iFunc;

	// Length or size of the parameter data.
	// Can not be negative value, otherwise call to CallFunction will 
	// return KErrGeneral.
	// Server needs to know the size to be able to allocate memory from heap.
	TInt iParLength;

	// Auto complete flag.
	// ETrue if the function is completed by the server, 
	// EFalse if it's gonna be completed by the DSY.
	TBool iAutoComplete;
};

// Convenience type definitions to wrap package buffers:
typedef TPckgBuf<TExtensionPar> TExtensionParPckg;

/**
* This class represents a session with Dos Server and has some general
* purpose services.
*/
class RDosServer : public RSessionBase
{
public:
	/**
	* Starts the server.
	* @return Symbian error code.
	*/
	IMPORT_C TInt StartServer() const;
	
	/**
	* Connects and registers a session to ther server.
	* If the server has not been started before a call to this function
	* will start it.
	* @return Symbian error code.
	*/
	IMPORT_C TInt Connect();

	/**
	* Closes the session.
	*/
    IMPORT_C void Close();

	/**
	* Shuts down the server.
	*/
	IMPORT_C void ServerShutdown() const;
};

/**
* This the base class for all the Dos Server subsessions.
*/
class RDosSubSession : public RSubSessionBase
{
public:
	/**
	* Closes the subsession.
	*/
	IMPORT_C void Close();

protected:
    // Deprecated 
	TPtr8* NewPtr(TAny* aDataPtr,TInt aDataSize);

protected:
	RPointerArray<TPtr8>* iParArray;
};

/**
* This subsession offers possibilities for licensees to extend the functionality of the 
* dos server.
*/
class RDosExtension : public RDosSubSession
{
public:
	/**
	* Opens the subsession.
	* @param aServer A connected session with the Server.
	* @return Symbian error code.
	*/
	IMPORT_C TInt Open(RDosServer& aServer);

	//================================================
	// Subsession services
	//================================================

protected:
	/**
	* Deprecated, please use the overloaded function taking descriptors;
	* TInt CallFunction(const TExtensionParPckg& aParamInfo, TDes8& aParameter)
	*
	* Implements a synchronous raw function call that has to be implement in the Dsy plug-in.
	* Internal implementation creates temporary descriptors and calls the overloaded 
	* CallFunction -function taking TExtensionParPckg and TDes8 parameters.
	* @param aFunc Unique integer among the extended functions what function we are calling.
	* @param aParameter A pointer to the parameter.
	* @param aParLength Length of the parameter.
	* @param aAutoComplete ETrue if the function is completed by the server, EFalse if it's gonna be completed
	* in the plug-in.
	* @return Symbian error code.
	*/
	IMPORT_C TInt CallFunction(TInt aFunc, TAny* aParameter=NULL, TInt aParLength=0,TBool aAutoComplete=ETrue);

	/**
	* Implements a function call that has to be implement in the Dsy plug-in
	* @param aParamInfo Package buffer containing parameter information (see TExtensionPar).
	* @param aParameter Reference to a descriptor containing parameter data.
	* @return Symbian error code.
	*/
	IMPORT_C TInt CallFunction(const TExtensionParPckg& aParamInfo, TDes8& aParameter);

	/**
	* Implements an asynchronous raw function call that has to be implement in the Dsy plug-in
	* @param aStatus Status variable to control the completion of the asynchronous request.
	* @param aParamInfo Package buffer containing parameter information (see TExtensionPar).
	* @param aParameter Reference to a descriptor containing parameter data.
	* @return Symbian error code.
	*/
	IMPORT_C void CallFunction(TRequestStatus& aStatus, const TExtensionParPckg& aParamInfo, TDes8& aParameter);
};

/**
* This enumeration describes the different types of queues when registering for events.
*/
enum TQueueType
{
	ENoQueue, //The incoming events when the client is busy will be discarded.
	EQueue, //The incoming events when the client is busy will be queued.
	EOnlyLast //Only the last incoming event will be queued when the client is busy, deleting the previous queued one.
};


/**
* This type represents an event when the client registers for it.
*/
struct TRegisterEvent
{
	// The unique identifier of the event. 
	TUint iEvent;

	// The size of the event's parameter.
	TInt iParamSize;

	// The type of queue that the client wishes to have for the event.
	TQueueType iQueue;
};

/**
* This subsession offers services to register and listen for events.
* It should not be used directly. Instead derive your class from CDosEventListenerBase
*/
class RDosEventRcv : public RDosSubSession
{
public:
	/**
	* Constructor
	*/
	RDosEventRcv();

	/**
	* Desctructor
	*/
	~RDosEventRcv();

	/**
	* Opens the subsession.
	* @param aServer A connected session with the Server.
	* @return Symbian error code.
	*/
	TInt Open(RDosServer& aServer);

	/**
	* Informs the server that the client wished to receive an event.
	* @param aEvent The unique identifier of the event.
	* @param aParamSize The size of the parameter that event is going to carry.
	* @param aQueue The type of queue that the event is going to have.
	* @return Symbian error code.
	*/
	TInt Register(TInt aEvent,TInt aParamSize,TQueueType aQueue);

	/**
	* It asynchronously tells the server that the client doesn't wish to receive the event anymore.
	*/
	void UnRegister();

	/**
	* Puts the client to wait for the event.
	* @param aStatus Status varibale that informs about the incoming events.
	* @return Symbian error code.
	*/
	TInt WaitEvent(TRequestStatus& aStatus) const;

	/**
	* Cancels outstanding requests.
	* @return Symbian error code.
	*/
	TInt CancelWaitEvent() const;

	
	/**
	* Returns a pointer to the buffer containing the parameter that came with the latest received event.
	* @return A pointer to the paramater buffer.
	*/
	inline HBufC8* Parameter() const;

	/**
	* Returns the size of the parameter that came with the latest received event.
	* @return Size of the parameter.
	*/
	inline TInt ParameterSize() const;

	/**
	* Returns the type of queue that the event has.
	* @return The type of queue.
	*/
	inline TQueueType Queue() const;

	/**
	* Returns the unique id of the event that the object is listening for.
	* @return An event identifier.
	*/
	inline TUint Event() const;

private:

	// Pointer to the buffer containing the parameter of the latest received event.
	HBufC8* iParamContent;

	//A Descriptor pointer pointing to iParamContent.
	TPtr8* iContentPtr;

	//Indicates if the client has been already registerd or not.
	TBool iRegistered;

	//Queue type for the event.
	TQueueType iQueue;

	//The event.
	TUint iDosEvent;
};

//
// Events Constants
//

class CDosEventListenerBase; //Forward declaration

/**
* This active object is used to wait for the event provided by the server throught the class RDosEventRcv.
* It is not recomended to use this class directly but instead use a derived class from CDosEventListenerBase.
*/
class CDosEventNotifier : public CActive
{
public:
	/**
	* Constructor.
	* @param aListener A pointer to the CDosListenerBase-derived object that owns this object.
	* @param aPriority The priority of the CActive object.
	*/
	CDosEventNotifier(CDosEventListenerBase* aListener,TPriority aPriority=EPriorityStandard);

	/**
	* Destructor.
	*/
	~CDosEventNotifier();

	/**
	* NewL that creates the object.
	* @param aServer A connected session to the DosServer.
	* @param aListener A pointer to the CDosListenerBase-derived object that owns this object.
	* @return A pointer to the created CDosEventNotifier object.
	*/
	static CDosEventNotifier* NewL(RDosServer& aServer,CDosEventListenerBase* aListener);

	/**
	* Registers the event in the Server and starts the listening for incoming events.
	* @param aEvent Event to listen to.
	* @param aParamSize Size of the parameter that the event is going to carry.
	* @param aQueue Desired type of the event queue.
	*/
	void StartListeningL(TUint aEvent,TInt aParamSize,TQueueType aQueue);

private:
	/**
	* Symbian two-phased constructor.
	* @param aServer A connected session to the DosServer.
	*/
	void ConstructL(RDosServer& aServer);

private: //Inherited from CActive

	/**
	* It's called by CActive on error situations.
	* @param aError Symbian error code.
	* @return Returns KErrNone if the error was handled or aError otherwise.
	*/
	TInt RunError(TInt aError);

	/**
	* It's called by CActive when the active object is scheduled to be executed.
	*/
	void RunL();

	/**
	* It's called by CActve when Cancel() is called and process the cancelation of the request.
	*/
	void DoCancel();

public:
	// To part of a list of Notifiers
	TSglQueLink iLink; 
private:
	//The RDosEventRcv subsession with the server.
	RDosEventRcv iReceiver;

	//A pointer to the owner.
	CDosEventListenerBase* iListener;
};



/**
* This class provides with services to listen for events.
* The class is intended for derivation.
* The derived class should override all the events that its objects are going to register to.
*/
class CDosEventListenerBase : public CBase
{
public:
	/**
	* Constructor.
	* @param aServer A connected session to the DosServer.
	*/
	IMPORT_C CDosEventListenerBase(RDosServer& aServer);

	/**
	* Constructor. It creates the sessions to the DosServer.
	*/
	IMPORT_C CDosEventListenerBase();

	/**
	* Destructor.
	*/
	IMPORT_C ~CDosEventListenerBase();

	/**
	* It registers the object to all the events contained in aEventArray.
	* @param aEventArray An array of event for registration.
	* @param aEventCount Number of events in the array.
	*/
	IMPORT_C void StartListeningL(TRegisterEvent* aEventArray, TInt aEventCount);

	/**
	* It registers only one event.
	* @param aEvent Unique Id of the event.
	* @param aParamSize Size of the parameter that the event carries.
	* @param aQueue Type of queue wished for the event.
	*/
	IMPORT_C void StartListeningL(TUint aEvent,TInt aParamSize=0, TQueueType aQueue=EQueue);

	/**
	* Stops the listening.
	*/
	IMPORT_C void Stop();

	/**
	* The function gets called whenever an error occurs and it can be overrided by the derived class.
	* @param aError Symbian error code.
	* @param aStopListening Put this parameter as ETrue if you wish the Listener to stop listening for the event
	* that caused the error.
	*/
	IMPORT_C virtual void OnError(TInt aError, TBool& aStopListening);

	/**
	* Returns the session with the DosServer.
	* @return A session with the DosServer.
	*/
	inline const RDosServer& DosServer() const;

protected:

	//================================================
	// Events: To be overriden by the derived class
	//================================================

	/**
	* This a raw event that is intended for expansion of the event functionality by licensees.
	* @param aEvent The event.
	* @param aParameter A pointer to the parameter.
	*/
	IMPORT_C virtual void GeneralEventL(const TRegisterEvent& aEvent,TAny* aParameter);

private:

	/**
	* This function handles the raw event and calls one of the "event functions".
	* @param aEvent The event.
	* @param aParameter A pointer to the parameter.
	*/
	void HandleEventL(const TRegisterEvent& aEvent,HBufC8* aParameter);

	/**
	* Register the events and start listening for them.
	*/
	void RegisterAndListenL();

private:
	friend class CDosEventNotifier;

private:
	//A list of notifiers listening to this event.
	TSglQue<CDosEventNotifier> iNotifierList; 

	//The TSglQueIter class to browser iNotifierList.
    TSglQueIter<CDosEventNotifier> iNotifierIter;

	//A pointer to the event list.
	TRegisterEvent* iEventList;

	//Number of events to listen to.
	TInt iNotifierCount;

	//Session with the DosServer.
	RDosServer iDosServer;

	//To check if the server was connected by this object
	TBool iOwnServer;
};

#include "DosSvrServices.inl"


#endif // __DOSSVRSERVICES_H__
