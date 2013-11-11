// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// contentupdateproxy.h
// Proxy object for receiving SUS surface content update notifications and rerouting them to nativestream 
//
#ifndef COMPBACKUPDATEPROXY_H
#define COMPBACKUPDATEPROXY_H
#include <e32cmn.h>
#include <e32base.h>
#include <graphics/compositionsurfaceupdate.h>
#include <graphics/extensioncontainer.h>
class TSurfaceId;
class MRectangle;
class TRegion;
class COpenWfcStreamMap;
class CSurfaceStream;

#define KContentUpdateProxyVersion 3 

NONSHARABLE_CLASS(CContentUpdateProxy) : public CExtensionContainer, public MCompositionSurfaceUpdate
	{
public:
	static CExtensionContainer* NewL(TInt aScreenNum, COpenWfcStreamMap*	aSingleton,TInt aInternalVersion,TInt aPriority);
	

/*-------------------------------------------------------------------*//*!
 * \brief   Gets the API version number of the
 *          MCompositionSurfaceUpdate implementation.
 *
 * @return  1 for Oghma conformant implementation,
 *          2 for Sphinx.
 *          3 for OpenWF-C
 *//*-------------------------------------------------------------------*/

    virtual TInt        ApiVersion          (void) ;

/*-------------------------------------------------------------------*//*!
 * \brief   Gets the internal version number of the
 *          MCompositionSurfaceUpdate implementation.
 *
 * @return  The version number of the MCompositionSurfaceUpdate
 *          instance.
 *//*-------------------------------------------------------------------*/

    virtual TVersion    InternalVersion     (void) ;


/*-------------------------------------------------------------------*//*!
 * \brief   A function that responds to surface content notifications.
 *
 * When the contents of a surface change, this function gets called
 * by a mechanism that is an implementation detail of the graphics
 * driver together with the MCompositionSurfaceUpdate implementation. The
 * call can originate from different processes and the
 * MCompositionSurfaceUpdate together with the surface implementation
 * relays the information content of it, through implementation
 * specific means, to trigger the actual composition soon after this
 * call - this composition call should be synchronized with the
 * refresh rate of the screen.
 *
 * @param   aSurface            The surface that has been updated.
 * @param   aBuffer             The buffer of the surface to be used in
 *                              composition. Integer starting from 0.
 * @param   aRegion             The sub-area that has the updates. If NULL, the
 *                              whole surface is considered changed.
 * @param   aStatusConsumed     A request status object or NULL. If not NULL, then the
 *                              request status is completed once the backend
 *                              does not anymore need the contents of the
 *                              surface to render the update. This may happen
 *                              before actually displaying the finished frame.
 * @param   aStatusDisplayed    This is signaled after the composited frame
 *                              is posted the to display for the first time after
 *                              the update. After this the value in
 *                              aTimeStamp is valid, if the value in the
 *                              status object is KErrNone. Can be NULL, if
 *                              no signal is desired.
 * @param   aTimeStamp          Value of the User::FastCounter() right after the
 *                              display refresh that signaled aStatusDisplayed.
 * @param   aStatusDispXTimes   This is signaled when the surface has been on
 *                              the screen for aDisplayedXTimes refreshes,
 *                              including the update that signaled aStatusDisplayed.
 *                              Can be NULL, if no signal is wanted.
 * @param   aDisplayedXTimes    The number of refreshes after which aStatusDispXTimes
 *                              is signaled or NULL. If values is provided, it must be
 *                              >= 1.
 *
 * @pre     The MCompositionSurfaceUpdate implementation is waiting for content
 *          update notifications.
 * @pre     aSurface.IsNull() returns EFalse.
 * @pre     The surface aSurface is registered with the backend or all status
 *          requests are completed with KErrArgument.
 * @pre     Either both aStatusDisplayed and aTimeStamp are NULL or neither
 *          of them are.
 * @pre     Either both aStatusDispXTimes and aDisplayedXTimes are NULL or
 *          neither of them are.
 * @post    The MCompositionSurfaceUpdate is waiting for vsync or another
 *          synchronisation method to trigger composition,
 *          depending on cached visibility information.
 * @note    The updated region is used only as an optimisation hint.
 *          the current contents of the surface are always used for
 *          composition independent of the aRegion.
 * @see     Surface Update API Specification [R6] for further details
 *          about the semantics of the parameters and return values
 *          of the TRequestStatus objects.
 *//*-------------------------------------------------------------------*/

    virtual void        ContentUpdated      (const TSurfaceId&  aSurface,
                                             TInt               aBuffer,
                                             const TRegion*     aRegion,
                                             TRequestStatus*    aStatusConsumed,
                                             TRequestStatus*    aStatusDisplayed,
                                             TUint32*           aTimeStamp,
                                             TRequestStatus*    aStatusDispXTimes,
                                             TInt*              aDisplayedXTimes) ;

    
public:
    void RegisterNotifications(CSurfaceStream& aNs,
                               TInt            aBuffer,
                               TRequestStatus* aStatusDisplayed, TUint32* aTimeStamp,
                               TRequestStatus* aStatusDispXTimes, TInt* aDisplayedXTimes,
                               TRequestStatus* aStatusConsumed, const TRegion* aRegion);   
    
private:	//methods
	void ConstructL(TInt aScreenNum, COpenWfcStreamMap*	aSingleton,TInt aInternalVersio,TInt aPriority);
	CContentUpdateProxy();
	~CContentUpdateProxy();
	void CompleteStatus(TRequestStatus* aStatusConsumed,TRequestStatus* aStatusDisplayed,
	        TRequestStatus* aStatusDispXTimes,TInt aReason);
	virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);

private:	//data
	TInt				iScreenNum;
	TInt				iInternalVersion;
	TInt				iPriority;
	COpenWfcStreamMap*	iSingleton;
	RHeap&              iHeap;
	};


#endif /*COMPBACKUPDATEPROXY_*/
