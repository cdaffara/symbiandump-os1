// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "epos_cpospsylisthandler.h"
#include "epos_cpossingletonmanager.h"
#include "epos_cpospsyfixstatemanager.h"
#include "epos_defaultproxyprivatecrkeys.h"
#include "epos_cposconstmanager.h"
#include "epos_defaultproxycommon.h"

#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <centralrepository.h>


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPsyListHandler::CPosPsyListHandler()
    {
    }

// EPOC default constructor can leave.
void CPosPsyListHandler::ConstructL()
    {
    TRACESTRING( "CPosPsyListHandler::ConstructL start... " )

    iFixStateManager = CPosPsyFixStateManager::GetInstanceL();
    iFixStateManager->AddListenerL( this );
    
    iConstManager = CPosConstManager::GetInstanceL();

    TRACESTRING( "CPosPsyListHandler::ConstructL end " )
    }

// ---------------------------------------------------------
// CPosPsyListHandler::GetInstanceL
// ---------------------------------------------------------
//
CPosPsyListHandler* CPosPsyListHandler::GetInstanceL()
    {
    CPosPsyListHandler* self = reinterpret_cast < CPosPsyListHandler* > 
        ( CPosSingletonManager::GetObject( 
            EPosSigletonObjectIdPsyListHandlerId ) );
        
    if ( !self )
        {
        //Construct a new object and store it to CPosSingletonManager
        self = new ( ELeave ) CPosPsyListHandler;
        CleanupStack::PushL(self);
        self->ConstructL();
        CPosSingletonManager::SetObjectL(
            self,
            EPosSigletonObjectIdPsyListHandlerId );
        CleanupStack::Pop(self);
        }
        
    self->iRefCount++;
    
    return self;
    }

// ---------------------------------------------------------
// CPosPsyListHandler::ReleaseInstance
// ---------------------------------------------------------
//
void CPosPsyListHandler::ReleaseInstance()
    {
    iRefCount--;
    if ( iRefCount == 0 )
        {
        //We shall delete the instance
        CPosSingletonManager::ReleaseObject(
            EPosSigletonObjectIdPsyListHandlerId );
        }
    }

// Destructor
CPosPsyListHandler::~CPosPsyListHandler()
    {
    TRACESTRING( "CPosPsyListHandler::destructor start... " )

    if ( iConstManager )
        {
        iConstManager->ReleaseInstance();
        }
        
    if ( iFixStateManager )
        {
        iFixStateManager->RemoveListener( this );
        iFixStateManager->ReleaseInstance();
        }
    
    if ( iModulesSettings )
        {
        iModulesSettings->RemoveListener( *this );
        }
    
    iModuleInfoArray.Close();

    iListenerArray.Close();
    TRACESTRING( "CPosPsyListHandler::destructor end" )
    }

// ---------------------------------------------------------
// CPosPsyListHandler::SetModuleSettingsManagerL
// ---------------------------------------------------------
//
void CPosPsyListHandler::SetModuleSettingsManagerL(
            MPosModuleSettingsManager& aManager )
    {
    if ( iModulesSettings == NULL )
        {
        iModulesSettings = &aManager;
        iModulesSettings->AddListenerL( *this );

        UpdateModuleListL();

        //Move last working GPS PSY to the front of the list
        MovePsyToTop( iConstManager->GetLastWorkingGpsPsyId() );
        }
    }

// ---------------------------------------------------------
// CPosPsyListHandler::AddListenerL
// ---------------------------------------------------------
//
void CPosPsyListHandler::AddListenerL(
            MPosPsyListListener* aListener )
    {
    User::LeaveIfError( iListenerArray.Append( aListener ) );
    }
            
// ---------------------------------------------------------
// CPosPsyListHandler::RemoveListener
// ---------------------------------------------------------
//
void CPosPsyListHandler::RemoveListener(
            MPosPsyListListener* aListener )
    {
    TInt count = iListenerArray.Count();
    for ( TInt i = count -1; i >= 0; i-- )
        {
        if ( iListenerArray[i] == aListener )
            {
            iListenerArray.Remove( i );
            }
        }
    }

