#include "program.h"
#include "common.h"

void
SetupApplicationStack(void *sp,
					  uint32_t argc,
					  const char **argv,
					  const char **envp,
					  const AUXVector *aux_vectors,
					  uint32_t auxv_cnt)
{
	uint64_t *stack = (uint64_t*)sp;
	stack[0] = argc;
	for(uint32_t i = 0; i < argc; i++)
		stack[i + 1] = (uint64_t)argv[i];
	stack[argc + 1] = 0;

	uint32_t offset = argc + 2;

	if(envp != NULL)
	for(uint32_t i = 0; envp[i] != NULL; i++)
		stack[offset++] = (uint64_t)envp[i];

	stack[offset++] = 0;

	memcpy(&stack[offset], (void*)aux_vectors, auxv_cnt * sizeof(AUXVector));

	offset = argc + 3 + (auxv_cnt * sizeof(AUXVector))/sizeof(uint64_t);

	AUXVector *auxv = (AUXVector*)&stack[offset];
	auxv->a_type = AUXVectorType_NULL;
}