#ifndef _CARDINAL_NOTIFICATIONS_H_
#define _CARDINAL_NOTIFICATIONS_H_

#include "types.h"

typedef enum NotificationType{
	NotificationType_Power,
	NotificationType_Performance
}NotificationType;

typedef (void)(*NotificationHandler)(NotificationType t, uint64_t n);

void
SubscribeNotification(NotificationHandler handler, 
					  NotificationType type, 
					  uint64_t specificEvent);

void
UnsubscribeNotfication(NotificationHandler handler, 
					   NotificationType type, 
					   uint64_t specificEvent);

#endif