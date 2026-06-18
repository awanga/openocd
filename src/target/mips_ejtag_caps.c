// SPDX-License-Identifier: GPL-2.0-or-later

/***************************************************************************
 *   EJTAG runtime capability decoding                                     *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "mips_ejtag_caps.h"

void mips_ejtag_decode_caps(uint32_t impcode, struct mips_ejtag_caps *caps)
{
	memset(caps, 0, sizeof(*caps));

	caps->ejtag_version =
		(impcode >> MIPS_EJTAG_CAP_VERSION_SHIFT) & MIPS_EJTAG_CAP_VERSION_MASK;

	/* Core capabilities, valid for all EJTAG versions. dma_supported is
	 * taken straight from the impcode NODMA bit, so it reflects the true
	 * hardware capability even if higher layers later choose to gate DMA. */
	caps->mips64 = (impcode & MIPS_EJTAG_CAP_MIPS64) != 0;
	caps->dma_supported = (impcode & MIPS_EJTAG_CAP_NODMA) == 0;
	caps->mips16 = (impcode & MIPS_EJTAG_CAP_MIPS16) != 0;
	caps->eaddr_over_32 = (impcode & MIPS_EJTAG_CAP_EADDR_NO32BIT) != 0;

	if (impcode & MIPS_EJTAG_CAP_ASID8)
		caps->asid_size = 8;
	else if (impcode & MIPS_EJTAG_CAP_ASID6)
		caps->asid_size = 6;
	else
		caps->asid_size = 0;

	/* EJTAG v2.6 reuses some impcode bits for R3k/DINT reporting. */
	caps->type_r3k = (impcode & MIPS_EJTAG_CAP_R3K) != 0;
	caps->dint_supported = (impcode & MIPS_EJTAG_CAP_DINT) != 0;

	/* EJTAG v2.0 encodes break-unit presence as active-low NOxB bits and a
	 * break-channel count. Only meaningful when ejtag_version == 0. */
	caps->has_inst_break = (impcode & MIPS_EJTAG_CAP_NOIB) == 0;
	caps->has_data_break = (impcode & MIPS_EJTAG_CAP_NODB) == 0;
	caps->has_proc_break = (impcode & MIPS_EJTAG_CAP_NOPB) == 0;
	caps->break_channels =
		(impcode >> MIPS_EJTAG_CAP_BCHAN_SHIFT) & MIPS_EJTAG_CAP_BCHAN_MASK;

	caps->valid = true;
}
