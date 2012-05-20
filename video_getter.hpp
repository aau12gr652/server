#include <hollywood_source.hpp>
#include <serializer/serializer.hpp>
#include <cstdlib>
#include <cstring>
#include <boost/bind.hpp>


class video_getter {
public:
	uint8_t* data_ptr;
	bool buffer_ready;
	uint32_t buffer_size;

	std::vector<uint32_t> serialized_buffer_table,serialized_buffer_table_out;

	hollywood_source *hwood_src;
	serializer *slizer;

	video_getter(const char* path_to_file);
	~video_getter();

	void prepare_avpacket_for_encoder(AVPacket* pkt);
	void serialize_avpacket(AVPacket* pkt);
	uint32_t get_gop(uint8_t** ptr);

//	ret get () // THIS IS MAIN FUNCTIONALITY. RETURNS SERIALIZED BUFFER OF AVPACKET WITH INDICATION OF WHERE IDATA ENDS.

};
