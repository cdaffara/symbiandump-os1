/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Naviscroll data type definitions
*
*/



#ifndef SENSRVNAVISCROLLDATATYPES_H
#define SENSRVNAVISCROLLDATATYPES_H

//  INCLUDES
#include <e32base.h>
#include <sensrvtypes.h> 

//NAVISCROLL RELATED DATATYPES

/**
* Naviscroll raw-data item
*/
class TSensrvNaviData
    {
public:
    /**
    * Channel data type Id number
    */      
    static const TSensrvChannelDataTypeId KDataTypeId = 0x10205087;

    /**
    * Channel data type index numbers
    */      
    const class Index
        {
    public:  
        enum TSensrvNaviDataIndexes
          {
          iTimeStamp = 0,
          iPosition,
          iTouch
          };    
        };
 
public:
    /**
    * - Item name:   Sampling time from sensor.
    * - Item Index:  0
    * - Description: Timestamp for a sample.
    */   
    TTime iTimeStamp;    
   
    /**
    * - Item name:   Position for naviscroll.
    * - Item Index:  1 
    * - Description: Position of the naviscroll. Range 0 -> 0xFFFF
    */   
    TInt iPosition;  
  
    /**
    * - Item name:   Naviscroll touch.
    * - Item Index:  2 
    * - Description: Info about is touch indicated in naviscoll.
    */   
    TBool iTouch;  
    };


class TSensrvNaviEvent
    {
public:
    /**
    * Channel data type Id number
    */      
    static const TSensrvChannelDataTypeId KDataTypeId = 0x10205080;

    /**
    * Channel data type index numbers
    */      
    const class Index
        {
    public:  
        enum TSensrvNaviEventIndexes
          {
          iTimeStamp = 0,
          iEventType,
          iEventState,
          iTouch,
          iSpeed
          };    
        };  

public:
    /**
    * - Item name:   Event generation time.
    * - Item Index:  0
    * - Description: Timestamp for a event generation time.
    */     
    TTime iTimeStamp;    

    /**
    * The TEventType indicates of direction of the event.
    *
    * Possible values:
    * - ENext, 
    * - EPrevious, 
    *
    */

    enum TEventType
        {
        ENext =  0,
        EPrevious
        };  
  
    /**
    * - Item name:   Type of the navi event
    * - Item Index:  1
    * - Description: Indicates type of the navi event
    */    
    TEventType iEventType;    
 
    /**
    * The TEventType indicates of state of the event.
    *
    * Possible values:
    * - EStart,  
    * - ERepeat, 
    * - EStop,
    *
    */
    enum TEventState
        {
        EStart =  0,
        ERepeat,
        EStop    
        };  
  
    /**
    * - Item name:   State of the navi event
    * - Item Index:  2
    * - Description: Indicates state of the navi event
    */    
    TEventState iEventState;    

  
    /**
    * - Item name:   Naviscroll touch.
    * - Item Index:  3 
    * - Description: Info about is touch indicated in naviscoll.
    */   
    TBool iTouch; 
  
    /**
    * - Item name:   Naviscroll speed.
    * - Item Index:  4
    * - Description: Position units in second.
    */   
    TInt iSpeed; 
    };

#endif //SENSRVNAVISCROLLDATATYPES_H

// End of File


