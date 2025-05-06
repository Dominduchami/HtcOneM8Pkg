#include <Library/ArmLib.h>
#include <Library/IoLib.h>

#include <Library/FramebufferHelperLib.h>

#include "FramebufferHelperLibPrivate.h"

/* source: https://github.com/msm8916-mainline/lk2nd/blob/main/lk2nd/display/cont-splash/refresh.c#L115 */
static void mdp5_enable_auto_refresh()
{
 	UINT32 vsync_count = 19200000 / (FixedPcdGet32(PcdMipiFrameBufferHeight) * 60); /* 60 fps */
 	UINT32 mdss_mdp_rev = MmioRead32(MDP_HW_REV);
 	UINT32 pp0_base;
 
 	if (mdss_mdp_rev >= MDSS_MDP_HW_REV_105)
 		pp0_base = (0xFD900000 + 0x71000);
 	else if (mdss_mdp_rev >= MDSS_MDP_HW_REV_102)
 		pp0_base = (0xFD900000 + 0x12D00);
 	else
 		pp0_base = (0xFD900000 + 0x21B00);
 
 	MmioWrite32(pp0_base + MDP_PP_SYNC_CONFIG_VSYNC, vsync_count | BIT(19));
 	MmioWrite32(pp0_base + MDP_PP_AUTOREFRESH_CONFIG, BIT(31) | 1);

	// Kick refresh
	MmioWrite32(MDP_CTL_0_BASE + CTL_START, 1);
}

void DisplayAutorefresh(UINTN Enable)
{
	if(Enable) {
		mdp5_enable_auto_refresh();
	}
}