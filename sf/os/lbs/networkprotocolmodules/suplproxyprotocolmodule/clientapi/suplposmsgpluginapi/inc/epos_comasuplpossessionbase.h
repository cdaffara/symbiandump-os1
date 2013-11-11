/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provides the interface between SUPL Gateway and SUPL Protocol Handler. 
				 This class provides the functionality of loading and unloading 
				 the Protocol handler ECom plug-in.
*
*/


#ifndef C_COMASUPLPOSSESSIONBASE_H
#define C_COMASUPLPOSSESSIONBASE_H

#define S60_SUPL_POS_MSG_PLUGIN_API_VERSION 1

#include <e32base.h>


// Forward Declaration
class COMASuplPosPayload;
class COMASuplPosition;
class COMASuplInfoRequestList;
class MOMASuplObserver;
class TOMASuplAllowedCapabilities; 

/**
* The base class for the POS Session to be defined
* by the POS Message plug-in. 
*
* POS Session in the adaptation POS Message plug-in facilitates the computation 
* of the location of the mobile terminal by forwarding control plane positioning 
* protocol messages from and to the SUPL Framework. SUPL Framework provides the
* secure IP connection required to exchange the control plane positioning messages
* between the SET and network.
* 
*
* @since Series 60 3.1u
*/

 
class COMASuplPosSessionBase : public CBase
    {
    
public: // Constructors and destructor

	/**
     * @Description COMASuplPosHandlerBase implementation should implement the 
     * overloaded constructor that takes the MOMASuplObserver as the 
     * parameter. The MOMASuplObserver from the COMASuplPosHandlerBase::CreateNewSessionL
     * should be set in COMASuplPosHandlerBase implementation
     *
     * @param aSuplObserver The observer for the COMASuplPosHandlerBase implementation
     *	This is used to retrieve SUPL Version and POS Message plugin can terminate SUPL Session
     *  in SUPL Framework using this observer.
     */
   IMPORT_C COMASuplPosSessionBase(MOMASuplObserver *aSuplObserver);

public: //Enumerations
	
	/**
	  * @Description Enumeration for SUPL message type. SUPL Framework informs
	  * POS Message plug-in about the change in state of SUPL Session through 
	  * a combination of values in TOMASuplMsg and TOMASuplMsgState. 
	  * For example, when SUPL Framework sends SUPL START to SLP, SUPL Message type
	  * is set to EOMASuplStart and Message state is set to EOMASuplSent.
    * SUPL Framework invokes SetSuplState to set the state of SUPL Session.
    * POS Message plug-in can invoke GetSuplState the retrieve the state
    * of SUPL Session in SUPL Framework
    */
	enum TOMASuplMsg
		{
		EOMASuplStart = 0,
		EOMASuplResponse,
		EOMASuplInit,
		EOMASuplPosInit,
		EOMASuplPos,
		EOMASuplEnd	
		};
	/**
	  * @Description Enumeration for SUPL message states.
	  */
	enum TOMASuplMsgState
		{
			EOMASuplGeneric = 0,
			EOMASuplSent,
			EOMASuplReceived,
			EOMASuplCreating,
			EOMASuplError
		};
	/**
	 * @Description Enumeration for SUPL positioning method to be used.
	 */
	enum TOMASuplPositioningMethod
		{
		EPositionUndefined = 0,
		ENoPosition,
		EGpsSETAssisted,
		EGpsSETBased,
		EGpsSETBasedPreferred,
		EAutonomousGps,
		EAFLT,
		EeOTD,
		EoTDOA,
		EGpsSETAssistedPreferred
		};	

public:
 
	/**
     * @Description Asynchronous method that initializes the COMASuplPosSessionBase implementation.
     * InitializeL is invoked by SUPL framework either after POS session object creation 
     * or when the POS session object needs re-initialization. There will be instances 
     * when the SUPL framework will re-use the POS session object with a new SUPL session. 
     * Under such conditions, SUPL framework re-initializes the POS session object with 
     * InitializeL method. 
     *
     * When the SUPL protocol is used to tunnel the control plane location protocol, there 
     * is a RequestID sent to the SUPL subsystem through the SUPL Terminal Initiation API, 
     * by the licensee Initiation Client. SUPL Framework forwards this RequestID to the POS 
     * Message plug-in so that the SUPL Session meant to carry messages for a particular 
     * control plane session is identified. 
     *
     * If the licensee Initiation Client does not initiate the SUPL Session, then SUPL 
     * Framework sets RequestID to a default value of zero. 
     *
     * If the SUPL Framework wants to re-use an existing POS Session for a new SUPL Session, 
     * it re-initializes the POS Session object with a RequestID provided by SUPL Client.
     * Soon after creation of SUPL POS Session, the SUPL Framework has to invoke 
     * InitializeL(). If SUPL Framework performs any operation before invoking InitializeL(), 
     * POS Message plug-n returns an error value, KErrOMASuplPosInActive. 
     *
     * Similarly, when the SUPL Framework wants to re-use a terminated POS Session 
     * (COMASuplPosSessionBase::SessionEnd()) for a new SUPL Session, it has to invoke InitializeL() 
     * before performing any other operation on the POS Session. If SUPL Framework invokes 
     * any other method like GetSuplInfoL() on a terminated session, POS Session will return 
     * KErrOMASuplPosInActive as the error value.
	 *
	 * At any given instance of time, there can be one and only one outstanding asynchronous
	 * request from SUPL Framework to POS Message plug-in. When SUPL Framework wants to 
	 * re-initialize a POS Session, SUPL Framework will cancel any possible outstanding 
	 * request to POS Session and then invoke InitializeL.
	 *
     * @param aRequestID A unique identifier that identifies the 
     * POS Session with a particular SUPL Session
     * @param aStatus Staus for the Asynchronous request to indicate completion of 
     * InitializeL. 
     * @return If initialization of POS Session is successful, POS Message plug-in should
     * set  aStatus to KErrNone. If POS Message plug-in is unable to initialize the POS Session,
     * POS Message plug-in should set aStatus to KErrGeneral.
     */
        
    virtual void InitializeL(const TInt aRequestID, TRequestStatus &aStatus) = 0;

		/**
     * @Description Cancellation method for InitializeL of POS Session .
     * If request cancellation is successful, POS Message plug-in completes the original request 
     * with KErrCancel. If there is no outstanding request, no action is taken.
     */
    virtual void CancelInitialize() = 0;


	/**
     * @Description Method to handle the POS Payload in COMASuplPosHandlerBase implementation.
     * HandleSuplPosMessageL is a synchronous method and is invoked by the SUPL 
     * framework when it receives a SUPL POS message from the network. 
     * HandleSuplPosMessageL is responsible for processing the POS Payload 
     * in the SUPL POS Message. 
     *
     * When SUPL Framework receives SUPL POS Message from the network, the POS Payload 
     * is forwarded to POS Message plug-in by invoking the HandleSuplPosMessageL method. 
     * HandleSuplPosMessageL is a synchronous method and POS Message plug-in should 
     * make a copy of the SUPL POS Payload and return control to SUPL Framework 
     * immediately to facilitate processing of other SUPL POS messages 
     * that the SUPL Framework may receive from the network. SUPL Framework
     * destroys the SUPL POS message payload as soon as POS Message plug-in
     * returns the HandleSuplPosMessageL call.
     *
     * As a guideline, if there is long processing involved in the implementation 
     * of HandleSuplPosMessageL, it should make use of active objects. The entire 
     * POS Payload is provided to the POS Message plug-in as an argument. 
     * HandleSuplPosMessageL does not have a return value and therefore, SUPL Framework 
     * does not handle any POS payload processing errors.
     *
     * @param aPosPayload POS Payload to be handled by the POS Message plug-in
     */
    virtual void HandleSuplPosMessageL(const COMASuplPosPayload *aPosPayload) = 0;
    
    
	/**
     * @Description Asynchronous method that provides different POS parameters to SUPL Framework.
     * GetSuplInfoL is an asynchronous method and is invoked by the SUPL framework to 
     * get information about location Capabilities supported by the control plane positioning 
     * protocol. Assistance Data Request, Velocity, Position estimates, POS Payload and 
     * Position are the other parameters that SUPL Framework requests for. The information provided 
     * by GetSuplInfoL is used in different SUPL messages. 
     *
     * SUPL Framework can retrieve a list of information elements from the POS module using this 
     * method. For example, when SUPL Framework is generating SUPL START or SUPL POS INIT 
     * message, it requires the set of capabilities supported by the underlying control plane 
     * positioning protocol. 
     *
	 * POS Message plug-in should handle the COMASuplInfoRequestList as described below
	 * - Iterate through the COMASuplInfoRequest objects in COMASuplInfoRequestList
	 * - Determine the type of data object (COMASuplInfoRequest::iType)
	 * - Use the Set method to fill up appropriate values in COMASuplInfoRequest
	 * - Complete the request once all the values are filled up
	 *
     *
     * @param aInfoRequestList The List of data elements to be filled up by POS Message Plug-in
     *
     * @param aStatus Status for the asynchronous request
     *
     * @return If the POS Message plug-in cannot provide value for any of the mandatory fields 
     * in the data object due to an erroneous condition, the iStatus data member of 
     * COMASuplInfoRequest is set to KErrOMASuplPosInfo. 
     * If there is an error in any of the COMASuplInfoRequest objects in the COMASuplInfoRequestList, 
     * then the GetSuplInfoL should be completed with KErrOMASuplPosInfo.
     * 
     * If the POS Message plug-in is in an inactive state due to a call to 
     * COMASuplPosSessionBase::SessionEnd() and SUPL Framework invokes GetSuplInfoL(), then, POS 
     * Message plug-in should complete the GetSuplInfoL with KErrOMASuplPosInActive. 
     * When the status returned by GetSuplInfoL is KErrOMASuplPosInacive, SUPL Framework discards 
     * the different data type objects in COMASuplInfoRequestList. 
     * 
     * If the POS Message plug-in is able to provide all the mandatory parameters for all the data 
     * types present in COMASuplInfoRequestList, then, GetSuplInfoL should be completed with KErrNone.
     *
     * If the POS Message being sent is the last POS message, then, the request is completed with KErrNone and 
     * iReqStatus in COMASuplInfoRequest is set to KErrOMASuplLastPosMsg
     */
        
    virtual void GetSuplInfoL(COMASuplInfoRequestList *aInfoRequestList,
    						  TRequestStatus &aStatus) = 0;
    
	/**
     * @Description Cancellation method for GetSuplInfoL.At any given point, for a particular SUPL Session, 
     * there can be one and only one outstanding request from SUPL Framework to POS Message Plug-in. 
     * If there is an abnormal termination of SUPL Session, the SUPL Framework invokes the cancellation 
     * methods to cancel the outstanding request. 
     * If request cancellation is successful, POS Message plug-in completes the original request 
     * with KErrCancel. If there is no outstanding request, no action is taken.
	   *
     */
	     
    virtual void CancelSuplInfoRequest() = 0;

	/**
     * @Description Asynchronous method to retrieve Position information from POS 
     * Message plug-in. SUPL Framework typically gets the position information in 
     * SUPL END message. However, position is an optional field in SUPL END and 
     * if position is not available in SUPL END, SUPL Framework requests the POS Message 
     * plug-in for position information. 
     *
     * This method returns an object of COMASuplPosition that contains the 
     * position information. 
     * Please note that SUPL Framework does not invoke this method to retrieve the position 
     * field in SUPL POS INIT message. Instead, GetSuplInfoL is invoked. 
	   *
     * @param aStatus Status for the asynchronous request
     *
     * @param aPosition Position information to be filled by the POS Message plugin
     *
     * @return If the POS Message plug-in does not have any position information, 
     * the request is completed with a KErrOMASuplPosInfo. If the position information is 
     * present, GetPositionL request is completed with KErrNone.
     */
        
    virtual void GetPositionL(TRequestStatus &aStatus, COMASuplPosition* aPosition) = 0;
    
	/**
     * @Description Cancellation method for GetPositionL. At any given point, for a particular SUPL Session, 
     * there can be one and only one outstanding request from SUPL Framework to POS Message Plug-in. 
     * If there is an abnormal termination of SUPL Session, SUPL Framework invokes the cancellation 
     * methods to cancel the outstanding request. 
     * If request cancellation is successful, POS Message plug-in completes the original request 
     * with KErrCancel. If there is no outstanding request, no action is taken.
	   *
     */
	     
    virtual void CancelGetPosition() = 0;

	/**
     * @Description Method to End the SUPL POS Session. SessionEnd is a synchronous method invoked 
     * by SUPL framework to end the SUPL Session, during which the POS Session object 
     * may perform clean up operation for releasing the resources. 
     *
     * If there is an outstanding asynchronous request from SUPL Framework, it is the 
     * responsibility of the POS Message plug-in to Cancel the request when SessionEnd is invoked 
     * 
     * SessionEnd method indicates that the SUPL Session has come to an end and 
     * the only operations that can be performed after this 
     * method call is InitializeL. When SUPL Framework invokes SessionEnd, the POS Message 
     * plugin should not delete the POS Session.
     *
     */
    virtual void SessionEnd() = 0;
    
   	/**
     * @Description Method to retrive the Supl Observer set through the 
     * overloaded constructor
     *
     */

    IMPORT_C virtual MOMASuplObserver* SuplObserver();
    
    /**
    * @Description POS Message plug-in can fetch SUPL Session state from the SUPL Framework by invoking 
    * COMASuplPosSessionBase::GetSuplState. 
    * 
    * @param aSuplMsg indicates the SUPL Message currently being processed by the SUPL Framework 
    * 
    * @param aSuplMsgState provides the state of the message like Created, Sent, Received.
    */
    
    IMPORT_C void GetSuplState(TOMASuplMsg& aSuplMsg,TOMASuplMsgState& aSuplMsgState) const;
    
    /**
    * @Description Whenever there is a change in state of the SUPL Session in SUPL Framework, SUPL 
    * Framework sets the SUPL State in COMASuplPosSessionBase using the SetSuplState method. 
    *
    * @param aSuplMsg indicates the SUPL Message that is currently being processed by 
    * the SUPL Framework. 
    *
    * @param aSuplMsgSate indicates the state of the SUPL Message like Created, Sent, 
    * Received.
    */
    IMPORT_C void SetSuplState(TOMASuplMsg aSuplMsg,TOMASuplMsgState aSuplMsgState);
    
    /**
     * @Description A wrapper for ECom object destruction. Notifies the ECOM
     *              framework that specific instance is being destroyed.
     */
    IMPORT_C  ~COMASuplPosSessionBase();

   /**
     * @Description Synchronous method that is used to set the allowed capabilites and 
     * the positioning method. 
     * SetPosMethodAndAllowedCapabilities is a synchronous method and is invoked by the SUPL 
     * framework to set allowed capabilites and the positioning method. 
     * 
     * SUPL Framework can set the allowed capabilites and positioning method for use 
     * by the Pos Message Plugin using this method.
     * 
     * @param aAllowedCapabilities The allowed capabilites for the client.
     * 
     * @param aPosMethod The positioning  method to be used when making the position request.
     */
    IMPORT_C virtual void SetPosMethodAndAllowedCapabilities( TOMASuplAllowedCapabilities aAllowedCapabilities, TOMASuplPositioningMethod aPosMethod);
    
   /**
     * @Description Synchronous method that is used to set the SLP Address of the server used.
     * SetSLPAddressUsed is a synchronous method and is invoked by the SUPL 
     * framework to set the SLP Address of the server being used in current session. 
     * 
     * SUPL Framework can set the SLP Address being used for this session for use 
     * by the Pos Message Plugin through this method.
     * 
     * 
     * @param aServerAddress The SLP Address of the server that is being currently used for this session.
     */
    IMPORT_C virtual void SetSLPAddressUsed(const TDesC& aServerAddress);
private:

 	/**
     *	By default, prohibit copy constructor
	 */
         
    COMASuplPosSessionBase();
    
   
private:

    MOMASuplObserver *iSuplObserver;
    
    TOMASuplMsg iSuplMsg;
    
    TOMASuplMsgState iSuplMsgState;
             
    };


#endif // EPOS_COMASUPLPOSSESSIONBASE_H

// End of File