#include "notifications.h"

typedef struct NotificationProviderData{
	NotificationType type;
	uint64_t event;
	NotificationSubscriptionHandler subscriptionHandler;
}NotificationProviderData;

NotificationError
SubscribeNotification(NotificationHandler handler,
                      NotificationType type,
                      uint64_t specificEvent)
{
	handler = NULL;
	type = NotificationType_Power;
	specificEvent = 0;

	return NotificationError_Failed;
}

NotificationError
UnsubscribeNotfication(NotificationHandler handler,
                       NotificationType type,
                       uint64_t specificEvent)
{
	handler = NULL;
	type = NotificationType_Power;
	specificEvent = 0;

	return NotificationError_Failed;
}

UID
RegisterNotifyingSystem(const char *sys_name,
						NotificationType t,
						uint64_t n,
						NotificationSubscriptionHandler handler)
{
	sys_name = NULL;
	t = NotificationType_Power;
	n = 0;
	handler = NULL;

	return -1;
}

NotificationError
PostNotification(UID subID,
				 NotificationType t,
				 uint64_t n)
{
	subID = 0;
	t = NotificationType_Power;
	n = 0;

	return NotificationError_Failed;
}