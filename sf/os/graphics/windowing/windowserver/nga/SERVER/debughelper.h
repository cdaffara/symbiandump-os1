#ifndef __DEBUGHELPER_H__
#define __DEBUGHELPER_H__

#include <e32event.h>
#include <e32base.h>
#include <w32std.h>

const TDesC& RawEventName(const TRawEvent& aEvent); 
const TDesC& WsEventName(const TWsEvent& aEvent);

#endif
