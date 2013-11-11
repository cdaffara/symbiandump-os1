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
// CompositionSurfaceUpdate.h
// Interface to be implemented and registered with SUS by listener for surface content update messages.  
//
#ifndef __COMPOSITIONSURFACEUPDATE_H__
#define __COMPOSITIONSURFACEUPDATE_H__

/**
 * \brief Interface to implement and register with SUS to receive ContentUpdated() notifications. 
 * 
 * 
 **/
NONSHARABLE_CLASS(MCompositionSurfaceUpdate)
    {
public:
    enum    
        {
        KUidCompositionSurfaceUpdate =  0x10286498,
        ETypeId=     KUidCompositionSurfaceUpdate 
        };
    /*-------------------------------------------------------------------*//*!
     * \brief   Gets the API version number of the
     *          implementation.
     *
     * @return  1 for Oghma conformant implementation,
     *          2 for Sphinx (Maybe).
     *          3 for Native Stream first version.
     *//*-------------------------------------------------------------------*/

        virtual TInt        ApiVersion          (void) = 0;

    /*-------------------------------------------------------------------*//*!
     * \brief   Gets the internal version number of the
     *          implementation.
     *
     * @return  The version number of the 
     *          instance.
     *//*-------------------------------------------------------------------*/

        virtual TVersion    InternalVersion     (void) = 0;

        /*-------------------------------------------------------------------*//*!
         * \brief   A function that responds to surface content notifications.
         *
         * When the contents of a surface change, this function gets called
         * by a mechanism that is an implementation detail of the graphics
         * driver together with the Native Stream and Compositor implementations. 
         * The call can originate from different processes and the
         * Native Stream together with the Compositor implementation
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
         *                              request status is completed once the receiver
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
         * @pre     The surface aSurface is registered with the receiver or all status
         *          requests are completed with KErrArgument.
         * @pre     Either both aStatusDisplayed and aTimeStamp are NULL or neither
         *          of them are.
         * @pre     Either both aStatusDispXTimes and aDisplayedXTimes are NULL or
         *          neither of them are.
         * @post    The MCompositionSurfaceUpdate implementation is waiting for vsync 
         *          or another synchronisation method to trigger composition,
         *          depending on cached visibility information.
         * @note    The updated region is used only as an optimisation hint.
         *          the current contents of the surface are always used for
         *          composition independent of the aRegion.
         * @see     Surface Update API Specification for further details
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
                                                     TInt*              aDisplayedXTimes) = 0;
public:
    /** Get Extension Interface.
     *  Implemented using the CBase::Extension_() mechanism 
     *  Note that the pointer returned is only good for the lifetime of the called CBase derived object.
     *  @param  aExtensionId    The GUID/ Well-known ID of the interface
     *  @return pointer to the interface or NULL if not available
     **/
        inline TAny* GetInterface(TUint aExtensionId);
    /** Get Extension Interface - templated helper.
     *  Resolves the ID and returned pointer based on the class name.
     *  Note that the pointer returned is only good for the lifetime of the called CBase derived object.
     *  Class name should support ETypeId intergral value, else use non-template version. 
     *  @param  MClass  The class of the interface with embedded GUID / Well known ID 
     *  @return pointer to the interface or NULL if not available
     **/
        template <class MClass> MClass* GetInterface()
        {
            return static_cast<MClass*>(GetInterface(MClass::ETypeId));
        }

protected:  //From CBase
    virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)=0;
    
    };

inline TAny* MCompositionSurfaceUpdate::GetInterface(TUint aExtensionId)
    {
    TAny*   retVal=NULL;
    //Note that extension is intentionally not overloaded in MCompositionSurfaceUpdate
    if (this->Extension_(aExtensionId,retVal,NULL)<KErrNone)
        {
        return NULL;
        }
    else
        {
        return retVal;
        }
    }

#endif  //__COMPOSITIONSURFACEUPDATE_H__
