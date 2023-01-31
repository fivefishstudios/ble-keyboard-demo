#include <stdint.h>
#include <Arduino.h>
#include <Fivefish_PGAChip.h>

void Fivefish_PGAChannel::IncrementGain()
{
  // Channel.dBValue is the value we display on the GLCD
  dBValue++;

  // we can't have dB values between 1dB and 9dB. We go from 0dB to 10dB, then 11dB, 12dB ...
  if (dBValue > DB_VALUE_MIN && dBValue < 10)
  {
    dBValue = 10;
  }

  // can't exceed max gain of chip
  if (dBValue > DB_VALUE_MAX)
  {
    dBValue = DB_VALUE_MAX;
  };

  _update_pgachip_required = true;
}

void Fivefish_PGAChannel::DecrementGain()
{
  // Channel.dBValue is the value we display on the GLCD
  dBValue--;

  // we can't have dB values between 1dB and 9dB. We go from 10dB to 0dB when decrementing
  if (dBValue < 10)
  {
    dBValue = DB_VALUE_MIN;
  }

  _update_pgachip_required = true;
}

void Fivefish_PGAChannel::TogglePad()
{
  pad = !pad;
  _update_pgachip_required = true;
}

void Fivefish_PGAChannel::EnablePad()
{
  pad = 1;
  _update_pgachip_required = true;
}

void Fivefish_PGAChannel::DisablePad()
{
  pad = 0;
  _update_pgachip_required = true;
}

void Fivefish_PGAChannel::ToggleP48V()
{
  p48v = !p48v;
  _update_pgachip_required = true;
}

void Fivefish_PGAChannel::EnableP48V()
{
  p48v = 1;
  _update_pgachip_required = true;
}

void Fivefish_PGAChannel::DisableP48V()
{
  p48v = 0;
  _update_pgachip_required = true;
}

void Fivefish_PGAChannel::TogglePolarityReverse()
{
  polarityReverse = !polarityReverse;
  _update_pgachip_required = true;
}

void Fivefish_PGAChannel::EnablePolarityReverse()
{
  polarityReverse = 1;
  _update_pgachip_required = true;
}

void Fivefish_PGAChannel::DisablePolarityReverse()
{
  polarityReverse = 0;
  _update_pgachip_required = true;
}

void Fivefish_PGAChannel::UpdatePGAChip()
{

  // do we need to update?
  if (!_update_pgachip_required)
  {
    return;
  }

  if (pad)
    _PGAFlags = uint8_t(_PGAFlags) | B00000001; // set 1st bit
  else
    _PGAFlags = uint8_t(_PGAFlags) & B11111110; // clear 1st bit

  if (p48v)
    _PGAFlags = uint8_t(_PGAFlags) | B00000010; // set 2nd bit
  else
    _PGAFlags = uint8_t(_PGAFlags) & B11111101; // clear 2nd bit

  if (polarityReverse)
    _PGAFlags = uint8_t(_PGAFlags) | B00000100; // set 3rd bit
  else
    _PGAFlags = uint8_t(_PGAFlags) & B11111011; // clear 3rd bit

  if (futureuse)
    _PGAFlags = uint8_t(_PGAFlags) | B00001000; // set 4th bit
  else
    _PGAFlags = uint8_t(_PGAFlags) & B11110111; // clear 4th bit

  // 5th = Overload Level Setting,
  // if 1, set clipping at 4.0Vrms  (+14.26 dBu)
  _PGAFlags = uint8_t(_PGAFlags) & B11101111; // clear 5th bit, set clipping at 5.1Vrms  (+16.37 dBu)

  // 6th bit - always ZERO
  _PGAFlags = uint8_t(_PGAFlags) & B11011111; // clear 6th bit, always 0

  // 7th = CM Servo Enable, Active HIGH 1
  _PGAFlags = uint8_t(_PGAFlags) | B01000000; // set 7th bit, CM Servo Enable

  // 8th = DC Servo Enable, Active LOW 0
  _PGAFlags = uint8_t(_PGAFlags) & B01111111; // clear 8th bit, 0=DC Servo Enable

  // Compute _GainNValue to send to PGA chip
  if (dBValue == 0)
  {
    _GainNValue = 0;
  };

  if (dBValue >= 10 && dBValue <= DB_VALUE_MAX)
  {
    _GainNValue = dBValue - 9;
  }

  if (dBValue > DB_VALUE_MAX)
  {
    _GainNValue = DB_VALUE_MAX - 9;
  }

  // Now, we use ShiftOut to send our serial datastream to the PGA chip
  digitalWrite(_pga_cs, LOW); // Pull LOW, Enable PGA chip to accept data stream input

  // MSB first
  shiftOut(_pga_sdi, _pga_clk, MSBFIRST, uint8_t(_PGAFlags));   // Send PGA flag settings
  shiftOut(_pga_sdi, _pga_clk, MSBFIRST, uint8_t(_GainNValue)); // followed by the "N" number for PGA Gain

  digitalWrite(_pga_cs, HIGH); // Pull HIGH. We'done!

  _update_pgachip_required = false; // reset flag
} // UpdatePGAChip

bool Fivefish_PGAChannel::IsUpdateRequired()
{
  return (_update_pgachip_required ? true : false);
}