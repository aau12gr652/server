#include <kodo/rlnc/full_vector_codes.h>

#include <postoffice/Postoffice.h>
#include "kodo_encoder.h"
#include <node/kodo_decoder.h>
#include <iostream>
#include <fstream>

int main()
{
	int Monte_Carlo = 10;
	int gsize = 100;
	int Over_Head = 30;
	int symbol_size = 1000;

    kodo_encoder foo=kodo_encoder();
    kodo_decoder poo=kodo_decoder();

	foo.set_symbol_size(symbol_size);

	std::ofstream statFileUEP;
	statFileUEP.open("uep.txt");
	statFileUEP << "%Generation, Packets, Layer 1, Layer 2, Layer 3" << std::endl;

	std::ofstream statFileEEP;
	statFileEEP.open("eep.txt");
	statFileEEP << "%Generation, Packets, Layer 1" << std::endl;

	serial_data Packet;
    std::vector<uint8_t> data_out;
	int error = 0, error1 = 0;
	for (int n = 0; n < Monte_Carlo; n++)
	{
		int Packet_Number;
		char* RandomData = (char*)devRandom((gsize + 1) * symbol_size); // Generate random data

		int layers = 2;
		foo.set_layers(layers);
		foo.set_generation_size(gsize);
		foo.set_layer_size(1, 32);
		foo.set_layer_size(2, gsize);
		foo.set_layer_gamma(1, 75);
		foo.set_layer_gamma(2, 100);
		foo.new_generation(RandomData); // add random data too encoder

		for (Packet_Number = 0; Packet_Number < Over_Head * gsize; Packet_Number++)
		{
			Packet = foo.get_packet(); // Get a packet
			data_out = poo.decode(&foo.payload_stamp, Packet); // Decode the packet

			statFileUEP << n << ", " << Packet_Number+1;
			for (int L = 1; L <= layers; L++)
				 statFileUEP << ", " << poo.is_layer_finish(L);
			statFileUEP << std::endl;
			if (poo.has_finished_decoding())
				break; // No need to continue, since the data is all decoded
		}
		std::cout << "Required packets (UEP): " << Packet_Number + 1 << std::endl;

		foo.set_layers(1);
		foo.set_generation_size(gsize);
		foo.set_layer_size(1, gsize);
		foo.set_layer_gamma(1, 100);
		foo.new_generation(RandomData); // add random data too encoder
		for (Packet_Number = 0; Packet_Number < Over_Head * gsize; Packet_Number++)
		{
			Packet = foo.get_packet(); // Get a packet
			data_out = poo.decode(&foo.payload_stamp, Packet); // Decode the packet

			if (poo.has_finished_decoding())
			{
				statFileEEP << n << ", " << Packet_Number+1 << ", " << 1 << std::endl;
				break; // No need to continue, since the data is all decoded
				if (Packet_Number == 102)
				std::cout << "HEY HEY HEY HEY HEY HEY HEY HEY " << Packet_Number+1 << std::endl;
			}
		}
		std::cout << "Required packets (EEP): " << Packet_Number + 1<< std::endl;

		free(RandomData); // Free the random data
	}
	statFileUEP.close();
	statFileEEP.close();
    return 0;
}
