#ifndef _CRC_H
#define _CRC_H

#include <array>
#include <type_traits>
#include <cstddef>
#include <ostream>
#include <algorithm>


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

      template<typename ForwardIter>
	T gen(ForwardIter beg, ForwardIter end)
	{
	  //Generalize. Check value_type.
	  //An iterator trait is used to obtain iterator type info
	  typedef typename std::iterator_traits<ForwardIter>::value_type val_t;
	  return _gen(beg, end, val_t());
	}
      
      friend std::ostream& operator<<(std::ostream& os, const CRC<W>& obj)
      {
	//Output hex, with leading zeroes would be best.
	for(const auto& ele : obj._crc_lut)
	  os << ele << "\t";
	os << std::flush;
      }
	
    private:
      std::array<T, 256> _crc_lut;

      //If value_type is any data type that is not 1 byte in size, return 0;
      template<typename ForwardIter, typename T>
	T _gen(ForwardIter beg, ForwardIter end, T){ return 0; };

      //If value_type is 1 byte in size, calculate.
      //This is not a partial specification. Simple overide.
      template<typename ForwardIter>
	T _gen(ForwardIter beg, ForwardIter end, uint8_t);
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

  //Notice how to write the definition of a template member function of a template class.
  template<size_t W>
  template<typename ForwardIter>
    typename CRC<W>::T CRC<W>::_gen(ForwardIter beg, ForwardIter end, uint8_t)
    {
      T checksum = 0;
      size_t lut_pos = 0;
      size_t poly_bits = sizeof(T) * 8;
      size_t bits_to_lsb = poly_bits - 8;

      for(; beg != end; ++beg)
  	{
  	  lut_pos = (checksum >> bits_to_lsb) ^ *beg;
  	  checksum = (checksum << 8) ^ _crc_lut[lut_pos];
  	}
      
      return checksum;
    }

}

#endif
