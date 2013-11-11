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

#ifndef OPENWFCWRAPPER_H_
#define OPENWFCWRAPPER_H_

#include <babitflags.h>
#include <graphics/wsscene.h>
#include <babitflags.h>
#include <graphics/surface.h>
#include "displaypolicy.h"

//
//OpenWFC stuff

#include <e32std.h>
#include <e32base.h>
#include <e32hashtab.h>     //for RHashMap
#include <WF/wfc.h>
#include <EGL/egl.h>
#include <graphics/surfacemanager.h>
#include <graphics/wsdisplaycontrol.h>

//

class COpenWfcJobManger;

class TDisplayConfiguration;
class CElementWrapper;

class COpenWfcWrapper : public CBase, public MWsDisplayControl
	{
friend class CStdRenderStage;
friend class CElementWrapper;

public:
	static COpenWfcWrapper* NewL(TInt aScreenNo, CDisplayPolicy* aDisplayPolicy);
	TInt SetUiSurface(const TSurfaceId& aNewUiSurface);
	~COpenWfcWrapper();
	
	MWsElement* CreateElementL();
	void DestroyElement(MWsElement* aElement);
	void Compose(const TSurfaceId& aOffScreenTarget, const TPoint& aOrigin,
	             MWsElement* aStart, MWsElement* aEnd, TRequestStatus* aCompleted);
	TInt AddElement(MWsElement* aElement, MWsElement* aAbove);
	void RemoveElement(MWsElement* aElement);
	void PauseCompositorIfNeeded();
	void ResumeCompositorIfPaused();
	void SetScreenRotation(MWsScene::TSceneRotation aRotation);
	MWsScene::TSceneRotation ScreenRotation() const;
	TSize ScreenSize() const;
	void Begin(const TRegion* aRegion);
	void End();	
	TBool CompositionDue ();
	void Compose(TRequestStatus* aCompleted);
	TInt FlushedSetConfiguration(const TDisplayConfiguration& aConfig);
    TBool WithinBeginEnd() { return iWithinBeginEnd; };
	CDisplayPolicy* DisplayPolicy();
    void FlushSceneElementChanges ();
    void RemoveElementFromSceneList(CElementWrapper* aElement);
	
    /*-------------------------------------------------------------------*//*
     * CBase interface implementation functions
     *//*-------------------------------------------------------------------*/
    //Standard symbian interface extension mechanism
    virtual TInt        Extension_              (TUint aExtensionId, TAny*& a0, TAny* a1);

    enum    TInternalExtensionGuids
        {   //These debug object interfaces need not be supported at runtime or on all platforms
        EExtensionDebugBackendGuid  =   0x1028589A,
        EExtensionDebugContextGuid  =   0x1028589B,
        EExtensionDebugDeviceGuid   =   0x1028589C,
        EExtensionDebugElementGuid  =   0x1028589D,
        EExtensionDebugSourceGuid   =   0x1028589E, //F still spare when this was written...
        };
    
    /**    Get Extension Interface.
     *  Implemented using the CBase::Extension_() mechanism 
     *  @param  aExtensionId    The GUID/ Well-known ID of the interface
     *  @return pointer to the interface or NULL if not available
     **/
    inline TAny* GetInterface(TUint aExtensionId);

    /** Get Extension Interface - templated helper.
     *  Resolves the ID and returned pointer based on the class name.
     *  Class name should support ETypeId intergral value, else use non-template version. 
     *  @param  MClass  The class of the interface with embedded GUID / Well known ID 
     *  @return pointer to the interface or NULL if not available
     **/
    template <class MClass> MClass* GetInterface()
    {
        return static_cast<MClass*>(GetInterface(MClass::ETypeId));
    }
    
    TInt RegisterSurface(const TSurfaceId& aSurface);
    TInt UnregisterSurface(const TSurfaceId& aSurface);
    void Compose(const TSurfaceId& aOffScreenTarget, TRequestStatus* aCompleted);
    virtual TBitFlags32 SupportedScreenRotations() const;
    void PauseComposition();
    void ResumeComposition ();
	/*-------------------------------------------------------------------*//*
	* MWsDisplayControl/MDisplayControlBase interface implementation functions
	*//*-------------------------------------------------------------------*/

    virtual TInt NumberOfResolutions()const ;
    virtual TInt GetResolutions(RArray<TResolution>& aResolutions)const ;
    virtual void GetConfiguration(TDisplayConfiguration& aConfig)const ;
    virtual TInt SetConfiguration(const TDisplayConfiguration& aConfig) ;
    virtual TInt PreferredDisplayVersion()const ;

    virtual void NotifyOnDisplayChange(TRequestStatus& aStatus);
    virtual void NotifyOnDisplayChangeCancel();
    virtual void NotifyOnConfigChange(TRequestStatus& aStatus);
    virtual void NotifyOnConfigChangeCancel();

    //"secret" methods available to element
    WFCDevice Device(){return iDevice;}
    WFCContext OnScreenContext()   {return iOnScreenContext;}
    WFCSource IncEltRefRegSource(const TSurfaceId& aHashKey); //<@return registered source handle or NULL if not registered
    TInt DecEltRefRegSource(const TSurfaceId& aHashKey); //<@return 0=destroyed, +ve count left, -ve error

    static TInt TranslateOpenWfcError(WFCErrorCode error);
    static void DestroyAllContextElements(WFCDevice dev, WFCContext ctx);
    RSurfaceManager& SurfaceManager(){return iSurfaceManager;}
    EGLDisplay Display(){return iEGLDisplay;}
    
private:
    COpenWfcWrapper(CDisplayPolicy* aDisplayPolicy);
	void ConstructL(TInt aScreenId);
	TInt AttachSurfaceToUiElement(CElementWrapper* aNewUiElement);
	void SetCompositionModified();
	
    static TUint32 HashFunction(const TSurfaceId& aHashKey);
    void  ComposeInternal(const TSurfaceId&  aOffScreenTarget,
                                 const TPoint&      aOrigin,
                                 MWsElement*      aStart,
                                 MWsElement*      aEnd,
                                 TRequestStatus*    aCompleted);
    TBool IsValidSurface(const TSurfaceId& aSurface);

private:
	TBool iPaused;
	TBool iWithinBeginEnd;
	TBool iCompositionModified;
	TBool iAllowCompositorPause;
	
	CElementWrapper* iUiElement1;
	CElementWrapper* iUiElement2;
	
	TSurfaceId iUiSurface;
	CDisplayPolicy* iDisplayPolicy;
	
private:
    class OffScreenComposeGuard;
    
    struct SourceAndRef
        {
        WFCSource source;
        TInt sourceRef;
        TInt elementRef;
        };
    RHashMap<TSurfaceId, SourceAndRef> iSourceMap;
    
    WFCDevice iDevice;
    WFCContext iOnScreenContext;
    EGLDisplay iEGLDisplay;
    WFCint iScreenNumber;
    static const TInt iInitialSourceMapSize = 0x0;
    CElementWrapper* iSceneElementList;      //all elements visible in the scene hierarchy
    CElementWrapper* iRemoveElementList;     //all elements removed from the scene hierarchy
    CElementWrapper* iCleanupElementList;    //all elements currently allocated are destroyed on cleanup
    COpenWfcJobManger* iJobManager;
    MWsScene::TSceneRotation iRotation;
    TBool iAutonomousCompositionInitiated;
    MDisplayControlBase* iContextDisplayControl;
    RSurfaceManager iSurfaceManager;
	};

inline TAny* COpenWfcWrapper::GetInterface(TUint aExtensionId)
    {
    TAny*   retVal=NULL;
    //Note that extension is intentionally not overloaded in CCompositionBackend
    if (this->Extension_(aExtensionId,retVal,NULL)<KErrNone)
        {
        return NULL;
        }
    else
        {
        return retVal;
        }
    }

#endif /*OPENWFCWRAPPER_H_*/
