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
// A class which wraps OpenWF element related functionality.

//

#ifndef ELEMENTWRAPPER_H_
#define ELEMENTWRAPPER_H_

#include "openwfcwrapper.h"
#include <graphics/wselement.h>
#include <graphics/surface.h>
#include <WF/wfc.h>

#if defined(ENABLE_LI_LOGGING)
#define LILOG(X)  RDebug::Printf X
#else
#define LILOG(X)
#endif

#define OWF_SURFACE_WIDTH  (768) /* needs to be > 640 + 2 to allow S60 nHD config to work */
#define OWF_SURFACE_HEIGHT (768) /* needs to be > 640 + 2 to allow S60 nHD config to work */

NONSHARABLE_CLASS(CElementWrapper) : public CBase, public MWsElement
    {
public:  
    enum TOwfUpdates
        {   //All setters should set EUpdate_PostIllegalUpdate
        EUpdate_None                        = 0,

        EUpdate_DESTINATION_RECTANGLE       = 0x00000001,
        EUpdate_SOURCE                      = 0x00000002,
        EUpdate_SOURCE_RECTANGLE            = 0x00000004,
        EUpdate_SOURCE_FLIP                 = 0x00000008,
        EUpdate_SOURCE_ROTATION             = 0x00000010,
        EUpdate_SOURCE_SCALE_FILTER         = 0x00000020,  //No client-side code yet to drive this
        EUpdate_TRANSPARENCY_TYPES          = 0x00000040,
        EUpdate_GLOBAL_ALPHA                = 0x00000080,
        EUpdate_MASK                        = 0x00000100,  //No client-side code yet to drive this
        EUpdate_DESTINATION_CLIP_RECTANGLE  = EUpdate_DESTINATION_RECTANGLE|EUpdate_SOURCE_RECTANGLE,

        EUpdate_SOURCE_GEOMETRY_SET         = EUpdate_SOURCE_RECTANGLE|EUpdate_SOURCE_FLIP|EUpdate_SOURCE_ROTATION,
        EUpdate_ALL_ATTRIBS                 = 0x000fffff,
        EUpdate_AttributesIllegal           = 0x00100000,   //If set then the owf scene will not be updated because it would be illegal.
        EUpdate_SettingValid                = 0x00200000,

        EUpdate_Insert                      = 0x01000000,
        EUpdate_Remove                      = 0x02000000,
        EUpdate_Destroy                     = 0x04000000,
        EUpdate_SceneCommited               = 0x10000000,
        };    

public: // from MWsElement  
    const TSurfaceId& ConnectedSurface() const;
    TInt ConnectSurface(const TSurfaceId& aSurface);

    TInt SetSourceRectangle(const TRect& aSrc);
    TInt GetSourceRectangle(TRect& aSrc);
    
    TInt SetSourceFlipping(const TBool aFlip);
    TBool SourceFlipping() const;
    
    MWsElement* ElementAbove();
    MWsElement* ElementBelow();

    TInt SetGlobalAlpha(const TInt aAlpha);
    void GlobalAlpha(TInt& aAlpha) const;

    TInt SetSourceRotation(const TElementRotation aElementRotation);
    MWsElement::TElementRotation SourceRotation() const;
    
    TInt SetTargetRendererFlags(const TUint32& aTargetRendererFlags);
    TInt SetRenderStageFlags(const TUint32& aRenderStageFlags);
    void GetTargetRendererFlags(TUint32& aTargetRendererFlags) const;
    void GetRenderStageFlags(TUint32& aRenderStageFlags) const;

    TInt SetDestinationRectangle(const TRect& aDest);
    TInt GetDestinationRectangle(TRect& aDest) const;
    TInt SetDestinationClippingRect(const TRect& aDestClipRect);
    TInt GetDestinationClippingRect(TRect& aDestClipRect) const;
    
public:
    static CElementWrapper* NewL(COpenWfcWrapper& aCOpenWfcWrapper, CElementWrapper*& aCleanupPrevRef);
    CElementWrapper(COpenWfcWrapper& aOpenWfcWrapper);
    ~CElementWrapper();
    void ConstructL(CElementWrapper*& aCleanupPrevRef);
    void RemoveAfter(CElementWrapper** apBelow);
    void InsertAfter(CElementWrapper*& aRefBelow, CElementWrapper* aBelow, TOwfUpdates aUpdateFlag);
    CElementWrapper** RefsMyElementBelow();
    CElementWrapper*& MyElementAbove();
    COpenWfcWrapper& Owner()const { return iOpenWfcWrapper;}
    TBool FlushSceneChanges();
    WFCElement Element(){ return iElement;};
    TOwfUpdates UpdateFlags()const {return static_cast<TOwfUpdates>(iUpdateFlags);}
    TInt ReplicateElements(WFCContext aContext, WFCElement& aPreviousTargetElement, const TPoint& aOffset);

protected:
    void SetEltDestinationRectangle(WFCElement aElement)const;
    void SetEltDestinationRectangle(WFCElement aElement, const TPoint& aOffset)const;
    void SetEltSource(WFCElement aElement)const;
    void SetEltSourceComposed();
    void SetEltSourceRectangle(WFCElement aElement);
    void SetEltSourceFlip(WFCElement aElement)const;
    void SetEltTransparencyTypes(WFCElement aElement)const;
    void FlushAttributeChanges(WFCElement aElement, TInt aUpdateFlags);
    TBool IsNewElementLegal()const;
    
private: //methods
    TBool UnsetSurface(TSurfaceId& aSurface);
    static TInt WfcRotation(MWsElement::TElementRotation aRotation);
    void HelpStreamGetHeader(khronos_int32_t* width, khronos_int32_t* height )const;
    TRect SetSourceDestRect(const TRect& aDestElementRect, const TRect& aDestElementClippedRect,const TRect& aSrcOriginalElementRect,const TBool& aElementFlip,const MWsElement::TElementRotation& aElementRotation) const;
    TInt SurfaceInfo(const TSurfaceId& aSurface, RSurfaceManager::TInfoBuf& aInfo);


private: //data
    TUint                         iUpdateFlags;
    COpenWfcWrapper&              iOpenWfcWrapper;
    TSurfaceId                    iComposedSurfaceId;
    TSurfaceId                    iSurfaceId;
    TSize                         iSurfaceSize;
    TRect                         iSourcRect; 
    TRect                         iDestinationRect;
    TRect                         iDestinationClippingRect;
    TInt                          iGlobalAlpha;
    TBool                         iFlipped;
    MWsElement::TElementRotation  iRotation;
    TUint32                       iTargetRendererFlags;
    TUint32                       iRenderStageFlags;
    WFCElement                    iElement;
    CElementWrapper*              iElementAbove;     //used to maintain the scene list
    CElementWrapper*              iElementBelow;     //used to maintain the scene list
    CElementWrapper**             iCleanupPrevRef;    //used to clean up the context on exit
    CElementWrapper*              iCleanupNext;       //used to clean up the context on exit
    };

#endif /* ELEMENTWRAPPER_H_ */
