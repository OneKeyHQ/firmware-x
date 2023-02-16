#ifndef ONEKEY_PROTOCOL_HPP
#define ONEKEY_PROTOCOL_HPP

#include <algorithm>
#include <cstdint>
#include <vector>

#include <messages.pb.h>

// Project Headers
#include "RunAndCheck.hpp"
#include "StdLog.hpp"

namespace onekey
{
    namespace protocol
    {
        static const size_t PROTOCOL_LEGACY_PACKET_SIZE = 64;
        static const size_t PROTOCOL_LEGACY_PACKET_OFFSET_HDR = 0;
        static const size_t PROTOCOL_LEGACY_PACKET_OFFSET_ID = 3;
        static const size_t PROTOCOL_LEGACY_PACKET_OFFSET_LEN = 5;
        static const size_t PROTOCOL_LEGACY_PACKET_OFFSET_DATA = 9;
        static const size_t PROTOCOL_LEGACY_PACKET_SIZE_DATA_FIRST = PROTOCOL_LEGACY_PACKET_SIZE - 3 - 2 - 4;
        static const size_t PROTOCOL_LEGACY_PACKET_SIZE_DATA_SUBSEQ = PROTOCOL_LEGACY_PACKET_SIZE - 1;

        static const std::vector<uint8_t> header_magic = {0x3f, 0x23, 0x23}; // ?##

        typedef std::vector<uint8_t> message_composed;

        typedef struct msg_struct
        {
            uint16_t id;
            uint32_t size;
            std::vector<uint8_t> protobuf_data;

            msg_struct()
            {
                reset();
            }
            void reset()
            {
                id = 0;
                size = 0;
                protobuf_data.resize(0);
            }

        } message_parsed;

        class parser
        {
        private:
            std::vector<message_composed> message_composed_pack;
            message_parsed parsed;

        public:
            parser()
            {
                reset();
            }

            inline void reset()
            {
                message_composed_pack.resize(0);
                parsed.reset();
            }

            inline std::vector<message_composed> get_composed()
            {
                return message_composed_pack;
            }

            inline message_parsed get_parsed()
            {
                return parsed;
            }

            bool parse_legacy(message_composed msg_raw)
            {
                bzh_utils::log::StdLog::Debug("msg_raw.size() -> ", std::to_string(msg_raw.size()));

                // handle as first packet
                if ( std::equal(msg_raw.begin(), msg_raw.begin() + 3, header_magic.begin()) )
                {
                    reset();

                    message_composed_pack.push_back(msg_raw);

                    parsed.id = *reinterpret_cast<uint16_t*>(&msg_raw.data()[3]);
                    parsed.size = *reinterpret_cast<uint32_t*>(&msg_raw.data()[5]);

                    if ( parsed.size != 0 )
                    {
                        parsed.protobuf_data.assign(
                            msg_raw.begin() + PROTOCOL_LEGACY_PACKET_OFFSET_DATA, msg_raw.end()
                        );
                    }

                    return true;
                }
                // handle as subsequential packet (and make sure parsed + new data won't overflow informed
                // total size)
                else
                if ( (msg_raw.at(0) == header_magic.at(0)) &&
                     ((msg_raw.size() - 1 + PROTOCOL_LEGACY_PACKET_SIZE_DATA_SUBSEQ) <= parsed.size) )
                {
                    message_composed_pack.push_back(msg_raw);

                    std::copy(msg_raw.begin() + 1, msg_raw.end(), std::back_inserter(parsed.protobuf_data));

                    return true;
                }
                // wrong header
                else
                {
                    return false;
                }
            }
        };

        class composer
        {
        private:
            std::vector<message_composed> message_composed_pack;
            message_parsed parsed;

        public:
            composer()
            {
                reset();
            }

            inline void reset()
            {
                message_composed_pack.resize(0);
                parsed.reset();
            }

            inline std::vector<message_composed> get_composed()
            {
                return message_composed_pack;
            }

            inline message_parsed get_parsed()
            {
                return parsed;
            }

