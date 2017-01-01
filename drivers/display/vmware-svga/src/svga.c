/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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

#include <cardinal/driver_utils.h>
#include <cardinal/shared_memory.h>
#include <cardinal/mem/server.h>
#include <pci/pci.h>

#include <string.h>


#include "svga3d_reg.h"
#include "svga_reg.h"

#include "svga.h"
#include "helpers.h"

void
SVGA_Init(SVGA_Context *ctxt) {

    ctxt->devVer = SVGA_ID_2;
    do {
        SVGA_Write32(ctxt, SVGA_REG_ID, ctxt->devVer);
        if (SVGA_Read32(ctxt, SVGA_REG_ID) == ctxt->devVer) {
            break;
        } else {
            ctxt->devVer--;
        }
    } while (ctxt->devVer >= SVGA_ID_0);

    ctxt->vramsz = SVGA_Read32(ctxt, SVGA_REG_VRAM_SIZE);
    ctxt->fbsz = SVGA_Read32(ctxt, SVGA_REG_FB_SIZE);
    ctxt->fifosz = SVGA_Read32(ctxt, SVGA_REG_MEM_SIZE);

    if(ctxt->devVer >= SVGA_ID_1)
        ctxt->caps = SVGA_Read32(ctxt, SVGA_REG_CAPABILITIES);

    SVGA_Enable(ctxt);
}

void
SVGA_Enable(SVGA_Context *ctxt) {

    ctxt->fifomem[SVGA_FIFO_MIN] = SVGA_FIFO_NUM_REGS * sizeof(uint32_t);
    ctxt->fifomem[SVGA_FIFO_MAX] = ctxt->fifosz;
    ctxt->fifomem[SVGA_FIFO_NEXT_CMD] = ctxt->fifomem[SVGA_FIFO_MIN];
    ctxt->fifomem[SVGA_FIFO_STOP] = ctxt->fifomem[SVGA_FIFO_MIN];

    //Some setup work for 3d acceleration
    if(SVGA_FIFOCapabilityAvailable(ctxt, SVGA_CAP_EXTENDED_FIFO) &&
            SVGA_IsFIFORegisterValid(ctxt, SVGA_FIFO_GUEST_3D_HWVERSION))
        ctxt->fifomem[SVGA_FIFO_GUEST_3D_HWVERSION] = SVGA3D_HWVERSION_CURRENT;

    SVGA_Write32(ctxt, SVGA_REG_ENABLE, 1);
    SVGA_Write32(ctxt, SVGA_REG_CONFIG_DONE, 1);
}

void
SVGA_SetMode(SVGA_Context *ctxt,
             uint32_t width,
             uint32_t height,
             uint32_t bpp) {
    ctxt->width = width;
    ctxt->height = height;
    ctxt->bpp = bpp;


    SVGA_Write32(ctxt, SVGA_REG_WIDTH, width);
    SVGA_Write32(ctxt, SVGA_REG_HEIGHT, height);
    SVGA_Write32(ctxt, SVGA_REG_BITS_PER_PIXEL, bpp);
    SVGA_Write32(ctxt, SVGA_REG_ENABLE, 1);
    ctxt->pitch = SVGA_Read32(ctxt, SVGA_REG_BYTES_PER_LINE);
}

uint32_t
SVGA_IsFIFORegisterValid(SVGA_Context *ctxt,
                         uint32_t reg) {

    return ctxt->fifomem[SVGA_FIFO_MIN] > (reg << 2);
}

uint32_t
SVGA_FIFOCapabilityAvailable(SVGA_Context *ctxt,
                             uint32_t cap) {
    return (ctxt->caps & cap);
}

