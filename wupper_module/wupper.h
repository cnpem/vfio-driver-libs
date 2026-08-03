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
#include <cstring>
#include <unistd.h>

class Wupper {
public:
    /**
     * @brief Initializes an interface for operation of a Wupper device
     * @description Initializes a VFIO interface for the device identified by
     * the device_name and iommu_group_id and uses it to create DataBuffer
     * references to the device's BARs
     * @param[in] device_name BDBF identification of the device
     * @param[in] iommu_group_id ID of the IOMMU group to which the device
     * belongs
     */
    Wupper(const std::string device_name, int iommu_group_id);
    ~Wupper();

    /**
     * @brief Transfers data from the device to the host via DMA
     * @description Writes the adequate values on the DMA descriptor indicated
     * by dma_index to make a DMA data transfer with the given buffer as
     * destination. Before starting the transfer, it calls the VFIO interface to
     * map the given buffer for DMA transfers
     * @param[in] transfer_size Size in bytes of the transfer
     * @param[in] dma_index DMA descriptor index, expected to be a member from
     * the enum defined in register_map
     * @param[in] wrap_around If false, DMA will operate in single transfer mode, if
     * true will operate in wrap_around mode
     * @param[in] buffer Reference to the DataBuffer object that points to the
     * memory buffer that will receive the transfer
     */
    void dma_to_host(
        size_t transfer_size, int dma_index, bool wrap_around, DataBuffer &buffer);

    /**
     * @brief Returns whether the one-shot DMA transfer from the indexed DMA
     * descriptor is done or not
     * @description The DMA_DESC_ENABLE bit correspondent to the indexed DMA
     * descriptor is set to start the transfer and is cleared when it is done,
     * so the bit is read and is complement is returned as a bool
     * @param[in] dma_index DMA descriptor index, expected to be a member from
     * the enum defined in register_map
     * @return Complement of the DMA_DESC_ENABLE bit of the indexed DMMA
     * descriptor
     */
    bool dma_is_done(int dma_index);

    /**
     * @brief Returns the value of the even_cycle bit on the indexed DMA
     * descriptor status register as a boolean
     * @param[in] dma_index DMA descriptor index, expected to be a member from
     * the enum defined in register_map
     * @return Value of the even_cycle bit
     */
    bool dma_get_even_addr(int dma_index);

    /**
     * @brief Returns the last address written by DMA
     * @param[in] dma_index DMA descriptor index, expected to be a member from
     * the enum defined in register_map
     * @return Value of the current_address field of the indexed DMA descriptor
     * status register
     */
    u_long dma_get_current_addr(int dma_index);

    /**
     * @brief Updates field of the indexed DMA descriptor that indicates last
     * read address from the DMA mapped region
     * @param[in] dma_index DMA descriptor index, expected to be a member from
     * the enum defined in register_map
     */
    void dma_update_read_ptr(int dma_index);

    VFIO interface;
    DataBuffer bar0;
    wuppercard_bar0_regs_t *bar0_regs;
};
