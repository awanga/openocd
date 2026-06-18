/* SPDX-License-Identifier: GPL-2.0-or-later */

/***************************************************************************
 *   EJTAG runtime capability decoding                                     *
 *                                                                         *
 *   Decodes the EJTAG implementation register (impcode) into a structured *
 *   set of runtime capabilities, so that downstream logic can be driven   *
 *   by detected hardware features instead of build-time target type.      *
 ***************************************************************************/

#ifndef OPENOCD_TARGET_MIPS_EJTAG_CAPS_H
#define OPENOCD_TARGET_MIPS_EJTAG_CAPS_H

#include <stdbool.h>
#include <stdint.h>

/*
 * impcode bit positions used for capability decoding.
 *
 * These mirror the EJTAG_IMP_* / EJTAG_V20_IMP_* / EJTAG_V26_IMP_* and
 * version macros in mips_ejtag.h. They are duplicated here, with distinct
 * names, so that this decoder stays free of the heavier mips_ejtag.h
 * (and jtag/jtag.h) dependency chain and can be unit-tested standalone.
 * The bit positions are fixed by the MIPS EJTAG specification and must not
 * diverge from mips_ejtag.h. Keep the two lists in sync.
 */
#define MIPS_EJTAG_CAP_VERSION_SHIFT	29
#define MIPS_EJTAG_CAP_VERSION_MASK	0x7u
#define MIPS_EJTAG_CAP_R3K		(1u << 28)	/* v2.6: R3k vs R4k */
#define MIPS_EJTAG_CAP_DINT		(1u << 24)	/* v2.6: DINT support */
#define MIPS_EJTAG_CAP_ASID8		(1u << 22)
#define MIPS_EJTAG_CAP_ASID6		(1u << 21)
#define MIPS_EJTAG_CAP_EADDR_NO32BIT	(1u << 19)	/* EJTAG_ADDR > 32 bits */
#define MIPS_EJTAG_CAP_MIPS16		(1u << 16)
#define MIPS_EJTAG_CAP_NODMA		(1u << 14)
#define MIPS_EJTAG_CAP_NOPB		(1u << 7)	/* v2.0: no proc breaks */
#define MIPS_EJTAG_CAP_NODB		(1u << 6)	/* v2.0: no data breaks */
#define MIPS_EJTAG_CAP_NOIB		(1u << 5)	/* v2.0: no instr breaks */
#define MIPS_EJTAG_CAP_BCHAN_SHIFT	1		/* v2.0: break channels */
#define MIPS_EJTAG_CAP_BCHAN_MASK	0xfu
#define MIPS_EJTAG_CAP_MIPS64		(1u << 0)

/* EJTAG protocol version, decoded from impcode[31:29]. Matches the
 * EJTAG_VERSION_* enumeration in mips_ejtag.h. */
struct mips_ejtag_caps {
	bool valid;			/* set once decoded from a real impcode */
	unsigned int ejtag_version;	/* 0=2.0, 1=2.5, 2=2.6, 3=3.1, 4=4.1, 5=5.1 */

	/* Core capabilities (all EJTAG versions) */
	bool mips64;			/* 64-bit core (impcode MIPS64 bit) */
	bool dma_supported;		/* hardware advertises DMA (NODMA clear) */
	bool mips16;			/* MIPS16 ASE present */
	bool eaddr_over_32;		/* EJTAG address bus wider than 32 bits */
	unsigned int asid_size;		/* TLB ASID width in bits: 0, 6 or 8 */

	/* EJTAG v2.6 specific */
	bool type_r3k;			/* R3k-style core (else R4k) */
	bool dint_supported;		/* DINT signal available */

	/* EJTAG v2.0 specific break-unit presence */
	bool has_inst_break;		/* instruction breakpoints present */
	bool has_data_break;		/* data breakpoints present */
	bool has_proc_break;		/* processor breakpoints present */
	unsigned int break_channels;	/* number of v2.0 break channels */
};

/**
 * mips_ejtag_decode_caps - decode an impcode word into capability flags.
 * @param impcode  raw EJTAG implementation register value
 * @param caps     output structure, fully overwritten (and marked valid)
 *
 * Pure function: no JTAG access, no global state. The v2.0-specific fields
 * (break presence/channels) are only meaningful when ejtag_version == 0.
 */
void mips_ejtag_decode_caps(uint32_t impcode, struct mips_ejtag_caps *caps);

#endif /* OPENOCD_TARGET_MIPS_EJTAG_CAPS_H */
