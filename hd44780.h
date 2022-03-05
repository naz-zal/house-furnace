   /*
    2  * ----------------------------------------------------------------------------
    3  * "THE BEER-WARE LICENSE" (Revision 42):
    4  * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
    5  * can do whatever you want with this stuff. If we meet some day, and you think
    6  * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
    7  * ----------------------------------------------------------------------------
    8  *
    9  * HD44780 LCD display driver
   10  *
   11  * $Id: hd44780_8h_source.html,v 1.1.1.6 2016/02/09 07:13:38 joerg_wunsch Exp $
   12  */

   /*
   15  * Send byte b to the LCD.  rs is the RS signal (register select), 0
   16  * selects instruction register, 1 selects the data register.
   17  */
void    hd44780_outbyte(uint8_t b, uint8_t rs);

/*
   21  * Read one byte from the LCD controller.  rs is the RS signal, 0
   22  * selects busy flag (bit 7) and address counter, 1 selects the data
   23  * register.
   24  */
uint8_t hd44780_inbyte(uint8_t rs);

 /*
   28  * Wait for the busy flag to clear.
   29  */
void    hd44780_wait_ready(bool islong);

/*
   33  * Initialize the LCD controller hardware.
   34  */
void    hd44780_init(void);
 
/*
   38  * Prepare the LCD controller pins for powerdown.
   39  */
void    hd44780_powerdown(void);

/* Send a command to the LCD controller. */
#define hd44780_outcmd(n)   hd44780_outbyte((n), 0)

/* Send a data byte to the LCD controller. */
#define hd44780_outdata(n)  hd44780_outbyte((n), 1)

/* Read the address counter and busy flag from the LCD. */
#define hd44780_incmd()     hd44780_inbyte(0)

/* Read the current data byte from the LCD. */
#define hd44780_indata()    hd44780_inbyte(1)


/* Clear LCD display command. */
#define HD44780_CLR \
     0x01

/* Home cursor command. */
#define HD44780_HOME \
     0x02
/*
   65  * Select the entry mode.  inc determines whether the address counter
   66  * auto-increments, shift selects an automatic display shift.
*/
#define HD44780_ENTMODE(inc, shift) \
     (0x04 | ((inc)? 0x02: 0) | ((shift)? 1: 0))
 
 /*
   72  * Selects disp[lay] on/off, cursor on/off, cursor blink[ing]
   73  * on/off.
   74  */
 #define HD44780_DISPCTL(disp, cursor, blink) \
     (0x08 | ((disp)? 0x04: 0) | ((cursor)? 0x02: 0) | ((blink)? 1: 0))
 
 /*
   79  * With shift = 1, shift display right or left.
   80  * With shift = 0, move cursor right or left.
   81  */
#define HD44780_SHIFT(shift, right) \
     (0x10 | ((shift)? 0x08: 0) | ((right)? 0x04: 0))

 /*
   86  * Function set.  if8bit selects an 8-bit data path, twoline arranges
   87  * for a two-line display, font5x10 selects the 5x10 dot font (5x8
   88  * dots if clear).
   89  */
 #define HD44780_FNSET(if8bit, twoline, font5x10) \
     (0x20 | ((if8bit)? 0x10: 0) | ((twoline)? 0x08: 0) | \
         ((font5x10)? 0x04: 0))
 
 /*
   95  * Set the next character generator address to addr.
   96  */
 #define HD44780_CGADDR(addr) \
     (0x40 | ((addr) & 0x3f))
 
 /*
  101  * Set the next display address to addr.
  102  */
#define HD44780_DDADDR(addr) \
     (0x80 | ((addr) & 0x7f))
 