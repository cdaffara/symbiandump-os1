#include "debughelper.h"

const TDesC& RawEventName(const TRawEvent& aEvent) 
	{
	switch(aEvent.Type())
		{	
		case TRawEvent::ENone:
			_LIT(ENone,"ENone");
			return ENone;
			
		case TRawEvent::EPointerMove:
			_LIT(EPointerMove,"EPointerMove");
			return EPointerMove;	
			
		case TRawEvent::EPointerSwitchOn:
			_LIT(EPointerSwitchOn,"EPointerSwitchOn");
			return EPointerSwitchOn;	
			
		case TRawEvent::EKeyDown:
			_LIT(EKeyDown,"EKeyDown");
			return EKeyDown;	
			
		case TRawEvent::EKeyUp:
			_LIT(EKeyUp,"EKeyUp");
			return EKeyUp;		
			
		case TRawEvent::ERedraw:
			_LIT(ERedraw,"ERedraw");
			return ERedraw;		
			
		case TRawEvent::ESwitchOn:
			_LIT(ESwitchOn,"ESwitchOn");
			return ESwitchOn;		
			
		case TRawEvent::EActive:
			_LIT(EActive,"EActive");
			return EActive;			
			
		case TRawEvent::EInactive:
			_LIT(EInactive,"EInactive");
		    return EInactive;		    
		    
		case TRawEvent::EUpdateModifiers:
			_LIT(EUpdateModifiers,"EUpdateModifiers");
			return EUpdateModifiers;		
			
		case TRawEvent::EButton1Down:
			_LIT(EButton1Down,"EButton1Down");
			return EButton1Down;		
			
		case TRawEvent::EButton1Up:
			_LIT(EButton1Up,"EButton1Up");
			return EButton1Up;			
			
		case TRawEvent::EButton2Down:
			_LIT(EButton2Down,"EButton2Down");
			return EButton2Down;		
			
		case TRawEvent::EButton2Up:
			_LIT(EButton2Up,"EButton2Up");
			return EButton2Up;			
			
		case TRawEvent::EButton3Down:
			_LIT(EButton3Down,"EButton3Down");
			return EButton3Down;	
			
		case TRawEvent::EButton3Up:
			_LIT(EButton3Up,"EButton3Up");
			return EButton3Up;		
			
		case TRawEvent::ESwitchOff:
			_LIT(ESwitchOff,"ESwitchOff");
			return ESwitchOff;		
			
		case TRawEvent::EKeyRepeat:
			_LIT(EKeyRepeat,"EKeyRepeat");
			return EKeyRepeat;	
			
		case TRawEvent::ECaseOpen:
			_LIT(ECaseOpen,"ECaseOpen");
			return ECaseOpen;			
			
		case TRawEvent::ECaseClose:
			_LIT(ECaseClose,"ECaseClose");
			return ECaseClose;		
			
		case TRawEvent::EPointer3DInRange:
			_LIT(EPointer3DInRange,"EPointer3DInRange");
			return EPointer3DInRange;	
			
		case TRawEvent::EPointer3DOutOfRange:
			_LIT(EPointer3DOutOfRange,"EPointer3DOutOfRange");
			return EPointer3DOutOfRange;	
			
		case TRawEvent::EPointer3DTilt:
			_LIT(EPointer3DTilt,"EPointer3DTilt");
			return EPointer3DTilt;	
			
		case TRawEvent::EPointer3DRotation:
			_LIT(EPointer3DRotation,"EPointer3DRotation");
			return EPointer3DRotation;	
			
		case TRawEvent::EPointer3DTiltAndMove:
			_LIT(EPointer3DTiltAndMove,"EPointer3DTiltAndMove");
			return EPointer3DTiltAndMove;	
			
		case TRawEvent::EButton4Down:
			_LIT(EButton4Down,"EButton4Down");
			return EButton4Down;	
			
		case TRawEvent::EButton4Up:
			_LIT(EButton4Up,"EButton4Up");
			return EButton4Up;		
			
		case TRawEvent::EButton5Down:
			_LIT(EButton5Down,"EButton5Down");
			return EButton5Down;	
			
		case TRawEvent::EButton5Up:
			_LIT(EButton5Up,"EButton5Up");
			return EButton5Up;	
			
		case TRawEvent::EButton6Down:
			_LIT(EButton6Down,"EButton6Down");
			return EButton6Down;	
			
		case TRawEvent::EButton6Up:
			_LIT(EButton6Up,"EButton6Up");
			return EButton6Up;		
			
		case TRawEvent::ERestartSystem:
			_LIT(ERestartSystem,"ERestartSystem");
			return ERestartSystem;
			
		default:
			_LIT(KUndef,"User Defined Event");
			return KUndef;
		}
	}

