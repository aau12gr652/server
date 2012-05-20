#include <iostream>
#include <fstream>
#include <kodo/rlnc/full_vector_codes.h>
#include <postoffice/Postoffice.h>
#include <node/kodo_decoder.h>
#include <server/kodo_encoder.h>

int main()
{
	int Monte_Carlo = 1000000;
	int gsize = 96;
	int Over_Head = 300;
	int symbol_size = 1;

    kodo_encoder foo=kodo_encoder();
    kodo_decoder poo=kodo_decoder();

	foo.set_symbol_size(symbol_size);

	std::ofstream statFileEEP;
	statFileEEP.open("eep.txt");
	statFileEEP << "%Generation, Packets, Layer 1" << std::endl;

	serial_data Packet;

	foo.set_layers(1);
	foo.set_generation_size(gsize);
	foo.set_layer_size(1, gsize);
	foo.set_layer_gamma(1, 100);
	char* RandomData = (char*)devRandom((gsize + 1) * symbol_size); // Generate random data
	foo.new_generation(RandomData); // add random data too encoder

	for (int n = 0; n < Monte_Carlo; n++)
	{
		int Packet_Number;

		for (Packet_Number = 0; Packet_Number < Over_Head * gsize; Packet_Number++)
		{
			Packet = foo.get_packet(1); // Get a packet
			poo.decode(&foo.payload_stamp, Packet); // Decode the packet

			if (poo.has_finished_decoding())
			{
				statFileEEP << n << ", " << Packet_Number+1 << ", " << 1 << std::endl;
				foo.payload_stamp.Generation_ID++;
				break; // No need to continue, since the data is all decoded
			}
		}
		if (n%10000 == 0)
			std::cout << "Generation: " << n << " required packets (EEP): " << Packet_Number + 1<< std::endl;
	}
	free(RandomData); // Free the random data
	statFileEEP.close();
    return 0;
}
