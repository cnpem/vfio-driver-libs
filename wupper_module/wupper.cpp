/*******************************************************************/
/*                                                                 */
/* This is the C++ source code of the WupperCard object               */
/*                                                                 */
/* Author: Markus Joos, CERN                                       */
/*                                                                 */
/**C 2019 Ecosoft - Made from at least 80% recycled source code*****/

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

#include "wupper.h"

// Constants
#define MAX_TLP_BYTES 256

Wupper::Wupper() {}

void Wupper::dma_to_host(size_t transfer_size, uint64_t start, uint8_t dma_index)
{
    // Create and enable DMA descriptor (Wupper)
    volatile dma_descriptor_t &dma_desc = bar0->DMA_DESC[dma_index];

    // Address 0x0000 - DMA_DESC_0
    dma_desc.start_address = start;
    dma_desc.end_address = start + transfer_size;

    // Adderss 0x0030 - DMA_DESC_1a
    dma_desc.tlp = MAX_TLP_BYTES / 4;
    dma_desc.read = 0;
    dma_desc.wrap_around = 0;

    // Address 0x0400 - DMA_DESC_ENABLE
    bar0->DMA_DESC_ENABLE |= 0x1 << dma_index;
}

Wupper::~Wupper() {}
