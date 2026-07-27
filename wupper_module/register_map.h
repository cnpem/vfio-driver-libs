/******************************************************************* 
 * \mainpage                                                       * 
 *                                                                 * 
 * @author: Markus Joos, CERN                                      *
 *  Maintainer: Henk Boterenbrood, Nikhef                          * 
 *                                                                 *
 * @brief                                                          *
 * This note defines an application program interface (API) for    *
 * the use of the WUPPER PCIe I/O card in the ATLAS read-out system.  *
 * The intention of the API is to satisfy the needs of simple      *
 * test programs as well as the requirements of the                *
 * FelixApplication.                                               *
 *                                                                 *
 * @copyright CERN, Nikhef                                         *
 ******************************************************************/ 

/***************************************************************************
 * Copyright (C) 2001-2021 CERN for the benefit of the ATLAS collaboration.*
 * Licensed under the Apache License, version 2.0.                         *
 *                                                                         *
 * Contributors                                                            *
 * ============                                                            *
 * Andrea Borga <andrea.borga@cern.ch>                                     *
 * Carlo Alberto Gottardo <carlo.gottardo@cern.ch>                         *
 * Elena Zhivun <elena.zhivun@cern.ch>                                     *
 * Enrico Gamberini <enrico.gamberini@cern.ch>                             *
 * Frans Schreuder <fransschreuder@gmail.com>                              *
 * Gokhan UNEL <gokhan.unel@cern.ch>                                       *
 * Henk Boterenbrood <boterenbrood@nikhef.nl>                              *
 * Jorn Schumacher <jorn.schumacher@cern.ch>                               *
 * Mark Donszelmann <Mark.Donszelmann@cern.ch>                             *
 **************************************************************************/

/*******************************************************************
 *                                                                 *
 * The following source is derivative work, original copyright     *
 * and attribution notice above                                    *
 ******************************************************************/

// Simplified register model
typedef struct {
    volatile unsigned long start_address; /*  low half, bits  63:00 */
    volatile unsigned long end_address; /*  low half, bits 127:64 */
    volatile unsigned long tlp : 11; /* high half, bits  10:00 */
    volatile unsigned long read : 1; /* high half, bit      11 */
    volatile unsigned long wrap_around : 1; /* high half, bit      12 */
    volatile unsigned long reserved : 51; /* high half, bits  63:13 */
    volatile unsigned long read_ptr; /* high half, bits 127:64 */
} dma_descriptor_t;

typedef struct {
    volatile unsigned long current_address; /* bits  63:00 */
    volatile unsigned long descriptor_done : 1; /* bit      64 */
    volatile unsigned long even_addr_dma : 1; /* bit      65 */
    volatile unsigned long even_addr_pc : 1; /* bit      66 */
} dma_status_t;

typedef struct {
    dma_descriptor_t DMA_DESC[8]; /* 0x000 - 0x0ff */
    unsigned char unused1[256]; /* 0x100 - 0x1ff */
    dma_status_t DMA_DESC_STATUS[8]; /* 0x200 - 0x27f */
    unsigned char unused2[128]; /* 0x280 - 0x2ff */
    volatile unsigned int BAR0_VALUE; /* 0x300 - 0x303 */
    unsigned char unused3[12]; /* 0x304 - 0x30f */
    volatile unsigned int BAR1_VALUE; /* 0x310 - 0x313 */
    unsigned char unused4[12]; /* 0x314 - 0x31f */
    volatile unsigned int BAR2_VALUE; /* 0x320 - 0x323 */
    unsigned char unused5[220]; /* 0x324 - 0x3ff */
    volatile unsigned int DMA_DESC_ENABLE; /* 0x400 - 0x403 */
    unsigned char unused7[28]; /* 0x404 - 0x41f */
    volatile unsigned int DMA_RESET; /* 0x420 - 0x423 */
    unsigned char unused8[12]; /* 0x424 - 0x42f */
    volatile unsigned int SOFT_RESET; /* 0x430 - 0x433 */
    unsigned char unused9[12]; /* 0x434 - 0x43f */
    volatile unsigned int REGISTERS_RESET; /* 0x440 - 0x443 */
} wuppercard_bar0_regs_t;

enum {
    DMA_DESC_0,
    DMA_DESC_1,
    DMA_DESC_2,
    DMA_DESC_3,
    DMA_DESC_4,
    DMA_DESC_5,
    DMA_DESC_6,
    DMA_DESC_7,
};
