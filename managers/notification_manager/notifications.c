#include "notifications.h"

typedef struct NotificationProviderData{
	NotificationType type;
	uint64_t event;
	NotificationProvider subscriptionHandler;
}NotificationProviderData;

NotificationError
SubscribeNotification(NotificationHandler handler,
                      NotificationType type,
                      uint64_t specificEvent)
{

}

NotificationError
UnsubscribeNotfication(NotificationHandler handler,
                       NotificationType type,
                       uint64_t specificEvent)
{

}

UID
RegisterNotifyingSystem(const char *sys_name,
						NotificationType t,
						uint64_t n,
						NotificationSubscriptionHandler handler)
{

}

NotificationError
PostNotification(UID subID,
				 NotificationType t,
				 uint64_t n)
{

}