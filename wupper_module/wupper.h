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

#include "../vfio_uapi.h"
#include "register_map.h"
#include <cstdint>
#include <unistd.h>
#include "register_map.h"

class Wupper {
public:
    Wupper();
    ~Wupper();

    // This pointers need to be mapped to the memory regions by the user
    volatile wuppercard_bar0_regs_t *bar0;

    void dma_to_host(size_t transfer_size, uint64_t start, uint8_t dma_index);
};
