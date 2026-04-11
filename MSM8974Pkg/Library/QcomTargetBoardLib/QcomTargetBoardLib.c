#include <Base.h>

#include <Library/LKEnvLib.h>

#include <Chipset/baseband.h>
#include <Chipset/smem.h>
#include <Chipset/board.h>

#include <Library/QcomTargetBoardLib.h>

/* Detect the target type */
void target_detect(struct board_data *board)
{
  /* This is filled from board.c */
}

/* Detect the modem type */
void target_baseband_detect(struct board_data *board)
{
  uint32_t platform;
  uint32_t platform_subtype;

  platform = board->platform;
  platform_subtype = board->platform_subtype;

  /*
	 * Look for platform subtype if present, else
	 * check for platform type to decide on the
	 * baseband type
	 */
	switch(platform_subtype) {
	case HW_PLATFORM_SUBTYPE_UNKNOWN:
	case HW_PLATFORM_SUBTYPE_8974PRO_PM8084:
	case HW_PLATFORM_8994_INTERPOSER:
		break;
	default:
		dprintf(CRITICAL, "Platform Subtype : %u is not supported\n",platform_subtype);
		ASSERT(0);
	};

	switch(platform) {
	case MSM8974:
	case MSM8274:
	case MSM8674:
	case MSM8274AA:
	case MSM8274AB:
	case MSM8274AC:
	case MSM8674AA:
	case MSM8674AB:
	case MSM8674AC:
	case MSM8974AA:
	case MSM8974AB:
	case MSM8974AC:
		board->baseband = BASEBAND_MSM;
		break;
	case APQ8074:
	case APQ8074AA:
	case APQ8074AB:
	case APQ8074AC:
		board->baseband = BASEBAND_APQ;
		break;
	default:
		dprintf(CRITICAL, "Platform type: %u is not supported\n",platform);
		ASSERT(0);
	};
}
