#ifndef _LIB_CARDINAL_SYSCALL_EMU_H_
#define _LIB_CARDINAL_SYSCALL_EMU_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "cardinal_types.h"
#include "syscall_property.h"
#include "syscall_list.h"
#include "syscall.h"
#include "file_server.h"
#include "ipc.h"

#define MAX_FILE_DESCRIPTORS 1024

typedef enum {
	Cardinal_EmulatedSyscalls_Read = 0,
	Cardinal_EmulatedSyscalls_Write = 1,
	Cardinal_EmulatedSyscalls_Open = 2,
	Cardinal_EmulatedSyscalls_Close = 3,
	Cardinal_EmulatedSyscalls_MMap = 9,
	Cardinal_EmulatedSyscalls_Brk = 12,
	Cardinal_EmulatedSyscalls_Ioctl = 16, 	//TODO: Implement a temporary wrapper for this, we don't want to actually translate ioctls
	Cardinal_EmulatedSyscalls_Readv = 19,
	Cardinal_EmulatedSyscalls_Writev = 20,
	Cardinal_EmulatedSyscalls_Nanosleep = 35,
	Cardinal_EmulatedSyscalls_GetPID = 39,
	Cardinal_EmulatedSyscalls_Fork = 57,
	Cardinal_EmulatedSyscalls_Exit = 60,
	Cardinal_EmulatedSyscalls_Fcntl = 72,
	Cardinal_EmulatedSyscalls_ArchPrctl = 158,
	Cardinal_EmulatedSyscalls_GetTID = 186,
	Cardinal_EmulatedSyscalls_Time = 201,
	Cardinal_EmulatedSyscalls_SetTidAddress = 218,
} Cardinal_EmulatedSyscalls;


typedef enum {
    DescriptorFlags_None = 0,
    DescriptorFlags_Free = (1 << 0),
    DescriptorFlags_CloseOnExec = (1 << 1)
} DescriptorFlags;

struct CardinalFileDescriptor{
	uint64_t AdditionalData;
	uint64_t TargetPID;
	DescriptorFlags Flags;
};

static _Atomic volatile uint8_t __card_allocating_fd = 0;
static struct CardinalFileDescriptor __card_fds[MAX_FILE_DESCRIPTORS];
static UID __card_currentPID = 0;
static _Atomic uint64_t __card_req_id = 1;

static __inline bool
__card_ValidateFD(int fd, bool is_open) {
	if(__card_currentPID == 0)
		__card_currentPID = Syscall2(Syscall_GetProperty, CardinalProperty_PID, 0);

	if(fd >= MAX_FILE_DESCRIPTORS)
		return 0;

	if(__card_fds[fd].Flags == DescriptorFlags_Free)
		return !is_open;

	return 1;
}

static __inline int
__card_AllocateFD(struct OpenResponse *resp, int flags) {

	while(__card_allocating_fd);

	int fd = 0;

	__card_allocating_fd = 1;

	for(int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
		
		if(__card_fds[i].Flags == DescriptorFlags_Free) {
			__card_fds[i].TargetPID = resp->targetPID;
			__card_fds[i].AdditionalData = resp->fd;
			__card_fds[i].Flags = DescriptorFlags_None;

			if(flags & O_CLOEXEC)
				__card_fds[i].Flags |= DescriptorFlags_CloseOnExec;

			fd = i;
			break;
		}
	}

	__card_allocating_fd = 0;
	return fd;
}

static __inline uint64_t
__card_open(const char *path, int flags, int mode) {

	uint64_t total_size = sizeof(struct OpenRequest);
	total_size += strnlen(path, MAX_PATH_LEN);

	struct OpenRequest *m = malloc(total_size);
	if(m == NULL)
		return -ENOMEM;

	m->m.DestinationPID = CARDINAL_IPCDEST_FILESERVER;
	m->m.MsgID = __card_req_id++;
	m->m.Size = total_size;

	m->flags = flags;
	m->mode = mode;
	m->msg_type = CARDINAL_MSG_TYPE_OPENREQUEST;

	strncpy(m->path, path, strnlen(path, MAX_PATH_LEN));

	uint64_t msgID_backup = m->m.MsgID;

	PostIPCMessages((Message**)&m, 1);
	free(m);

	struct OpenResponse *m2 = malloc(sizeof(struct OpenResponse));
	if(m2 == NULL)
		return -ENOMEM;

	while(!GetIPCMessageFrom((Message*)m2, CARDINAL_IPCDEST_FILESERVER, msgID_backup))
		Syscall1(Syscall_Nanosleep, 100);

	if(m2->fd != -1){
		int fd = __card_AllocateFD(m2, flags);
		free(m2);
		return fd;
	}

	free(m2);
	return -1;
}

