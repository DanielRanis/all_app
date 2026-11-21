#include <iostream>
#include "ByteParser.h"

int main() {
    // Example buffer
    std::byte buffer[] = {
        std::byte{0}, std::byte{12}, std::byte{2}, std::byte{0}, std::byte{0}, std::byte{0},
        std::byte{0x01}, std::byte{0x48}, std::byte{0x65}, std::byte{0x6c}, std::byte{0x6c}, std::byte{0x6f}
    };
    using ParseData = ByteParser::ParseData;
    using ParseError = ByteParser::ParseError;
    if (const auto result = ByteParser::parse({buffer, sizeof(buffer)}); std::holds_alternative<ParseError>(result)) {
        if (const auto error = std::get<ParseError>(result); error == ParseError::InvalidLength) {
            std::cout << "Error: Invalid length" << std::endl;
        } else if (error == ParseError::UnsupportedMessageType) {
            std::cout << "Error: Unsupported message type" << std::endl;
        } else if (error == ParseError::TooShortBuffer) {
            std::cout << "Error: Too short buffer" << std::endl;
        }
    } else {
        const auto& [messageLength, messageType, sequenceNumber, payload] = std::get<ParseData>(result);
        std::cout << "Message Length: " << messageLength << std::endl;
        std::cout << "Message Type: " << static_cast<int>(messageType) << std::endl;
        std::cout << "Sequence Number: " << sequenceNumber << std::endl;
        std::cout << "Payload Size: " << payload.size() << std::endl;
    }

    return 0;
}
