//Encoder.hpp

#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <string>
#include <unistd.h>
#include <cstddef>
#include <cstring>

#include "schifra/schifra_galois_field.hpp"
#include "schifra/schifra_galois_field_polynomial.hpp"
#include "schifra/schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra/schifra_reed_solomon_encoder.hpp"
#include "schifra/schifra_reed_solomon_decoder.hpp"
#include "schifra/schifra_reed_solomon_block.hpp"
#include "schifra/schifra_error_processes.hpp"

#include "utils.hpp"

/***************** Base Class Encoder *****************/


class Encoder {
    
public:
    Encoder(){}
    
    virtual bool encode(const byte *input, byte *output) = 0;
    
    virtual bool decode(const byte *input, byte *output) = 0;
    
    virtual ~Encoder(){}
};

/***************** Derived Class EncoderRS *****************/

template <std::size_t code_length, std::size_t fec_length, std::size_t data_length = code_length - fec_length>
class EncoderRS: public Encoder {
public:
    inline EncoderRS() {
        field = new schifra::galois::field(field_descriptor,
                                           schifra::galois::primitive_polynomial_size06,
                                           schifra::galois::primitive_polynomial06);
        
        generator_polynomial = new schifra::galois::field_polynomial(*field);
        
        if (
            !schifra::make_sequential_root_generator_polynomial(*field,
                                                                generator_polynomial_index,
                                                                generator_polynomial_root_count,
                                                                *generator_polynomial)
        )
        {
            COUT<< "Error - Failed to create sequential root generator!\n";
        }
        
        encoder = new encoder_t(*field, *generator_polynomial);
        decoder = new decoder_t(*field, generator_polynomial_index);
        
        corrected.resize(data_length, 0x00);

        COUT << "EncoderRS created\n";
    }
    
    inline bool encode(const byte *input, byte *output) {
        spacket.assign((const char *)input, code_length);
        encoder->encode(spacket, block);

        for (int k=0; k < code_length; k++) {
            output[k] = block[k];
        }

        return 0;
    }
    
    inline bool decode(const byte *input, byte *output) {
        int error = 0;
        
        spacket.assign((const char *)input, code_length);
        data = spacket.substr(0, data_length);
        fec = spacket.substr(data_length, fec_length);
        
        block = schifra::reed_solomon::block<code_length,fec_length>(data, fec);
        
        if(!decoder->decode(block)) {
            COUT<< "Error - Critical decoding failure! "
            << "Msg: " << block.error_as_string() << "\n";
            error = 1;
            
        } else{
            block.data_to_string(corrected);
            memcpy(output, (byte *) corrected.c_str(), code_length);
        }
        
        return error;
    }
    
    inline ~EncoderRS() {
        COUT<< "EncodeRS destroyed...\n";
    }
    
private:
    /* Finite Field Parameters */
    const std::size_t field_descriptor = 8;
    const std::size_t generator_polynomial_index = 120;
    const std::size_t generator_polynomial_root_count = fec_length;
    
    /* Instantiate Finite Field and Generator Polynomials */
    schifra::galois::field *field;
    schifra::galois::field_polynomial *generator_polynomial;
    
    /* Instantiate Encoder and Decoder (Codec) */
    typedef schifra::reed_solomon::shortened_encoder<code_length, fec_length, data_length> encoder_t;
    typedef schifra::reed_solomon::shortened_decoder<code_length, fec_length, data_length> decoder_t;
    
    encoder_t *encoder;
    decoder_t *decoder;
    
    /* Instantiate RS Block For Codec */
    schifra::reed_solomon::block<code_length, fec_length, data_length> block;
    
    std::string spacket;
    std::string data;
    std::string fec;
    std::string corrected;
};

#endif /* ENCODER_H_ */
