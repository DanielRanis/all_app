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

## Day 3: Safe integer decoding and alignment

Tighten the big-endian integer reader and explain why it is safe to use on network bytes. This is the task
specification; it does not include a solution.

### Requirements

- Make the helper accept a byte span and an unsigned integer type.
- Require the span to contain exactly the number of bytes needed for that type; reject shorter or longer spans.
- Decode `std::uint16_t`, `std::uint32_t`, and `std::uint64_t` values in big-endian order.
- Exercise it on byte spans that begin at an unaligned address, such as offset 1 in a larger buffer.
- Do not cast packet-buffer pointers to integer or packet-struct pointers.
- Do not allocate memory.

### Check these cases

- Decode known values: `0x1234`, `0x12345678`, and `0x0123456789ABCDEF`.
- Confirm the same bytes decode correctly when they start at an unaligned offset.
- Confirm undersized and oversized spans are rejected.
- Re-run the Day 2 quote and trade examples.

Be ready to explain how host byte order, alignment, padding, and object lifetime affect interpreting a network buffer.
Allow about 30–40 minutes.

## Day 4: Maintain a fixed-capacity order book

Build a fixed-capacity order book that applies quote updates for one instrument. Focus on correct level updates,
ordering, and capacity handling. This is the task specification; it does not include a solution.

### Book model

- Support one instrument per book instance.
- Store up to 8 bid price levels and 8 ask price levels independently.
- Keep bids sorted from highest price to lowest, and asks from lowest price to highest.
- For an existing price level, a positive quantity replaces its quantity.
- For a new price level, a positive quantity inserts it in sorted order.
- A zero-quantity update removes the matching price level. Return an error if that price is not present.
- Reject invalid instruments, sides, and prices.
- If the requested side is at capacity, report an error and leave the book unchanged.
- Use fixed-capacity storage; do not allocate memory while applying updates.

### Book state

- `Empty`: both sides have zero levels.
- `Active`: at least one level exists, and the book is not full.
- `Full`: both sides have 8 levels.
- Update the state after each successful add, update, or removal. A failed operation leaves the book unchanged.

### Checks

- Manually trace bid and ask insertion in sorted order.
- Trace replacement of an existing level's quantity.
- Trace removal from the beginning, middle, and end of a side.
- Trace removal of a price that is not present.
- Trace one side reaching capacity while the other still has room.
- Trace the transition to `Full` when both sides reach capacity, and back to `Active` after a removal.
- Trace the transition to `Empty` after removing the last level.
- Check invalid side, instrument, and price handling.

Snapshot loading, sequence numbers, gap detection, and resynchronization are deferred to a separate task.
Allow about 60–90 minutes.
