// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @publishedPartner
*/


#ifndef __MLTSYDISPATCHFACTORY_H_
#define __MLTSYDISPATCHFACTORY_H_

// Includes
#include <e32base.h>

#include <ctsy/mltsyfactorybase.h>
#include <ctsy/ltsy/mltsydispatchinterface.h>

class CCtsyDispatcherCallback;
class MCtsySatService;
class CTsySatMessagingBase;


class MLtsyDispatchFactoryV1 : public MLtsyFactoryBase
/**
 * This interface should be implemented by the Licensee LTSY and is used by the 
 * CTSY Dispatcher.  
 * 
 * It allows the CTSY Dispatcher to retrieve handlers that are provided by the LTSY
 * to handle rwequests made by the CTSY.  
 */
	{
public:

	/**
	 * Complete construction of the interface. This function will be invokesd by the CTSY Dispatcher to 
	 * allow the LTSY to create the various handler objects prior to any rquests being sent or received between the
	 * CTSY Dispatcher and LTSY.   	 
	 * 
	 * @param Reference to a CCtsyDispatcherCallback object used by the handlers
	 * in the Licensee LTSY to send request completions
	 * and indications to the CTSY Dispatcher.
	 * 
	 */
	virtual void ConstructL(CCtsyDispatcherCallback& aCallback) = 0;

	/**
	 * Get a reference to a pointer to a specified dispatch handler interface.
	 * 
	 * This function should be implemented by the Licensee LTSY to enable the 
	 * CTSY Dispatcher to retrieve a pointer to a specified 
	 * dispatch handler.  The dispatch handlers in the LTSY provide a functions 
	 * that enable the CTSY Dispatcher to request that the LTSY service a request. 
	 * 
	 * @param aInterfaceId The dispatch interface identifier of the requested
	 * interface. E.g. MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId
	 * 
	 * @param aDispatchInterface When this function completes, will contain
	 * a reference to a pointer to the dispatch handler interface in the LTSY.
	 * 
	 * @return KErrNone if the requested interface was returned in the
	 * aDispatchInterface parameter; KErrNotSupported if the requested interface
	 * is not supported by the Licensee LTSY.
	 */
	virtual TInt GetDispatchHandler(
			TLtsyDispatchInterfaceApiId aDispatchInterfaceId,
			TAny*& aDispatchInterface) = 0;
	
	/**
	 * Query support for specific dispatch handler in Licensee LTSY
	 * 
	 * This function should be implemented by the Licensee LTSY to enable the CTSY Dispatcher 
	 * to query whether the Licensee LTSY supports a particular dispatch handler 
	 * e.g. MLtsyDispatchPhoneBootNotifyModemStatusReady.
	 *  
	 * @param aFuncUnitId Identifier to functional unit being queried
	 * @param aDispatchApiId Identifier to dispatch API handler being queried
	 * @return result indicating whether dispatch handler is supported by LTSY
	 */
	virtual TBool IsDispatchInterfaceSupported(TInt aFuncUnitId, TLtsyDispatchInterfaceApiId aDispatchApiId) = 0;
	
	/**
	 * Query support in Licensee LTSY triggering the callback indicator APIs
	 * 
	 * This function should be implemented by the Licensee LTSY to enable the CTSY Dispatcher 
	 * to query whether the Licensee LTSY supports a particular callback indicator. 
	 * E.g. CCtsyDispatcherCallback::CallbackPhoneNotifyNitzInfoChangeInd.  If a Licensee LTSY does not support
	 * receiving the event information from the baseband it will not be able to invoke the callback and should 
	 * return false the query callback supported request.  
	 * 
	 * If the Licensee LTSY does not support a particular callback it can result in some 
	 * client notifier requests being completed with KErrNotSupported.
	 * 
	 * Example: IsCallbackIndicatorSupported(KDispatchCallControlFuncUnitId, EIndIdGroup1) should populate the aIndIdBitMask
	 * with a bit set for each indicator callback that is supported. 
	 * So, 0x0 implies none supported, 0xFFFF implies every callback indicator supported.
	 *   
	 * @param aFuncUnitId Identifier to functional unit being queried
	 * @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1
	 * @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
	 */
	virtual void IsCallbackIndicatorSupported(TInt aFuncUnitId, TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask) = 0;
	
	/**
	 * This function should be implemented by the Licensee LTSY to load a  
	 * SAT TSY.
	 * 
	 * @return a pointer to a SAT TSY
	 * 
	 * @param aSatMessagingService refenrence to CTSY side SAT services
	 */
	virtual CTsySatMessagingBase* GetSatMessagingService(MCtsySatService& aSatMessagingService)	= 0;
	
	}; // class MLtsyDispatchFactoryV1
	
	
#endif // __MLTSYDISPATCHFACTORY_H_
