#ifndef Fivefish_PGAChip_h
#define Fivefish_PGAChip_h

#include <stdint.h>

#define DB_VALUE_MAX 65
#define DB_VALUE_MIN 0

class Fivefish_PGAChannel
{
public:
  // constructor
  Fivefish_PGAChannel(uint8_t pga_cs, uint8_t pga_sdi, uint8_t pga_clk, uint8_t overload){
    _pga_cs = pga_cs; 
    _pga_sdi = pga_sdi;
    _pga_clk = pga_clk;
    _overload = overload;
  }

  int dBValue; // display dB value; Note: this is NOT the N gain value we'll be sending to the PGA chip

  // GPIO functions
  uint8_t pad;
  uint8_t p48v;
  uint8_t polarityReverse;
  uint8_t futureuse;

  void IncrementGain();
  void DecrementGain();
  void TogglePad();
  void EnablePad();
  void DisablePad();
  void ToggleP48V();
  void EnableP48V();
  void DisableP48V();
  void TogglePolarityReverse();
  void EnablePolarityReverse();
  void DisablePolarityReverse();
  void UpdatePGAChip();
  bool IsUpdateRequired();

private:
  // define pins
  uint8_t _pga_cs; 
  uint8_t _pga_sdi;
  uint8_t _pga_clk;
  uint8_t _overload; 

  bool _update_pgachip_required; // flag if we need to send new SPI data to the PGA chip

  uint8_t _PGAFlags; // this is the byte (8-bits) we'll send to the PGA2500 chip
                     // PGA Flags (from left to right), MSBFIRST
                     // 8th = DC Servo Enable, Active LOW 0
                     // 7th = CM Servo Enable, Active HIGH 1
                     // 6th = 0  set to ZERO always
                     // 5th = Overload Level Setting, default 0, will show clipping at 5.1Vrms  (+16.37 dBu)
                     //                               if 1, will show clipping at 4.0Vrms  (+14.26 dBu)
                     // 4th = GP04  use as ?????? future-use
                     // 3rd = GP03  use as OUTPUT Polarity Reverse
                     // 2ns = GP02  use as 48V Phantom Power
                     // 1st = GP01  use as -20dB INPUT Pad
                     //
                     // example: B01000000

  uint8_t _GainNValue; // this is the SPI Gain N data we need to send to PGA chip
  // GainNValue is the value we send to the PGAchip to control GAIN setting
  // For GainNValue = 0, dBValue= 0 dB
  // For GainNValue = 1 to 56, dBValue = 9 + N
  // For GainNValue = 57 to 63, dBValue = 66
};
#endif