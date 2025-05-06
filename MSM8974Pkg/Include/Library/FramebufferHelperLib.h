#define ENABLE 1
#define DISABLE 0

#define FB_ADDR_REG             0xFD901E14

VOID ReconfigFb();
VOID DisplayAutorefresh(UINTN Enable);
VOID PaintScreen(IN  UINTN   BgColor );
VOID MdpRefresh();