const TDesC& WsEventName(const TWsEvent& aEvent) 
	{
	switch(aEvent.Type())
		{
		case EEventNull:
			_LIT(EEventNull,"EEventNull");
			return EEventNull;
			
		case EEventKey:
			_LIT(EEventKey,"EEventKey");
			return EEventKey;
			
		case EEventKeyUp:
			_LIT(EEventKeyUp,"EEventKeyUp");
			return EEventKeyUp;
			
		case EEventKeyDown:
			_LIT(EEventKeyDown,"EEventKeyDown");
			return EEventKeyDown;
			
		case EEventModifiersChanged:
			_LIT(EEventModifiersChanged,"EEventModifiersChanged");
			return EEventModifiersChanged;
			
		case EEventPointer:
			_LIT(EEventPointer,"EEventPointer");
			return EEventPointer;
			
		case EEventPointerEnter:
			_LIT(EEventPointerEnter,"EEventPointerEnter");
			return EEventPointerEnter;
			
		case EEventPointerExit:
			_LIT(EEventPointerExit,"EEventPointerExit");
			return EEventPointerExit;
			
		case EEventPointerBufferReady:
			_LIT(EEventPointerBufferReady,"EEventPointerBufferReady");
			return EEventPointerBufferReady;
			
		case EEventDragDrop:
			_LIT(EEventDragDrop,"EEventDragDrop");
			return EEventDragDrop;
			
		case EEventFocusLost:
			_LIT(EEventFocusLost,"EEventFocusLost");
			return EEventFocusLost;
			
		case EEventFocusGained:
			_LIT(EEventFocusGained,"EEventFocusGained");
			return EEventFocusGained;
			
		case EEventSwitchOn:
			_LIT(EEventSwitchOn,"EEventSwitchOn");
			return EEventSwitchOn;
			
		case EEventPassword:
			_LIT(EEventPassword,"EEventPassword");
			return EEventPassword;
			
		case EEventWindowGroupsChanged:
			_LIT(EEventWindowGroupsChanged,"EEventWindowGroupsChanged");
			return EEventWindowGroupsChanged;
			
		case EEventErrorMessage:
			_LIT(EEventErrorMessage,"EEventErrorMessage");
			return EEventErrorMessage;
			
		case EEventMessageReady:
			_LIT(EEventMessageReady,"EEventMessageReady");
			return EEventMessageReady;
			
		case EEventMarkInvalid:
			_LIT(EEventMarkInvalid,"EEventMarkInvalid");
			return EEventMarkInvalid;
			
		case EEventSwitchOff:
			_LIT(EEventSwitchOff,"EEventSwitchOff");
			return EEventSwitchOff;
			
		case EEventKeySwitchOff:
			_LIT(EEventKeySwitchOff,"EEventKeySwitchOff");
			return EEventKeySwitchOff;
			
		case EEventScreenDeviceChanged:
			_LIT(EEventScreenDeviceChanged,"EEventScreenDeviceChanged");
			return EEventScreenDeviceChanged;
			
		case EEventFocusGroupChanged:
			_LIT(EEventFocusGroupChanged,"EEventFocusGroupChanged");
			return EEventFocusGroupChanged;
				
		case EEventCaseOpened:
			_LIT(EEventCaseOpened,"EEventCaseOpened");
			return EEventCaseOpened;
			
		case EEventCaseClosed:
			_LIT(EEventCaseClosed,"EEventCaseClosed");
			return EEventCaseClosed;
			
		case EEventWindowGroupListChanged:
			_LIT(EEventWindowGroupListChanged,"EEventWindowGroupListChanged");
			return EEventWindowGroupListChanged;
			
		case EEventWindowVisibilityChanged:
			_LIT(EEventWindowVisibilityChanged,"EEventWindowVisibilityChanged");
			return EEventWindowVisibilityChanged;
			
		case EEventRestartSystem:
			_LIT(EEventRestartSystem,"EEventRestartSystem");
			return EEventRestartSystem;
		case EEventDisplayChanged:
			_LIT(EEventDisplayChanged,"EEventDisplayChanged");
			return EEventDisplayChanged;
			
		case EEventKeyRepeat:
			_LIT(EEventKeyRepeat,"EEventKeyRepeat");
			return EEventKeyRepeat;
			
		case EEventGroupWindowOpen:
			_LIT(EEventGroupWindowOpen,"EEventGroupWindowOpen");
			return EEventGroupWindowOpen;
			
		case EEventGroupWindowClose:
			_LIT(EEventGroupWindowClose,"EEventGroupWindowClose");
			return EEventGroupWindowClose;
			
		case EEventWindowClose:
			_LIT(EEventWindowClose,"EEventWindowClose");
			return EEventWindowClose;
			
		case EEventDirectScreenAccessBegin:
			_LIT(EEventDirectScreenAccessBegin,"EEventDirectScreenAccessBegin");
			return EEventDirectScreenAccessBegin;
			
		case EEventDirectScreenAccessEnd:
			_LIT(EEventDirectScreenAccessEnd,"EEventDirectScreenAccessEnd");
			return EEventDirectScreenAccessEnd;
			
		case EEventHeartbeatTimerStateChange:
			_LIT(EEventHeartbeatTimerStateChange,"EEventHeartbeatTimerStateChange");
			return EEventHeartbeatTimerStateChange;
			
		case EEventPowerMgmt:
			_LIT(EEventPowerMgmt,"EEventPowerMgmt");
			return EEventPowerMgmt;
			
		case EEventReserved:
			_LIT(EEventReserved,"EEventReserved");
			return EEventReserved;
			
		case EEventUser:
			_LIT(EEventUser,"EEventUser");
			return EEventUser;

		default:
			_LIT(KUndef,"User Defined Event");
			return KUndef;						
		}
	}


