#ifndef _pin_magic_
#define _pin_magic_

// a 48 Mhz requiere 19.2 ciclos = 400ns
#define  DELAY7   delay_cycles(20)


  // LCD control lines:
  // RD (read), WR (write), CD (command/data), CS (chip select)


  #define RD_ACTIVE  output_low(RD_PIN)
  #define RD_IDLE    output_high(RD_PIN)
  #define WR_ACTIVE  output_low(WR_PIN)
  #define WR_IDLE    output_high(WR_PIN)
  #define CD_COMMAND output_low(CD_PIN)
  #define CD_DATA    output_high(CD_PIN)
  #define CS_ACTIVE  output_low(CS_PIN)
  #define CS_IDLE    output_high(CS_PIN)
  // These are macros for I/O operations...
// Data write strobe, ~2 instructions and always inline
  #define WR_STROBE { WR_ACTIVE; WR_IDLE; }
  // Write 8-bit value to LCD data lines
  #define write8inline(d) {                          \
    output_d(d); \
    WR_STROBE; } // STROBEs are defined later

  // Read 8-bit value from LCD data lines.  The signle argument
  // is a destination variable; this isn't a function and doesn't
  // return a value in the conventional sense.
  #define read8inline(result) {                       \
    RD_ACTIVE;                                        \
    DELAY7;                                           \
    result = input_d();                               \
    RD_IDLE; }

  // These set the PORT directions as required before the write and read
  // operations.  Because write operations are much more common than reads,
  // the data-reading functions in the library code set the PORT(s) to
  // input before a read, and restore them back to the write state before
  // returning.  This avoids having to set it for output inside every
  // drawing method.  The default state has them initialized for writes.
  #define setWriteDirInline() { set_tris_d(0);}
  #define setReadDirInline()  { set_tris_d(0xFF);}


// These higher-level operations are usually functionalized,
// except on Mega where's there's gobs and gobs of program space.

// Set value of TFT register: 8-bit address, 8-bit value
#define writeRegister8inline(a, d) { \
  CD_COMMAND; write8(a); CD_DATA; write8(d); }

// Set value of TFT register: 16-bit address, 16-bit value
// See notes at top about macro expansion, hence hi & lo temp vars
#define writeRegister16inline(a, d) { \
  unsigned char hi, lo; \
  hi = (a) >> 8; lo = (a); CD_COMMAND; write8(hi); write8(lo); \
  hi = (d) >> 8; lo = (d); CD_DATA   ; write8(hi); write8(lo); }

// Set value of 2 TFT registers: Two 8-bit addresses (hi & lo), 16-bit value
#define writeRegisterPairInline(aH, aL, d) { \
  unsigned char hi = (d) >> 8, lo = (d); \
  CD_COMMAND; write8(aH); CD_DATA; write8(hi); \
  CD_COMMAND; write8(aL); CD_DATA; write8(lo); }
  
  #define write8            write8inline
  #define read8             read8inline
  #define setWriteDir       setWriteDirInline
  #define setReadDir        setReadDirInline
  #define writeRegister8    writeRegister8inline
  #define writeRegister16   writeRegister16inline
  #define writeRegisterPair writeRegisterPairInline
#endif // _pin_magic_
