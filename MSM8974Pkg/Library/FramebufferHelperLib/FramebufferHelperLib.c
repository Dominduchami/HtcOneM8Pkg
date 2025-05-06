#include <Library/ArmLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>

#include <Library/FramebufferHelperLib.h>

#include "FramebufferHelperLibPrivate.h"

UINTN Width = FixedPcdGet32(PcdMipiFrameBufferWidth);
UINTN Height = FixedPcdGet32(PcdMipiFrameBufferHeight);
UINTN Bpp = FixedPcdGet32(PcdMipiFrameBufferPixelBpp);
UINTN FbAddr = FixedPcdGet32(PcdMipiFrameBufferAddress);

/* TODO: Move to the fb helper lib */
VOID
PaintScreen(
  IN  UINTN   BgColor
)
{
  // Code from FramebufferSerialPortLib
	char* Pixels = (void*)FixedPcdGet32(PcdMipiFrameBufferAddress);

	// Set color.
	for (UINTN i = 0; i < Width; i++)
	{
		for (UINTN j = 0; j < Height; j++)
		{
			// Set pixel bit
			for (UINTN p = 0; p < (Bpp / 8); p++)
			{
				*Pixels = (unsigned char)BgColor;
				BgColor = BgColor >> 8;
				Pixels++;
			}
		}
	}
}

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

VOID
DisplayAutorefresh(UINTN Enable)
{
	if(Enable) {
		mdp5_enable_auto_refresh();
	}
}

VOID
ReconfigFb()
{
  /* Change screen format to 32BPP BGRA for Windows (thanks sonic022gamer) */
  MmioWrite32(0xFD901E00 + 0x30, 0x000236FF);
  MmioWrite32(0xFD901E00 + 0x34, 0x03020001);
  MmioWrite32(0xFD901E00 + 0x24, 1080*4);
  MmioWrite32(0xFD900600 + 0x18, (1 << (3)));

  /* Move Framebuffer to the WP location */
  MmioWrite32(FB_ADDR_REG, FbAddr);
  /* Flush using CTL0_FLUSH and Flush VIG0 */
  MmioWrite32(0xfd900618, 0x00000001);
  MmioWrite32(0xfd900718, 0x00000001);
}

VOID
MdpRefresh()
{
#if DISPLAY_ENABLE_AUTOREFRESH == 0
  MmioWrite32(0xfd90061c, 1);
  MicroSecondDelay( 32000 );
#endif
}