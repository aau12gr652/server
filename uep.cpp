#include <iostream>
#include <fstream>
#include <kodo/rlnc/full_vector_codes.h>
#include <postoffice/Postoffice.h>
#include <node/kodo_decoder.h>
#include <server/kodo_encoder.h>

int main()
{
	int Monte_Carlo = 1000;
	int gsize = 96;
	int Over_Head = 300;
	int symbol_size = 1;

    kodo_encoder foo=kodo_encoder();
    kodo_decoder poo=kodo_decoder();

	foo.set_symbol_size(symbol_size);

	std::ofstream statFileUEP;
	statFileUEP.open("uep.txt");
	statFileUEP << "%Generation, Packets, Layer 1, Layer 2, Layer 3" << std::endl;

	serial_data Packet;

	int layers = 3;
	foo.set_layers(layers);
	foo.set_generation_size(gsize);
	foo.set_layer_size(1, 32);
	foo.set_layer_size(2, 64);
	foo.set_layer_size(3, gsize);
	foo.set_layer_gamma(1, 50);
	foo.set_layer_gamma(2, 80);
	foo.set_layer_gamma(3, 100);
	char* RandomData = (char*)devRandom((gsize + 1) * symbol_size); // Generate random data

	for (int n = 0; n < Monte_Carlo; n++)
	{
		int Packet_Number;

		foo.new_generation(RandomData); // add random data too encoder

		for (Packet_Number = 0; Packet_Number < Over_Head * gsize; Packet_Number++)
		{
			Packet = foo.get_packet(); // Get a packet
			poo.decode(&foo.payload_stamp, Packet); // Decode the packet

			if (poo.is_layer_finish(1) == 1 || poo.is_layer_finish(2) == 1 || poo.is_layer_finish(3) == 1)
			{
				statFileUEP << n << ", " << Packet_Number+1;
				for (int L = 1; L <= layers; L++)
					 statFileUEP << ", " << poo.is_layer_finish(L);
				statFileUEP << std::endl;
			}
			if (poo.has_finished_decoding())
				break; // No need to continue, since the data is all decoded
		}
		//std::cout << "Required packets (UEP): " << Packet_Number + 1 << std::endl;
		if (n%500 == 0)
				std::cout << "Generation " << n << " did complete!" << std::endl;
	}
	free(RandomData); // Free the random data
	statFileUEP.close();
    return 0;
}