void *
SVGA_FIFOReserve(SVGA_Context *ctxt,
                 uint32_t bytes) { // IN
    volatile uint32_t *fifo = ctxt->fifomem;
    uint32_t max = fifo[SVGA_FIFO_MAX];
    uint32_t min = fifo[SVGA_FIFO_MIN];
    uint32_t nextCmd = fifo[SVGA_FIFO_NEXT_CMD];
    uint32_t reserveable = SVGA_FIFOCapabilityAvailable(ctxt, SVGA_FIFO_CAP_RESERVE);

    if (bytes > sizeof ctxt->fifo.bounceBuffer ||
            bytes > (max - min)) {
        //TODO handle error, command is too large
        return NULL;
    }

    if (bytes % sizeof(uint32_t)) {
        return NULL;	//Size is not uint32_t aligned
    }

    if (ctxt->fifo.rsv_sz != 0) {
        //First need to commit
    }

    ctxt->fifo.rsv_sz = bytes;

    while (1) {
        uint32_t stop = fifo[SVGA_FIFO_STOP];
        uint32_t reserveInPlace = 0;
        uint32_t needBounce = 0;

        if (nextCmd >= stop) {

            if (nextCmd + bytes < max ||
                    (nextCmd + bytes == max && stop > min)) {
                reserveInPlace = 1;
            } else if ((max - nextCmd) + (stop - min) <= bytes) {
                SVGA_FIFOFull(ctxt);
            } else {
                needBounce = 1;
            }

        } else {
            if (nextCmd + bytes < stop) {
                reserveInPlace = 1;
            } else {
                SVGA_FIFOFull(ctxt);
            }
        }

        if (reserveInPlace) {
            if (reserveable || bytes <= sizeof(uint32)) {
                ctxt->fifo.usingBounceBuffer = 0;
                if (reserveable) {
                    fifo[SVGA_FIFO_RESERVED] = bytes;
                }
                return nextCmd + (uint8*) fifo;
            } else {
                needBounce = 1;
            }
        }

        if (needBounce) {
            ctxt->fifo.usingBounceBuffer = 1;
            return ctxt->fifo.bounceBuffer;
        }
    }


}

void
SVGA_FIFOFull(SVGA_Context *ctxt) {
    SVGA_Write32(ctxt, SVGA_REG_SYNC, 1);
    SVGA_Read32(ctxt, SVGA_REG_BUSY);
}

void
SVGA_FIFOCommit(SVGA_Context *ctxt,
                uint32_t bytes) {
    volatile uint32_t *fifo = ctxt->fifomem;
    uint32_t nextCmd = fifo[SVGA_FIFO_NEXT_CMD];
    uint32_t max = fifo[SVGA_FIFO_MAX];
    uint32_t min = fifo[SVGA_FIFO_MIN];
    uint8_t reserveable = SVGA_FIFOCapabilityAvailable(ctxt, SVGA_FIFO_CAP_RESERVE);

    if (ctxt->fifo.rsv_sz == 0) {
        //Error: Commit before reserve
    }
    ctxt->fifo.rsv_sz = 0;

    if (ctxt->fifo.usingBounceBuffer) {
        uint8_t *buffer = ctxt->fifo.bounceBuffer;

        if (reserveable) {
            /*
             * Slow path: bulk copy out of a bounce buffer in two chunks.
             *
             * Note that the second chunk may be zero-length if the reserved
             * size was large enough to wrap around but the commit size was
             * small enough that everything fit contiguously into the FIFO.
             *
             * Note also that we didn't need to tell the FIFO about the
             * reservation in the bounce buffer, but we do need to tell it
             * about the data we're bouncing from there into the FIFO.
             */

            uint32_t chunkSize = (bytes > (max - nextCmd)) ? (max - nextCmd) : bytes;
            fifo[SVGA_FIFO_RESERVED] = bytes;
            memcpy(nextCmd + (uint8_t*) fifo, buffer, chunkSize);
            memcpy(min + (uint8_t*) fifo, buffer + chunkSize, bytes - chunkSize);

        } else {
            /*
             * Slowest path: copy one dword at a time, updating NEXT_CMD as
             * we go, so that we bound how much data the guest has written
             * and the host doesn't know to checkpoint.
             */

            uint32_t *dword = (uint32_t *)buffer;

            while (bytes > 0) {
                fifo[nextCmd / sizeof *dword] = *dword++;
                nextCmd += sizeof *dword;
                if (nextCmd == max) {
                    nextCmd = min;
                }
                fifo[SVGA_FIFO_NEXT_CMD] = nextCmd;
                bytes -= sizeof *dword;
            }
        }
    }

    if (!ctxt->fifo.usingBounceBuffer || reserveable) {
        nextCmd += bytes;
        if (nextCmd >= max) {
            nextCmd -= max - min;
        }
        fifo[SVGA_FIFO_NEXT_CMD] = nextCmd;
    }

    if (reserveable) {
        fifo[SVGA_FIFO_RESERVED] = 0;
    }
}

void
SVGA_FIFOCommitAll(SVGA_Context *ctxt) {
    SVGA_FIFOCommit(ctxt, ctxt->fifo.rsv_sz);
}

void*
SVGA_FIFOReserveCmd(SVGA_Context *ctxt,
                    uint32_t type,
                    uint32_t bytes) {
    uint32_t *cmd = SVGA_FIFOReserve(ctxt, bytes + sizeof type);
    cmd[0] = type;
    return cmd + 1;
}

