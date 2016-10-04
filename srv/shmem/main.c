#include <cardinal/ipc.h>
#include <cserver/cserver.h>


int main(){
	cserver_main(CARDINAL_IPCDEST_FILESERVER, NULL, NULL, NULL);
	return 0;
}