#include <iostream>
#include <fstream>
#include <kodo/rlnc/full_vector_codes.h>
#include <postoffice/Postoffice.h>
#include <node/kodo_decoder.h>
#include <server/kodo_encoder.h>

int main()
{
	int Monte_Carlo = 100000;
	int gsize = 96;
	int Over_Head = 300;
	int symbol_size = 1;

    kodo_encoder foo=kodo_encoder();
    kodo_decoder poo=kodo_decoder();

	foo.set_symbol_size(symbol_size);
	serial_data Packet;
	char file_name[10];
	char file_name2[20];
	char* test_name = "uepRandom";
	char* test_name2 = "uepRandomLayer";
	char* file_type = "txt";

	int layers = 2;
	foo.set_layers(layers);
	foo.set_generation_size(gsize);
	foo.set_layer_size(1, 32);
	foo.set_layer_size(2, gsize);
	foo.set_layer_gamma(2, 100);
	int Gamma1[3]={30, 40 ,50};

	char* RandomData = (char*)devRandom((gsize + 1) * symbol_size); // Generate random data

	for (int u = 0; u < 3; u++)
	{
		std::ofstream statFileUEP;
		std::ofstream LayerFile;
		sprintf(file_name, "%s%d.%s",test_name,Gamma1[u],file_type);
//		std::cout << "F1:" << file_name << std::endl;
		statFileUEP.open(file_name);
		statFileUEP << "%Generation, Packets, Layer 1, Layer 2" << std::endl;
		sprintf(file_name2, "%s%d.%s",test_name2,Gamma1[u],file_type);
		std::cout << "F2:" << file_name << std::endl;
		LayerFile.open(file_name2);
		LayerFile << "%Generation, Layer" << std::endl;
		foo.set_layer_gamma(1, Gamma1[u]);
		for (int n = 0; n < Monte_Carlo; n++)
		{
			int Packet_Number;

			foo.new_generation(RandomData); // add random data too encoder

			for (Packet_Number = 0; Packet_Number < Over_Head * gsize; Packet_Number++)
			{
				Packet = foo.get_packet(); // Get a packet
				poo.decode(&foo.payload_stamp, Packet); // Decode the packet

				LayerFile << n << ", " << foo.payload_stamp.Layer_ID*1 << std::endl; // Report the layer

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
			if (n%5000 == 0)
					std::cout << "Generation " << n << " did complete, with gamma1 = " << Gamma1[u]*1 << std::endl;
		}
		statFileUEP.close();
		LayerFile.close();
	}
	free(RandomData); // Free the random data
    return 0;
}
