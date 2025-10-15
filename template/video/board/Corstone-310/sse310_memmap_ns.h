

#ifndef SSE310_MEMMAP_NS_H
#define SSE310_MEMMAP_NS_H

/*============================================================================*/
/*                           NON-SECURE MEMORY REGIONS                       */
/*============================================================================*/

/*---------------------------------------------------------------------------*/
/*                        Non-Secure Code Regions                           */
/*---------------------------------------------------------------------------*/

/* ITCM - Non-Secure */
#define ITCM_NS_BASE                    0x00000000
#define ITCM_NS_SIZE                    0x00008000    /* 32KB */
#define ITCM_NS_LIMIT                   (ITCM_NS_BASE + ITCM_NS_SIZE - 1)

/* FPGA SRAM - Non-Secure */
#define FPGA_SRAM_NS_BASE               0x01000000
#define FPGA_SRAM_NS_SIZE               0x00200000    /* 2MB */
#define FPGA_SRAM_NS_LIMIT              (FPGA_SRAM_NS_BASE + FPGA_SRAM_NS_SIZE - 1)

/*---------------------------------------------------------------------------*/
/*                         Non-Secure SRAM Regions                          */
/*---------------------------------------------------------------------------*/

/* DTCM - Non-Secure */
#define DTCM_NS_BASE                    0x20000000
#define DTCM_NS_SIZE                    0x00008000    /* 32KB */
#define DTCM_NS_LIMIT                   (DTCM_NS_BASE + DTCM_NS_SIZE - 1)

/* SRAM VM0 - Non-Secure */
#define SRAM_VM0_NS_BASE                0x21000000
#define SRAM_VM0_NS_SIZE                0x00200000    /* 2MB */
#define SRAM_VM0_NS_LIMIT               (SRAM_VM0_NS_BASE + SRAM_VM0_NS_SIZE - 1)

/* SRAM VM1 - Non-Secure */
#define SRAM_VM1_NS_BASE                0x21200000
#define SRAM_VM1_NS_SIZE                0x00200000    /* 2MB */
#define SRAM_VM1_NS_LIMIT               (SRAM_VM1_NS_BASE + SRAM_VM1_NS_SIZE - 1)

/*---------------------------------------------------------------------------*/
/*                         Secure QSPI Flash Regions                         */
/*---------------------------------------------------------------------------*/

/* QSPI Flash - Secure */
#define QSPI_FLASH_NS_BASE              0x28000000
#define QSPI_FLASH_NS_SIZE              0x00800000    /* 8MB */
#define QSPI_FLASH_NS_LIMIT             (QSPI_FLASH_NS_BASE + QSPI_FLASH_NS_SIZE - 1)

/*---------------------------------------------------------------------------*/
/*                        Non-Secure DDR4 Regions                           */
/*---------------------------------------------------------------------------*/

/* DDR4 Region 0 - Non-Secure */
#define DDR4_0_NS_BASE                  0x60000000
#define DDR4_0_NS_SIZE                  0x10000000    /* 256MB */
#define DDR4_0_NS_LIMIT                 (DDR4_0_NS_BASE + DDR4_0_NS_SIZE - 1)

/* DDR4 Region 2 - Non-Secure */
#define DDR4_2_NS_BASE                  0x80000000
#define DDR4_2_NS_SIZE                  0x10000000    /* 256MB */
#define DDR4_2_NS_LIMIT                 (DDR4_2_NS_BASE + DDR4_2_NS_SIZE - 1)

/* DDR4 Region 4 - Non-Secure */
#define DDR4_4_NS_BASE                  0xA0000000
#define DDR4_4_NS_SIZE                  0x10000000    /* 256MB */
#define DDR4_4_NS_LIMIT                 (DDR4_4_NS_BASE + DDR4_4_NS_SIZE - 1)

/* DDR4 Region 6 - Non-Secure */
#define DDR4_6_NS_BASE                  0xC0000000
#define DDR4_6_NS_SIZE                  0x10000000    /* 256MB */
#define DDR4_6_NS_LIMIT                 (DDR4_6_NS_BASE + DDR4_6_NS_SIZE - 1)

#endif /* SSE310_MEMMAP_NS_H */
