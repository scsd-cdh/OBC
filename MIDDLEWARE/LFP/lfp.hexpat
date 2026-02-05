#pragma endian big

import hex.core;
import std.hash;
import std.io;
import std.mem;
import std.array;
import std.string;
import type.fmt;
import type.byte;
import type.base;
import type.magic;

using Section = std::mem::Section;
using Byte = type::Byte;

const bool check_crc = true;

bitfield LFPLogicalRouteId {
    unsigned initiator: 4;
    unsigned target: 4;
};

enum LFPDirection : u8 {
    Target,
    Initiator
};

bitfield LFPEndpoint {
    unsigned endpoint: 7;
    LFPDirection direction: 1;
};

struct LFPHeader {
    LFPLogicalRouteId logical_route_id [[inline]] ;
    LFPEndpoint endpoint [[inline]];
    u16 encoded_length;
    padding[2];

    str header_data = std::mem::read_string(addressof(this), $ - addressof(this));
    type::Hex<u16> header_crc16;

    if (check_crc) {
        u16 computed_header_crc16 = std::hash::crc16(header_data, 0xFFFF, 0x1021, 0, true, true) & 0xFEFE;
        if(computed_header_crc16 != header_crc16) {
            std::warning(std::format("Header checksum mismatch at 0x{:04X}: stored={:04X} computed={:04X}", $, header_crc16, computed_header_crc16));
        }
    }
};

fn decode_cobs(u64 length, Section section) {
    u64 p_dst = 0;
    u64 p_buf = $;

    while ($ < p_buf + length) {
        u8 block_length = std::mem::read_unsigned($, 1, std::mem::Endian::Big) - 1 - 0xFF;
        $ += 1;
        if ($ + block_length > p_buf + length) {
            std::warning("Invalid COBS block length");
            return p_dst;
        }
        std::mem::copy_value_to_section(std::mem::read_string($, block_length), section, p_dst);
        $ += block_length;
        p_dst += block_length;
        if (block_length != 0xFE && $ < p_buf + length) {
            std::mem::copy_value_to_section("\xFF", section, p_dst);
            p_dst += 1;
        }
    }

    return p_dst;
};

struct LFPMessage {
    type::Magic<"\xFF"> preamble;

    LFPHeader header;

    Section section = std::mem::create_section("LFP @ " + std::string::to_string(addressof(this))) [[expor]];
    u8 data_segment[header.encoded_length - 5];
    u8 data_crc32_fragments[5] [[hidden]];
    u32 data_crc32 = data_crc32_fragments[0] << 25 | data_crc32_fragments[1] << 18 | data_crc32_fragments[2] << 11 |data_crc32_fragments[3] << 4 | data_crc32_fragments[4] << 0;

    $ = addressof(data_segment);

    u64 decoded_size = decode_cobs(sizeof(data_segment), section);
    u8 data[decoded_size] @ 0 in section;
    str decoded_data = builtin::std::mem::read_string(addressof(data), decoded_size, section) [[export]];


    if (check_crc) {
        u32 computed_data_crc32 = std::hash::crc32(data, 0xFFFFFFFF, 0x9960034c, 0, true, true);
        if(computed_data_crc32 != data_crc32) {
            std::warning(std::format("Data checksum mismatch at 0x{:04X}: stored={:08X} computed={:08X}", $, data_crc32, computed_data_crc32));
        }
    }
};


LFPMessage request @ 0;

