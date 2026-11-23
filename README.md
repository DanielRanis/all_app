# Byte Parser Practice

## Day 1: Build a bounded byte parser

Write a C++20 function that reads one message from a byte buffer and returns either a parsed message or a parse error.
This is the task specification; it does not include a solution.

### Message format

All integer fields are big-endian. The message length includes the 7-byte header.

| Offset |                     Size | Field           |
|-------:|-------------------------:|-----------------|
|      0 |                  2 bytes | Message length  |
|      2 |                   1 byte | Message type    |
|      3 |                  4 bytes | Sequence number |
|      7 | Remaining declared bytes | Payload         |

For this exercise, supported message types are `1` and `2`. The payload may be empty and may contain arbitrary bytes.

### Function requirements

- Accept a non-owning byte range, such as `std::span<const std::byte>`.
- Return a result that distinguishes success from failure. On success, expose the message type, sequence number, and
  payload. The payload must refer to the input buffer rather than copy it.
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

## Day 2: Parse typed market data messages

Extend the Day 1 parser so it decodes typed payloads into structured market data updates. Keep the parser stateless and
allocation-free. This is the task specification; it does not include a solution.

### Common frame

Keep the Day 1 7-byte header. All integer fields are big-endian, and the message length includes the header.

| Offset |                     Size | Field                 |
|-------:|-------------------------:|-----------------------|
|      0 |                  2 bytes | Total message length  |
|      2 |                   1 byte | Message type          |
|      3 |                  4 bytes | Sequence number       |
|      7 | Remaining declared bytes | Type-specific payload |

Ignore bytes after the declared message; they may belong to the next frame.

### Payload formats

**Type 1: Quote update — exactly 17 payload bytes**

| Payload offset |    Size | Field                      |
|---------------:|--------:|----------------------------|
|              0 | 4 bytes | Instrument ID              |
|              4 | 8 bytes | Price in integer ticks     |
|             12 | 4 bytes | Quantity                   |
|             16 |  1 byte | Side: `1` = bid, `2` = ask |

**Type 2: Trade — exactly 16 payload bytes**

| Payload offset |    Size | Field                  |
|---------------:|--------:|------------------------|
|              0 | 4 bytes | Instrument ID          |
|              4 | 8 bytes | Price in integer ticks |
|             12 | 4 bytes | Quantity               |

### Requirements

- Return either a structured quote, a structured trade, or a parse error.
- Preserve the common sequence number in the structured result.
- Reject a payload whose length does not exactly match its message type's format.
- Reject quote sides other than `1` or `2`.
- Reject unsupported message types and invalid frame lengths.
- Do not read a field until its bytes are known to be available.
- Do not copy the input payload or allocate memory.

Allow about 40–50 minutes. Manually trace one valid quote, one valid trade, and one malformed message.