void*
SVGA_FIFOReserveEscape(SVGA_Context *ctxt,
                       uint32_t nsid,
                       uint32_t bytes) {
    uint32_t paddedBytes = (bytes + 3) & ~3UL;
    struct {
        uint32_t cmd;
        uint32_t nsid;
        uint32_t size;
    } __attribute__ ((__packed__)) *header = SVGA_FIFOReserve(ctxt, paddedBytes
            + sizeof *header);

    header->cmd = SVGA_CMD_ESCAPE;
    header->nsid = nsid;
    header->size = bytes;

    return header + 1;
}

uint32_t
SVGA_InsertFence(SVGA_Context *ctxt) {
    uint32_t fence;

    struct {
        uint32_t id;
        uint32_t fence;
    } __attribute__ ((__packed__)) *cmd;

    if (!SVGA_FIFOCapabilityAvailable(ctxt, SVGA_FIFO_CAP_FENCE)) {
        return 1;
    }

    if (ctxt->fifo.nextFence == 0) {
        ctxt->fifo.nextFence = 1;
    }
    fence = ctxt->fifo.nextFence++;

    cmd = SVGA_FIFOReserve(ctxt, sizeof *cmd);
    cmd->id = SVGA_CMD_FENCE;
    cmd->fence = fence;
    SVGA_FIFOCommitAll(ctxt);

    return fence;
}

void
SVGA_SyncToFence(SVGA_Context *ctxt,
                 uint32_t fence) {
    if (!fence) {
        return;
    }

    if (!SVGA_FIFOCapabilityAvailable(ctxt, SVGA_FIFO_CAP_FENCE)) {
        /*
         * Fall back on the legacy sync if the host does not support
         * fences.  This is the old sync mechanism that has been
         * supported in the SVGA device pretty much since the dawn of
         * time: write to the SYNC register, then read from BUSY until
         * it's nonzero. This will drain the entire FIFO.
         *
         * The parameter we write to SVGA_REG_SYNC is an arbitrary
         * nonzero value which can be used for debugging, but which is
         * ignored by release builds of VMware products.
         */

        SVGA_Write32(ctxt, SVGA_REG_SYNC, 1);
        while (SVGA_Read32(ctxt, SVGA_REG_BUSY) != 0);
        return;
    }

    if (SVGA_HasFencePassed(ctxt, fence)) {
        /*
         * Nothing to do
         */

        return;
    }

    /*
     * Sync-to-fence mechanism for older hosts. Wake up the host,
     * and spin on BUSY until we've reached the fence. This
     * processes FIFO commands synchronously, blocking the VM's
     * execution entirely until it's done.
     */

    uint8_t busy = 1;

    SVGA_Write32(ctxt, SVGA_REG_SYNC, 1);

    while (!SVGA_HasFencePassed(ctxt, fence) && busy) {
        busy = (SVGA_Read32(ctxt, SVGA_REG_BUSY) != 0);
    }
}

uint8_t
SVGA_HasFencePassed(SVGA_Context *ctxt,
                    uint32_t fence) {
    if (!fence) {
        return 1;
    }

    if (!SVGA_FIFOCapabilityAvailable(ctxt, SVGA_FIFO_CAP_FENCE)) {
        return 0;
    }

    return ((int32_t)(ctxt->fifomem[SVGA_FIFO_FENCE] - fence)) >= 0;
}

void
SVGA_RingDoorbell(SVGA_Context *ctxt) {
    if (SVGA_IsFIFORegisterValid(ctxt, SVGA_FIFO_BUSY) &&
            ctxt->fifomem[SVGA_FIFO_BUSY] == 0) {

        /* Remember that we already rang the doorbell. */
        ctxt->fifomem[SVGA_FIFO_BUSY] = 1;

        SVGA_Write32(ctxt, SVGA_REG_SYNC, 1);
    }
}

void*
SVGA_AllocGMR(SVGA_Context *ctxt,
              uint32_t size,
              SVGAGuestPtr *ptr) {
    static SVGAGuestPtr nextPtr = { SVGA_GMR_FRAMEBUFFER, 0 };
    *ptr = nextPtr;
    nextPtr.offset += size;
    return (void*)(ctxt->fbmem + ptr->offset);
}

void
SVGA_Update(SVGA_Context *ctxt,
            uint32_t x,
            uint32_t y,
            uint32_t width,
            uint32_t height) {
    SVGAFifoCmdUpdate *cmd = SVGA_FIFOReserveCmd(ctxt, SVGA_CMD_UPDATE, sizeof *cmd);
    cmd->x = x;
    cmd->y = y;
    cmd->width = width;
    cmd->height = height;
    SVGA_FIFOCommitAll(ctxt);
}

