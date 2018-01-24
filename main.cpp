#include <iostream>
#include <cstdint>
#include "crc.h"

using namespace std;

#define POLY_DVBS2   0xd5
#define POLY_BUYPASS 0x8005
#define POLY_32Q     0x814141ab

int main(int argc, char** argv)
{
  unsigned char test[] = {0x11, 0xc4, 0xcc, 0xfc};

  cout << hex;
  //Convert the result to size_t, so that uint8_t type will show.
  cout << size_t(csum::serialGenCRC<32>(test, sizeof(test), POLY_32Q)) << endl;

  cout << "LUT CRC:" << endl;
  csum::CRC<32> c(POLY_32Q);
  cout << size_t(c.gen(test, sizeof(test))) << endl;
}