static __inline uint64_t
__card_writev(int fd, uint64_t vecs, int iovec_cnt) {

	typedef struct {
		void *iov_base;
		size_t iov_len;
	} iovec;

	if(!__card_ValidateFD(fd, 1))
		return -EINVAL;

	iovec *v = (iovec*)vecs;

	uint64_t total_size = 0;
	for(int i = 0; i < iovec_cnt; i++)
		total_size += v[i].iov_len;

	uint64_t needed_size = sizeof(struct WriteRequest);

	total_size = v[0].iov_len;
	if(total_size + needed_size > UINT16_MAX)
		total_size = UINT16_MAX - needed_size;

	struct WriteRequest *m = malloc(needed_size + total_size);
	if(m == NULL)
		return -ENOMEM;

	m->m.DestinationPID = __card_fds[fd].TargetPID;
	m->m.MsgID = __card_req_id++;
	m->m.Size = needed_size + total_size;

	m->fd = __card_fds[fd].AdditionalData;
	m->msg_type = CARDINAL_MSG_TYPE_WRITEREQUEST;

	uint8_t* src = (uint8_t*)v[0].iov_base;
	uint8_t* dst = (uint8_t*)m->buf;
	memcpy(dst, src, total_size);

	PostIPCMessages((Message**)&m, 1);

	struct WriteResponse response;
	while(!GetIPCMessageFrom((Message*)&response, __card_fds[fd].TargetPID, m->m.MsgID))
		Syscall1(Syscall_Nanosleep, 100);

	free(m);
	return response.write_size;

	/*
	int num_requests = total_size / (UINT16_MAX - needed_size) + 1;
	Message **m = malloc(num_requests);
	if(m == NULL)
		return -ENOMEM;

	int io_index = 0;
	for(int i = 0; i < num_requests; i++) {
		m[i] = malloc((needed_size + total_size) / num_requests);
		if(m[i] == NULL)
		{
			for(int j = 0; j < i; j++)
				free(m[j]);

			free(m);
			return -ENOMEM;
		}

		m[i]->SourcePID = currentPID;
		m[i]->DestinationPID = __card_fds[fd].TargetPID;
		m[i]->Size = (unsigned short)(needed_size + total_size) / num_requests;

		struct ReadWriteRequest *rwq = (struct ReadWriteRequest*)m[i]->Content;
		rwq->fd = __card_fds[fd].AdditionalData;
		if(i != num_requests - 1) rwq->lock = 1;

		uint8_t* src = (uint8_t*)v[io_index].iov_base;
		uint8_t* dst = (uint8_t*)rwq->buf;

		if()

	}*/
}

static __inline uint64_t
__card_readv(int fd, uint64_t vecs, int iovec_cnt) {

	typedef struct {
		void *iov_base;
		size_t iov_len;
	} iovec;

	if(!__card_ValidateFD(fd, 1))
		return -EINVAL;

	iovec *v = (iovec*)vecs;

	uint64_t total_size = v[0].iov_len;
	uint64_t needed_size = sizeof(struct ReadRequest);

	if(total_size + needed_size > UINT16_MAX)
		total_size = UINT16_MAX - needed_size;
	
	struct ReadRequest read_req;
	read_req.m.DestinationPID = __card_fds[fd].TargetPID;
	read_req.m.MsgID = __card_req_id++;
	read_req.m.Size = sizeof(struct ReadRequest);
	read_req.msg_type = CARDINAL_MSG_TYPE_READREQUEST;
	read_req.fd = __card_fds[fd].AdditionalData;
	read_req.read_size = total_size;
	
	Message *m = (Message*)&read_req;
	PostIPCMessages(&m, 1);

	struct ReadResponse *read_resp = malloc(UINT16_MAX);
	if(read_resp == NULL)
		return ENOMEM;

	while(!GetIPCMessageFrom((Message*)read_resp, __card_fds[fd].TargetPID, read_req.m.MsgID))
		Syscall1(Syscall_Nanosleep, 100);

	if(read_resp->msg_type != CARDINAL_MSG_TYPE_READRESPONSE)
		return EIO;

	uint64_t ret_val = read_resp->code;

	if((int64_t)ret_val >= 0)memcpy(v[0].iov_base, read_resp->data, ret_val);

	free(read_resp);

	return ret_val;
}

