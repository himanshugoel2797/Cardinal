#ifndef _CARDINAL_VMWARE_SVGA_SVGA_H_
#define _CARDINAL_VMWARE_SVGA_SVGA_H_

#include <cardinal/cardinal_types.h>
#include <pci/pci.h>

#include "svga_reg.h"
#include "svga_escape.h"
#include "svga_overlay.h"

typedef struct {
	PCI_Device device;

	uint64_t iobase;
	uint64_t fbmem;
	uint32_t *fifomem;

	uint64_t fifosz;
	uint64_t fbsz;
	uint64_t vramsz;

	uint32_t devVer;
	uint32_t caps;

	uint32_t width;
	uint32_t height;
	uint32_t bpp;
	uint32_t pitch;


   struct {
      uint32_t rsv_sz;
      uint32_t usingBounceBuffer;
      uint8_t *bounceBuffer;
      uint64_t bounceBufferPhys;
      uint32_t nextFence;
   } fifo;

} SVGA_Context;

void
SVGA_Init(SVGA_Context *ctxt);

void
SVGA_Enable(SVGA_Context *ctxt);

void
SVGA_SetMode(SVGA_Context *ctxt,
			 uint32_t width,
			 uint32_t height,
			 uint32_t bpp);

/**********************************************************
 * Copyright 2008-2009 VMware, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************/

uint32_t
SVGA_IsFIFORegisterValid(SVGA_Context *ctxt,
						 uint32_t reg);

uint32_t
SVGA_FIFOCapabilityAvailable(SVGA_Context *ctxt,
							 uint32_t cap);

void *
SVGA_FIFOReserve(SVGA_Context *ctxt,
				 uint32_t bytes);

void
SVGA_FIFOFull(SVGA_Context *ctxt);

void
SVGA_FIFOCommit(SVGA_Context *ctxt, 
				uint32_t bytes);

void
SVGA_FIFOCommitAll(SVGA_Context *ctxt);

void*
SVGA_FIFOReserveCmd(SVGA_Context *ctxt,
					uint32_t type,
                    uint32_t bytes);

void*
SVGA_FIFOReserveEscape(SVGA_Context *ctxt,
					   uint32_t nsid,
                       uint32_t bytes);

uint32_t
SVGA_InsertFence(SVGA_Context *ctxt);

void
SVGA_SyncToFence(SVGA_Context *ctxt,
				 uint32_t fence);

uint8_t
SVGA_HasFencePassed(SVGA_Context *ctxt,
					uint32_t fence);

void
SVGA_RingDoorbell(SVGA_Context *ctxt);


void*
SVGA_AllocGMR(SVGA_Context *ctxt,
			  uint32_t size,
              SVGAGuestPtr *ptr);

void
SVGA_Update(SVGA_Context *ctxt,
			uint32_t x,
            uint32_t y,
            uint32_t width,
            uint32_t height);

void
SVGA_BeginDefineCursor(SVGA_Context *ctxt,
					   const SVGAFifoCmdDefineCursor *cursorInfo,
                       void **andMask,
                       void **xorMask);

void
SVGA_BeginDefineAlphaCursor(SVGA_Context *ctxt,
							const SVGAFifoCmdDefineAlphaCursor *cursorInfo,
                            void **data);

void
SVGA_MoveCursor(SVGA_Context *ctxt,
				uint32_t visible,
                uint32_t x,
                uint32_t y,
                uint32_t screenId);

void
SVGA_BeginVideoSetRegs(SVGA_Context *ctxt,
					   uint32_t streamId,
                       uint32_t numItems,
                       SVGAEscapeVideoSetRegs **setRegs);

void
SVGA_VideoSetAllRegs(SVGA_Context *ctxt,
					 uint32_t streamId,
                     SVGAOverlayUnit *regs,
                     uint32_t maxReg);

void
SVGA_VideoSetReg(SVGA_Context *ctxt,
				 uint32_t streamId,
                 uint32_t registerId,
                 uint32_t value);

void
SVGA_VideoFlush(SVGA_Context *ctxt,
				uint32_t streamId);

#endif