#ifndef _CRC_H
#define _CRC_H

#include <array>
#include <type_traits>
#include <cstddef>
#include <ostream>

namespace csum
{
  typedef uint8_t byte1_t;

  //Type traits converting bit widths to corresponding types
  //Only available for 8, 16, 32, 64 bits
  template<size_t W>
    struct bit_width_trait{};
  
  template<>
    struct bit_width_trait<8>{ typedef uint8_t type; };

  template<>
    struct bit_width_trait<16>{ typedef uint16_t type; };
  
  template<>
    struct bit_width_trait<32>{ typedef uint32_t type; };

  template<>
    struct bit_width_trait<64>{ typedef uint64_t type; };
  

  //////////////
  template<size_t W>
    typename bit_width_trait<W>::type serialGenCRC(const byte1_t* buf, size_t len,
					  const typename bit_width_trait<W>::type& poly)
    {
      typedef typename bit_width_trait<W>::type T;
      T checksum = 0;
      size_t bits_to_msb = W - 8;
      T msb_mask = 1 << (W - 1);
      size_t bit_count = 0;
      while(len--)
	{
	  checksum ^= T(*buf++ << bits_to_msb);
	  for(bit_count = 0; bit_count < 8; ++bit_count)
	    checksum = (checksum & msb_mask) ? (checksum << 1) ^ poly : checksum << 1;
	}
      return checksum;
    }
    

  //Compute CRC with LUT
  template<size_t W>
    class CRC
    {
      typedef typename bit_width_trait<W>::type T;
      
    public:
      CRC(const T&);
      T gen(const byte1_t* buf, size_t len);
      friend std::ostream& operator<<(std::ostream& os, const CRC<W>& obj)
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
  template<size_t W>
    CRC<W>::CRC(const T& poly)
    {
      byte1_t lut_index = 0;
      do
	{
	  _crc_lut[lut_index] = serialGenCRC<W>(&lut_index, 1, poly);
	  ++lut_index;
	}while(lut_index != 0); //byte1_t will cycle back to 0
    }
  
  template<size_t W>
    typename CRC<W>::T CRC<W>::gen(const byte1_t* buf, size_t len)
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
