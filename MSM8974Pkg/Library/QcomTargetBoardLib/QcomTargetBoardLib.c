#include <Base.h>

#include <Library/LKEnvLib.h>

#include <Chipset/baseband.h>
#include <Chipset/board.h>
#include <Chipset/smem.h>

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

  platform = board->platform;

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
    dprintf(CRITICAL, "Platform type: %u is not supported\n", platform);
    ASSERT(0);
  };
}
