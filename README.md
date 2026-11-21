# Byte Parser Practice

## Day 1: Build a bounded byte parser

Write a C++20 function that reads one message from a byte buffer and returns either a parsed message or a parse error. This is the task specification; it does not include a solution.

### Message format

All integer fields are big-endian. The message length includes the 7-byte header.

| Offset | Size | Field |
| ---: | ---: | --- |
| 0 | 2 bytes | Message length |
| 2 | 1 byte | Message type |
| 3 | 4 bytes | Sequence number |
| 7 | Remaining declared bytes | Payload |

For this exercise, supported message types are `1` and `2`. The payload may be empty and may contain arbitrary bytes.

### Function requirements

- Accept a non-owning byte range, such as `std::span<const std::byte>`.
- Return a result that distinguishes success from failure. On success, expose the message type, sequence number, and payload. The payload must refer to the input buffer rather than copy it.
- Reject input shorter than the 7-byte header.
- Reject a declared length smaller than the header or larger than the available input.
- Reject unsupported message types.
- Ignore bytes after the declared message; they belong to a following message.
- Do not read a field until you have established that its bytes are available.
- Do not allocate memory.

### Suggested work order

1. State assumptions and error cases.
2. Sketch the result type and function signature.
3. Implement the length and header checks.
4. Decode the fields and expose the payload.
5. Manually trace one valid message and one malformed message.

Allow about 30–40 minutes. The returned payload view is valid only while the input buffer remains alive and unchanged.