static __inline int
__card_fcntl(int fd, int cmd, int arg){

	return 0;
}

static __inline void
__card_Initialize(void) {
	static bool inited = 0;
	if(!inited) {
		inited = 1;

		__card_fds[0].Flags = DescriptorFlags_None;
		__card_fds[0].TargetPID = CARDINAL_IPCDEST_FILESERVER;
		__card_fds[0].AdditionalData = 0;

		for(int i = 1; i < MAX_FILE_DESCRIPTORS; i++) {
			__card_fds[i].Flags = DescriptorFlags_Free;
			__card_fds[i].TargetPID = 0;
			__card_fds[i].AdditionalData = 0;
		}
	}
}

static __inline uint64_t
SyscallEmu0(uint32_t syscall_num) {
    uint64_t ret_error = -ENOSYS;

    switch(syscall_num) {
    	case Cardinal_EmulatedSyscalls_GetPID:
    		ret_error = Syscall2(Syscall_GetProperty, CardinalProperty_PID, 0);
    		__card_currentPID = ret_error;
    	break;
    	case Cardinal_EmulatedSyscalls_Fork:
    		ret_error = Syscall0(Syscall_Fork);
    		__card_currentPID = Syscall2(Syscall_GetProperty, CardinalProperty_PID, 0);
    	break;
    	case Cardinal_EmulatedSyscalls_GetTID:
    		ret_error = Syscall2(Syscall_GetProperty, CardinalProperty_TID, 0);
    	break;
    	default:
    		__asm__("hlt" :: "a"(syscall_num));
    	break;
    }

    return ret_error;
}

static __inline uint64_t
SyscallEmu1(uint32_t syscall_num,
         uint64_t p0) {

    uint64_t ret_error = -ENOSYS;

    switch(syscall_num) {
    	case Cardinal_EmulatedSyscalls_Brk:
    		ret_error = Syscall1(Syscall_Brk, p0);
    	break;
    	case Cardinal_EmulatedSyscalls_Time:
    		ret_error = Syscall2(Syscall_GetProperty, CardinalProperty_Time, 0);
    		if(p0 != 0)*(long*)p0 = (long)ret_error;
    	break;
    	case Cardinal_EmulatedSyscalls_SetTidAddress:
    		ret_error = Syscall3(Syscall_SetProperty, CardinalProperty_SetTidAddress, 0, p0);
    	break;
    	case Cardinal_EmulatedSyscalls_Exit:
    		ret_error = Syscall3(Syscall_SetProperty, CardinalProperty_Exit, 0, p0);
    	break;
    	default:
    		__asm__("hlt" :: "a"(syscall_num));
    	break;
    }

    return ret_error;
}