// ---------------------------------------------------------
// CPosPsyListHandler::GetPsyListL
// ---------------------------------------------------------
//
void CPosPsyListHandler::GetPsyListL(
            RArray< TPositionModuleId >& aPsyList )
    {
    TRACESTRING( "CPosPsyListHandler::GetPsyListL" )
    aPsyList.Reset();

    TBool integratedGpsAdded = EFalse;
    
    TInt count = iModuleInfoArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        const TPositionModuleInfo& moduleInfo = iModuleInfoArray[i];
        if( moduleInfo.IsAvailable() )
            {
            if ( IsGpsTechnology( moduleInfo ) && 
                moduleInfo.DeviceLocation() == TPositionModuleInfo::EDeviceInternal )
                {
                if ( !integratedGpsAdded )
                    {
                    TRACESTRING2( "PSY: %x", moduleInfo.ModuleId() )
                    aPsyList.AppendL( moduleInfo.ModuleId() );
                    integratedGpsAdded = ETrue;
                    }
                }
            else
                {
                TRACESTRING2( "PSY: %x", moduleInfo.ModuleId() )
                aPsyList.AppendL( moduleInfo.ModuleId() );
                }
            }
        }
    }

// ---------------------------------------------------------
// CPosPsyListHandler::GetExtGpsPsyListL
// ---------------------------------------------------------
//
void CPosPsyListHandler::GetExtGpsPsyListL(
        RArray< TPositionModuleId >& aPsyList )
    {
    TRACESTRING( "CPosPsyListHandler::GetExtGpsPsyListL" )

    aPsyList.Reset();
    
    TInt count = iModuleInfoArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        const TPositionModuleInfo& moduleInfo = iModuleInfoArray[i];
        if( moduleInfo.IsAvailable() && 
            IsGpsTechnology( moduleInfo ) && 
            moduleInfo.DeviceLocation() == TPositionModuleInfo::EDeviceExternal )
            {
            TRACESTRING2( "PSY: %x", moduleInfo.ModuleId() )
            aPsyList.AppendL( moduleInfo.ModuleId() );
            }
        }
    }

