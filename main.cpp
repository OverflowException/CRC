#include <iostream>
#include <cstdint>
#include "crc.h"

using namespace std;


int main(int argc, char** argv)
{
  unsigned char test[] = {0x11, 0xc4, 0xcc, 0xac, 0x65, 0xee, 0x45};

  cout << hex;
  cout << csum::serialGenCRC<uint32_t>(test, sizeof(test), 0x814141ab) << endl;

  cout << "LUT CRC:" << endl;
  csum::CRC<uint32_t> c(0x814141ab);
  cout << c.gen(test, sizeof(test)) << endl;
}
