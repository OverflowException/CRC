#ifndef _CRC_H
#define _CRC_H

#include <array>
#include <type_traits>
#include <cstddef>
#include <ostream>

namespace csum
{
  typedef uint8_t byte1_t;
  
  template<typename T>
    T serialGenCRC(const byte1_t* buf, size_t len, const T& poly)
    {
      T checksum = 0; //current checksum
      size_t poly_bits = sizeof(T) * 8; //bit width of polynomial
      size_t bits_to_msb = poly_bits - 8; //a byte need to shift these bits to get to the msb of type T
      T msb_mask = 1 << (poly_bits - 1); //type T most significant bit mask
      size_t bit_count = 0; //bit counter
      while(len--)
	{
	  checksum ^= T(*buf++ << bits_to_msb);
	  for(bit_count = 0; bit_count < 8; ++bit_count)
	    checksum = (checksum & msb_mask) ? (checksum << 1) ^ poly : checksum << 1;
	}
      return checksum;
    }

  template<typename T>
    class CRC
    {
    public:
      CRC(const T&);
      T gen(const byte1_t* buf, size_t len);
      friend std::ostream& operator<<(std::ostream& os, const CRC<T>& obj)
      {
	//Output hex, with leading zeroes would be best.
	for(const auto& ele : obj._crc_lut)
	  os << ele << "\t";
	os << std::flush;
      }
	
    private:
      std::array<T, 256> _crc_lut;
    };
  
  //template class methods' definitions
  template<typename T>
    CRC<T>::CRC(const T& poly)
    {
      byte1_t lut_index = 0;
      do
	{
	  _crc_lut[lut_index] = serialGenCRC<T>(&lut_index, 1, poly);
	  ++lut_index;
	}while(lut_index != 0); //byte1_t will cycle back to 0
    }
  
  template<typename T>
    T CRC<T>::gen(const byte1_t* buf, size_t len)
    {
      T checksum = 0;
      size_t lut_pos = 0;
      size_t poly_bits = sizeof(T) * 8;
      size_t bits_to_lsb = poly_bits - 8;
      
      while(len--)
	{
	  lut_pos = (checksum >> bits_to_lsb) ^ *buf++;
	  checksum = (checksum << 8) ^ _crc_lut[lut_pos];
	}
      
      return checksum;
    }

}


#endif