            bool compose_legacy(message_parsed msg_parsed)
            {
                // check size
                if ( msg_parsed.protobuf_data.size() != msg_parsed.size )
                {
                    return false;
                }

                reset();

                parsed = msg_parsed;

                // first pack
                {
                    bzh_utils::log::AutoLog a("first pack");
                    message_composed msg_composed;

                    // "?##" header
                    msg_composed.insert(msg_composed.end(), header_magic.begin(), header_magic.end());

                    // message id
                    msg_composed.push_back(*(reinterpret_cast<uint8_t*>(&parsed.id) + 1)); // byte 2
                    msg_composed.push_back(*(reinterpret_cast<uint8_t*>(&parsed.id) + 0)); // byte 1

                    // message len
                    msg_composed.push_back(*(reinterpret_cast<uint8_t*>(&parsed.size) + 3)); // byte 4
                    msg_composed.push_back(*(reinterpret_cast<uint8_t*>(&parsed.size) + 2)); // byte 3
                    msg_composed.push_back(*(reinterpret_cast<uint8_t*>(&parsed.size) + 1)); // byte 2
                    msg_composed.push_back(*(reinterpret_cast<uint8_t*>(&parsed.size) + 0)); // byte 1

                    // protobuf_data
                    msg_composed.insert(
                        msg_composed.end(), parsed.protobuf_data.begin(),
                        parsed.protobuf_data.begin() + PROTOCOL_LEGACY_PACKET_SIZE_DATA_FIRST
                    );

                    if ( msg_composed.size() != PROTOCOL_LEGACY_PACKET_SIZE )
                        return false;

                    message_composed_pack.push_back(msg_composed);
                }
                // subsequential pack
                {
                    while ( true )
                    {
                        bzh_utils::log::AutoLog a("subsequential pack");

                        // calculate processed bytes
                        size_t protobuf_data_bytes_composed =
                            PROTOCOL_LEGACY_PACKET_SIZE_DATA_FIRST +
                            (message_composed_pack.size() - 1) * PROTOCOL_LEGACY_PACKET_SIZE_DATA_SUBSEQ;
                        bzh_utils::log::StdLog::Debug("calculate processed bytes");

                        // break if all bytes processed
                        if ( protobuf_data_bytes_composed >= parsed.protobuf_data.size() )
                            break;
                        bzh_utils::log::StdLog::Debug("break if all bytes processed");

                        size_t protobuf_data_bytes_to_be_composed =
                            (parsed.protobuf_data.size() - protobuf_data_bytes_composed);
                        if ( protobuf_data_bytes_to_be_composed >= PROTOCOL_LEGACY_PACKET_SIZE_DATA_SUBSEQ )
                            protobuf_data_bytes_to_be_composed = PROTOCOL_LEGACY_PACKET_SIZE_DATA_SUBSEQ;
                        bzh_utils::log::StdLog::Debug(
                            "protobuf_data_bytes_composed -> ", std::to_string(protobuf_data_bytes_composed)
                        );
                        bzh_utils::log::StdLog::Debug(
                            "protobuf_data_bytes_to_be_composed -> ",
                            std::to_string(protobuf_data_bytes_to_be_composed)
                        );

                        message_composed msg_composed;

                        // "?" header
                        msg_composed.push_back(header_magic.at(0));
                        bzh_utils::log::StdLog::Debug("header");

                        // protobuf_data
                        msg_composed.insert(
                            msg_composed.end(), parsed.protobuf_data.begin() + protobuf_data_bytes_composed,
                            parsed.protobuf_data.begin() + protobuf_data_bytes_composed +
                                protobuf_data_bytes_to_be_composed
                        );
                        bzh_utils::log::StdLog::Debug("protobuf_data");

                        // fill 0x00 if last pack not full
                        if ( protobuf_data_bytes_to_be_composed < PROTOCOL_LEGACY_PACKET_SIZE_DATA_SUBSEQ )
                        {
                            msg_composed.insert(
                                msg_composed.end(),
                                PROTOCOL_LEGACY_PACKET_SIZE_DATA_SUBSEQ - protobuf_data_bytes_to_be_composed,
                                0x00
                            );
                            bzh_utils::log::StdLog::Debug("fill 0x00 if last pack not full");
                        }

                        if ( msg_composed.size() != PROTOCOL_LEGACY_PACKET_SIZE )
                            return false;
                        bzh_utils::log::StdLog::Debug("check size");

                        message_composed_pack.push_back(msg_composed);
                        bzh_utils::log::StdLog::Debug("push_back");
                    }
                }

                return true;
            }
        };

    } // namespace protocol

} // namespace onekey

#endif // ONEKEY_PROTOCOL_HPP