// ---------------------------------------------------------
// CPosPsyListHandler::IsClassSupported
// ---------------------------------------------------------
//
TBool CPosPsyListHandler::IsClassSupported(
            TUint32 aClassType,
            TPositionModuleId aModuleId ) const
    {
    TInt index = ModuleIndex( aModuleId );
    if ( index != KErrNotFound )
        {
        const TPositionModuleInfo& moduleInfo = iModuleInfoArray[index];
        
        TUint32 classesSupported =
                moduleInfo.ClassesSupported(EPositionInfoFamily);

        return ( aClassType == ( aClassType & classesSupported ) ) &&
                        ( aClassType & EPositionInfoClass );
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CPosPsyListHandler::PsyFixStateChanged
// ---------------------------------------------------------
//
void CPosPsyListHandler::PsyFixStateChanged( 
            TPositionModuleId aModuleId,
            CPosPsyFixStateManager::TPsyFixState aFixState )
    {
    TRACESTRING( "CPosPsyListHandler::PsyFixStateChanged start..." )
    TRACESTRING2( "PSY: %x", aModuleId )
    TRACESTRING2( "Fix state: %d", aFixState )

    TInt index = ModuleIndex( aModuleId );
    
    if ( index != KErrNotFound )
	    {
    	TPositionModuleInfo moduleInfo = iModuleInfoArray[index];
	    
	    if ( IsGpsTechnology( moduleInfo ) && 
	        aFixState == CPosPsyFixStateManager::EPsyFixStateYes )
	        {
	        //Move this one to the top of the list if it's not 
	        //already on top
	        TBool isListModified = MovePsyToTop( aModuleId );
	        
	        //If the list is modified, notify listeners
	        if ( isListModified )
	            {
	            //Write last working GPS PSY UID to CenRep
	            iConstManager->SetLastWorkingGpsPsyId( aModuleId );
	            
	            TPosPsyListChangeEvent event;
	            event.iType = EPosPsyListChangeEventPriorityChanged;
	            NotifyListeners( event );
	            }
	        }
    	}
    }

// ---------------------------------------------------------
// CPosPsyListHandler::HandleModuleSettingsChangedL
// ---------------------------------------------------------
//
void CPosPsyListHandler::HandleModuleSettingsChangedL(
            TPosModulesEvent aEvent )
    {
    TRACESTRING( "CPosPsyListHandler::HandleModuleSettingsChagnedL start..." )
    TRACESTRING2( "Event type: %x", aEvent.iType )
    TRACESTRING2( "PSY: %x", aEvent.iModuleId )

    TInt index = ModuleIndex(aEvent.iModuleId);
    TPosPsyListChangeEvent event;

    switch (aEvent.iType)
        {
        case EPosModulesEventModuleRemoved:
            {
            //Remove module from the array
            if ( index != KErrNotFound )
                {
                iModuleInfoArray.Remove( index );
                //notify listener module removed
                event.iType = EPosPsyListChangeEventPsyDeleted;
                event.iPsyId = aEvent.iModuleId;
                NotifyListeners( event );
                }
            }
            break;
        case EPosModulesEventAvailabilityChanged:
            {
            //Get new module info
            TPositionModuleInfo moduleInfo;
            iModulesSettings->GetModuleInfoL( 
                aEvent.iModuleId, 
                moduleInfo );
                
            TBool isModuleAvailable = moduleInfo.IsAvailable();
            
            //Update the list
            UpdateModuleListL();
            
            if ( !isModuleAvailable )
                {
                //Module is disabled. 
                //notify listener module removed
                event.iType = EPosPsyListChangeEventPsyDeleted;
                event.iPsyId = aEvent.iModuleId;
                NotifyListeners( event );
                }
            else
                {
                //If module is enabled. Current location request
                //will not be affected. We only notify listeners
                //to rebuild the PSY list
                event.iType = EPosPsyListChangeEventListRebuild;
                event.iPsyId = aEvent.iModuleId;
                NotifyListeners( event );
                }
            }
            break;
        case EPosModulesEventVisibilityChanged:
            {
            TBool isModuleVisible = iModulesSettings->IsModuleVisibleL(
                aEvent.iModuleId );

            //Update the list
            UpdateModuleListL();
            
            if ( !isModuleVisible )
                {
                //Module is becoming invisible. 
                //notify listener module removed
                event.iType = EPosPsyListChangeEventPsyDeleted;
                event.iPsyId = aEvent.iModuleId;
                NotifyListeners( event );
                }
            else
                {
                //If module is becoming visible. Current location request
                //will not be affected. We only notify listeners
                //to rebuild the PSY list
                event.iType = EPosPsyListChangeEventListRebuild;
                event.iPsyId = aEvent.iModuleId;
                NotifyListeners( event );
                }
            }
            break;
        case EPosModulesEventPrioritiesChanged:
        case EPosModulesEventModuleInstalled:
        case EPosModulesEventUnspecified:
            {
            UpdateModuleListL();
            event.iType = EPosPsyListChangeEventListRebuild;
            NotifyListeners( event );
            }
            break;
        default:
            //Other change has no effect on default proxy
            break;
        }
    }

// ---------------------------------------------------------
// CPosPsyListHandler::UpdateModuleListL
// ---------------------------------------------------------
//
void CPosPsyListHandler::UpdateModuleListL()
    {
    TRACESTRING( "CPosPsyListHandler::UpdateModuleListL start..." )


    iModuleInfoArray.Reset();
    
    TInt index;
    
    // Read Module Info from settings and fill array
    CPosModuleIdList* idList = iModulesSettings->ModuleIdListL();
    CleanupStack::PushL( idList );

    TInt count = idList->Count();
    
    for ( index = 0; index < count; index++ )
        {
        TPositionModuleInfo moduleInfo;

        iModulesSettings->GetModuleInfoL( ( *idList )[index], moduleInfo );

        User::LeaveIfError( iModuleInfoArray.Append( moduleInfo ) );
        TRACESTRING2( "PSY: %x", moduleInfo.ModuleId() )
        }

    CleanupStack::PopAndDestroy(idList);

    /*
    iModuleInfoArray.Reset();
    
    TInt index;
    
    // Read Module Info from settings and fill array
    CPosModuleIdList* idList = iModulesSettings->ModuleIdListL();
    CleanupStack::PushL( idList );

    TInt count = idList->Count();
    
    TBool integratedGpsAdded = EFalse;
    
    for ( index = 0; index < count; index++ )
        {
        TPositionModuleInfo moduleInfo;

        iModulesSettings->GetModuleInfoL( ( *idList )[index], moduleInfo );

        //only one internal GPS PSY sppears in the list
        if ( IsGpsTechnology( moduleInfo ) && 
            moduleInfo.DeviceLocation() == TPositionModuleInfo::EDeviceInternal )
            {
            if ( !integratedGpsAdded )
                {
                User::LeaveIfError( iModuleInfoArray.Append( moduleInfo ) );
                integratedGpsAdded = ETrue;
                TRACESTRING2( "PSY: %x", moduleInfo.ModuleId() )
                }
            }
        else
            {
            User::LeaveIfError( iModuleInfoArray.Append( moduleInfo ) );
            TRACESTRING2( "PSY: %x", moduleInfo.ModuleId() )
            }
        }

    CleanupStack::PopAndDestroy(idList);
    */
    }

// ---------------------------------------------------------
// CPosPsyListHandler::ModuleIndex
// ---------------------------------------------------------
//
TInt CPosPsyListHandler::ModuleIndex( TPositionModuleId aModuleId ) const
    {
    TInt count = iModuleInfoArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        const TPositionModuleInfo& moduleInfo = iModuleInfoArray[i];
        if( moduleInfo.ModuleId() == aModuleId )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CPosPsyListHandler::NotifyListeners
// ---------------------------------------------------------
//
void CPosPsyListHandler::NotifyListeners( 
            const TPosPsyListChangeEvent& aEvent )
    {
    TInt count = iListenerArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iListenerArray[i]->PsyListChanged( aEvent );
        }
    }
    
// ---------------------------------------------------------
// CPosPsyListHandler::GetTtffAndTtnf
// ---------------------------------------------------------
//
TInt CPosPsyListHandler::GetTtffAndTtnf(
            TPositionModuleId aModuleId,
            TTimeIntervalMicroSeconds& aTtff,
            TTimeIntervalMicroSeconds& aTtnf ) const
    {
    TInt index = ModuleIndex( aModuleId );
    if ( index != KErrNotFound )
        {
        const TPositionModuleInfo& moduleInfo = iModuleInfoArray[index];
        TPositionQuality quality;
        moduleInfo.GetPositionQuality( quality );
        
        aTtff = quality.TimeToFirstFix();
        aTtnf = quality.TimeToNextFix();

        TRACESTRING2( "Module Id: %x", aModuleId )
        TRACESTRING2( "TTFF: %d", aTtff.Int64() )
        TRACESTRING2( "TTNF: %d", aTtnf.Int64() )

        return KErrNone;
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CPosPsyListHandler::IsGpsTechnology
// ---------------------------------------------------------
//
TBool CPosPsyListHandler::IsGpsTechnology( 
            const TPositionModuleInfo& aModuleInfo ) const
    {
    //When the module support satellite info, it is a type GPS technolgy
    TUint32 classesSupported =
        aModuleInfo.ClassesSupported(EPositionInfoFamily);

    return ( classesSupported & EPositionSatelliteInfoClass );
    }

// ---------------------------------------------------------
// CPosPsyListHandler::IsFirstGpsPsyExternal
// ---------------------------------------------------------
//
TBool CPosPsyListHandler::IsFirstGpsPsyExternal() const
    {
    TInt count = iModuleInfoArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        const TPositionModuleInfo& moduleInfo = iModuleInfoArray[i];
        if( moduleInfo.IsAvailable() && 
            IsGpsTechnology( moduleInfo ) )
            {
            if ( moduleInfo.DeviceLocation() == TPositionModuleInfo::EDeviceExternal )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CPosPsyListHandler::IsModuleNetworkBased
// ---------------------------------------------------------
//
TBool CPosPsyListHandler::IsModuleNetworkBased(
    TPositionModuleId aModuleId ) const
    {
    TInt index = ModuleIndex( aModuleId );
    
    if ( index != KErrNotFound )
        {
        const TPositionModuleInfo& moduleInfo = iModuleInfoArray[index];
        return ( moduleInfo.TechnologyType() & 
            TPositionModuleInfo::ETechnologyNetwork );
        }
        
    return EFalse;
    }


// ---------------------------------------------------------
// CPosPsyListHandler::MovePsyToTop
// ---------------------------------------------------------
//
TBool CPosPsyListHandler::MovePsyToTop( TPositionModuleId aId )
    {
    if ( aId == KPositionNullModuleId )
        {
        return EFalse;
        }
        
    TInt index = ModuleIndex( aId );
    if ( index == KErrNotFound )
        {
        return EFalse;
        }
        
    TRACESTRING2( "MovePsyToTop: Id: %x", aId)
    
    TPositionModuleInfo moduleInfo = iModuleInfoArray[index];
        
    TBool isFirstSwitch = ETrue;
    
    TInt count = iModuleInfoArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TPositionModuleInfo info = iModuleInfoArray[i];
        if ( IsGpsTechnology( info ) )
            {
            if ( isFirstSwitch )
                {
                if( info.ModuleId() == aId || 
                    ( moduleInfo.DeviceLocation() == 
                        TPositionModuleInfo::EDeviceInternal &&
                      info.DeviceLocation() == 
                        TPositionModuleInfo::EDeviceExternal &&
                     iFixStateManager->GetPsyFixState(
                         info.ModuleId() ) == 
                         CPosPsyFixStateManager::EPsyFixStateYes ) )
                    {
                    //We dont move in this case.
                    return EFalse;
                    }
                      
                }
            //switch
            iModuleInfoArray[i] = moduleInfo;
            moduleInfo = info;
            isFirstSwitch = EFalse;
            if( moduleInfo.ModuleId() == aId )
                {
                return ETrue;
                }
            }
        }
    return ETrue;
    }

//  End of File
