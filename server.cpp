#include <kodo/rlnc/full_vector_codes.h>

#include <postoffice/Postoffice.h>
#include "kodo_encoder.h"
#include <fstream>
int main(int argc, char *argv[])
{
    kodo_encoder foo=kodo_encoder();
    //postoffice po("4000", "255.255.255.255");
    postoffice po("4000", "169.254.134.249");
	char* PI="3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609";
    if (argc == 1)
	{
		char* msg[3];
		msg[0] = "ABCDE, world!1";
		msg[1] = "FGHIJ, world!2";
		msg[2] = "KLMNO, world!3";
		foo.set_layers(2);
		foo.set_generation_size(14);
		foo.set_layer_size(1, 5);
		foo.set_layer_size(2, 14);
		foo.set_layer_gamma(1, 50);
		foo.set_layer_gamma(2, 100);
		foo.set_symbol_size(1);

		for (int u = 0; u < 3; u++)
		{
			foo.new_generation(msg[u]);
	//		for (int n = 0; n < 20; n++)
	//			foo.get_packet(2);
			for (int n = 0; n < 9; n++)
			{
				foo.send_packet(po, 2);
				std::cout << "Send packet: " << n  << " from layer: " << foo.payload_stamp.Layer_ID*1 << " Generation: " << foo.payload_stamp.Generation_ID*1 << std::endl;
			}
			for (int n = 0; n < 5; n++)
			{
	//			serial_data packet = foo.get_packet(1);
	//			po.send(packet, &foo.payload_stamp);
				foo.send_packet(po, 1);
				std::cout << "Send packet: " << n  << " from layer: " << foo.payload_stamp.Layer_ID*1 << " Generation: " << foo.payload_stamp.Generation_ID*1 << std::endl;
			}
		}
	}
	else
	{
		std::cout << "Jeg sender lige mega meget data" << std::endl;
		foo.set_layers(2);
		foo.set_generation_size(1400);
		foo.set_layer_size(1, 450);
		foo.set_layer_size(2, 1400);
		foo.set_layer_gamma(1, 50);
		foo.set_layer_gamma(2, 100);
		foo.set_symbol_size(1282);

		void* data = malloc(1400*1400);
		memset(data,0,1400*1400);
		memcpy(data,(void*)PI,400);
		foo.new_generation((char*)data);
		foo.send_packet(po,2);
		for (int u = 0; u < 1; u++)
		{
			foo.new_generation((char*)data);
			for (int n = 0; n < 2000; n++)
			{
				foo.send_packet(po,2);
				//print_stamp(&foo.payload_stamp);
				std::cout << "Packet: " << n+1 << std::endl;
			}
			std::cout << std::endl;
		}
		free(data);
	}

    po.closeConnection();
    return 0;
}