void
SVGA_BeginDefineCursor(SVGA_Context *ctxt,
                       const SVGAFifoCmdDefineCursor *cursorInfo,
                       void **andMask,
                       void **xorMask) {
    uint32_t andPitch = ((cursorInfo->andMaskDepth * cursorInfo->width + 31) >> 5) << 2;
    uint32_t andSize = andPitch * cursorInfo->height;
    uint32_t xorPitch = ((cursorInfo->xorMaskDepth * cursorInfo->width + 31) >> 5) << 2;
    uint32_t xorSize = xorPitch * cursorInfo->height;

    SVGAFifoCmdDefineCursor *cmd = SVGA_FIFOReserveCmd(ctxt,
                                   SVGA_CMD_DEFINE_CURSOR,
                                   sizeof *cmd + andSize + xorSize);
    *cmd = *cursorInfo;
    *andMask = (void*) (cmd + 1);
    *xorMask = (void*) (andSize + (uint8*) *andMask);
}

void
SVGA_BeginDefineAlphaCursor(SVGA_Context *ctxt,
                            const SVGAFifoCmdDefineAlphaCursor *cursorInfo,
                            void **data) {
    uint32_t imageSize = cursorInfo->width * cursorInfo->height * sizeof(uint32_t);
    SVGAFifoCmdDefineAlphaCursor *cmd = SVGA_FIFOReserveCmd(ctxt,
                                        SVGA_CMD_DEFINE_ALPHA_CURSOR,
                                        sizeof *cmd + imageSize);
    *cmd = *cursorInfo;
    *data = (void*) (cmd + 1);
}

void
SVGA_MoveCursor(SVGA_Context *ctxt,
                uint32_t visible,
                uint32_t x,
                uint32_t y,
                uint32_t screenId) {
    if (SVGA_FIFOCapabilityAvailable(ctxt, SVGA_FIFO_CAP_SCREEN_OBJECT)) {
        ctxt->fifomem[SVGA_FIFO_CURSOR_SCREEN_ID] = screenId;
    }

    if (SVGA_FIFOCapabilityAvailable(ctxt, SVGA_FIFO_CAP_CURSOR_BYPASS_3)) {
        ctxt->fifomem[SVGA_FIFO_CURSOR_ON] = visible;
        ctxt->fifomem[SVGA_FIFO_CURSOR_X] = x;
        ctxt->fifomem[SVGA_FIFO_CURSOR_Y] = y;
        ctxt->fifomem[SVGA_FIFO_CURSOR_COUNT]++;
    }
}

void
SVGA_BeginVideoSetRegs(SVGA_Context *ctxt,
                       uint32_t streamId,
                       uint32_t numItems,
                       SVGAEscapeVideoSetRegs **setRegs) {
    SVGAEscapeVideoSetRegs *cmd;
    uint32_t cmdSize = (sizeof *cmd
                        - sizeof cmd->items
                        + numItems * sizeof cmd->items[0]);

    cmd = SVGA_FIFOReserveEscape(ctxt, SVGA_ESCAPE_NSID_VMWARE, cmdSize);
    cmd->header.cmdType = SVGA_ESCAPE_VMWARE_VIDEO_SET_REGS;
    cmd->header.streamId = streamId;

    *setRegs = cmd;
}

void
SVGA_VideoSetAllRegs(SVGA_Context *ctxt,
                     uint32_t streamId,
                     SVGAOverlayUnit *regs,
                     uint32_t maxReg) {
    uint32_t *regArray = (uint32_t*) regs;
    const uint32_t numRegs = maxReg + 1;
    SVGAEscapeVideoSetRegs *setRegs;
    uint32_t i;

    SVGA_BeginVideoSetRegs(ctxt, streamId, numRegs, &setRegs);

    for (i = 0; i < numRegs; i++) {
        setRegs->items[i].registerId = i;
        setRegs->items[i].value = regArray[i];
    }

    SVGA_FIFOCommitAll(ctxt);
}

void
SVGA_VideoSetReg(SVGA_Context *ctxt,
                 uint32_t streamId,
                 uint32_t registerId,
                 uint32_t value) {
    SVGAEscapeVideoSetRegs *setRegs;

    SVGA_BeginVideoSetRegs(ctxt, streamId, 1, &setRegs);
    setRegs->items[0].registerId = registerId;
    setRegs->items[0].value = value;
    SVGA_FIFOCommitAll(ctxt);
}

void
SVGA_VideoFlush(SVGA_Context *ctxt,
                uint32_t streamId) {
    SVGAEscapeVideoFlush *cmd;

    cmd = SVGA_FIFOReserveEscape(ctxt, SVGA_ESCAPE_NSID_VMWARE, sizeof *cmd);
    cmd->cmdType = SVGA_ESCAPE_VMWARE_VIDEO_FLUSH;
    cmd->streamId = streamId;
    SVGA_FIFOCommitAll(ctxt);
}
