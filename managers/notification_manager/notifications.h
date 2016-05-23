#ifndef _CARDINAL_NOTIFICATIONS_H_
#define _CARDINAL_NOTIFICATIONS_H_

#include "types.h"

typedef enum NotificationType {
    NotificationType_Power,
    NotificationType_Performance
} NotificationType;

typedef enum NotificationError{
	NotificationError_Success,
	NotificationError_Failed,
	NotificationError_InvalidUID
}

typedef (void)(*NotificationHandler)(NotificationType t, uint64_t n);
typedef (NotificationError)(*NotificationSubscriptionHandler)(NotificationHandler h, NotificationType t, uint64_t n);


NotificationError
SubscribeNotification(NotificationHandler handler,
                      NotificationType type,
                      uint64_t specificEvent);

NotificationError
UnsubscribeNotfication(NotificationHandler handler,
                       NotificationType type,
                       uint64_t specificEvent);

UID
RegisterNotifyingSystem(const char *sys_name,
						NotificationType t,
						uint64_t n,
						NotificationSubscriptionHandler handler);

NotificationError
PostNotification(UID subID,
				 NotificationType t,
				 uint64_t n);

#endif