static __inline uint64_t
SyscallEmu2(uint32_t syscall_num,
         uint64_t p0,
         uint64_t p1) {

    uint64_t ret_error = -ENOSYS;

    //TODO Correct arch_prctl behavior, update newlib inline asm call for it
    switch(syscall_num) {
    	case Cardinal_EmulatedSyscalls_ArchPrctl:
    		ret_error = Syscall3(Syscall_SetProperty, CardinalProperty_ArchPrctl, p0, p1);
    	break;
    	case Cardinal_EmulatedSyscalls_Nanosleep:
    		{
    			struct timespec *tmp = (struct timespec*)p0;
    			ret_error = Syscall1(Syscall_Nanosleep, tmp->tv_sec * 1000000000ULL + tmp->tv_nsec);
    		}
    	break;
    	case Cardinal_EmulatedSyscalls_Open:
    		__card_Initialize();
    		ret_error = __card_open((const char*)p0, (int)p1, (int)0);
    	break;
    	case Cardinal_EmulatedSyscalls_Fcntl:
    		__card_Initialize();
    		ret_error = __card_fcntl((int)p0, (int)p1, 0);
    	break;
    	default:
    		__asm__("hlt" :: "a"(syscall_num));
    	break;
    }

    return ret_error;
}

static __inline uint64_t
SyscallEmu3(uint32_t syscall_num,
         uint64_t p0,
         uint64_t p1,
         uint64_t p2) {

    uint64_t ret_error = -ENOSYS;

    switch(syscall_num) {
    	case Cardinal_EmulatedSyscalls_Open:
    		__card_Initialize();
    		ret_error = __card_open((const char*)p0, (int)p1, (int)p2);
    	break;
    	case Cardinal_EmulatedSyscalls_Write:
    		__card_Initialize();
    		{

			typedef struct {
				void *iov_base;
				size_t iov_len;
			} iovec;

				iovec tmp;
				tmp.iov_base = (void*)p1;
				tmp.iov_len = (size_t)p2;

    			ret_error = __card_writev((int)p0, (uint64_t)&tmp, 1);
    		}
    	break;
    	case Cardinal_EmulatedSyscalls_Read:
    		__card_Initialize();
    		{

			typedef struct {
				void *iov_base;
				size_t iov_len;
			} iovec;

				iovec tmp;
				tmp.iov_base = (void*)p1;
				tmp.iov_len = (size_t)p2;

    			ret_error = __card_readv((int)p0, (uint64_t)&tmp, 1);

    		}
    	break;
    	case Cardinal_EmulatedSyscalls_Writev:
    		__card_Initialize();
    		{
    			ret_error = __card_writev((int)p0, p1, (int)p2);
    		}
    	break;
    	case Cardinal_EmulatedSyscalls_Readv:
    		__card_Initialize();
    		{
    			ret_error = __card_readv((int)p0, p1, (int)p2);
    		}
    	break;
    	case Cardinal_EmulatedSyscalls_Fcntl:
    		__card_Initialize();
    		ret_error = __card_fcntl((int)p0, (int)p1, (int)p2);
    	break;
    	default:
    		__asm__("hlt" :: "a"(syscall_num));
    	break;
    }

    return ret_error;
}

static __inline uint64_t
SyscallEmu4(uint32_t syscall_num,
         uint64_t p0,
         uint64_t p1,
         uint64_t p2,
         uint64_t p3) {

    uint64_t volatile parameters[4];
    parameters[0] = p0;
    parameters[1] = p1;
    parameters[2] = p2;
    parameters[3] = p3;

    uint64_t ret_error = -ENOSYS;

    return ret_error;
}

static __inline uint64_t
SyscallEmu5(uint32_t syscall_num,
         uint64_t p0,
         uint64_t p1,
         uint64_t p2,
         uint64_t p3,
         uint64_t p4) {

    uint64_t volatile parameters[5];
    parameters[0] = p0;
    parameters[1] = p1;
    parameters[2] = p2;
    parameters[3] = p3;
    parameters[4] = p4;

    uint64_t ret_error = -ENOSYS;

    return ret_error;
}

static __inline uint64_t
SyscallEmu6(uint32_t syscall_num,
         uint64_t p0,
         uint64_t p1,
         uint64_t p2,
         uint64_t p3,
         uint64_t p4,
         uint64_t p5) {

    uint64_t ret_error = -ENOSYS;

    switch(syscall_num) {
    	case Cardinal_EmulatedSyscalls_MMap:
    		ret_error = Syscall6(Syscall_MMap, p0, p1, p2, p3, p4, p5);
    	break;
    	default:
    		__asm__("hlt" :: "a"(syscall_num));
    	break;
    }

    return ret_error;
}


#endif