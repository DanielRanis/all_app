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

Tighten the big-endian integer reader and explain why it is safe to use on network bytes. This is the task specification; it does not include a solution.

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

Be ready to explain how host byte order, alignment, padding, and object lifetime affect interpreting a network buffer. Allow about 30–40 minutes.

## Day 4: Maintain a sequenced order book

Build a small, fixed-capacity order book that consumes the Day 2 quote updates. Focus on correct state changes and sequence handling, not speed tuning. This is the task specification; it does not include a solution.

### Book model

- Support one instrument per book instance.
- Store up to 8 bid price levels and 8 ask price levels.
- Keep bids sorted from highest price to lowest, and asks from lowest price to highest.
- A quote update with quantity `0` removes that price level; otherwise it inserts or updates the level.
- If an update would exceed capacity, report an error and leave the book unchanged.

### Snapshot and update behavior

- Provide a way to load a snapshot: an instrument ID, a sequence number, and the current quote levels.
- A valid snapshot replaces the book and marks it synchronized.
- Apply a quote update only when its sequence number is exactly one greater than the book's current sequence.
- Ignore and report a duplicate or stale update whose sequence number is less than or equal to the current sequence.
- If there is a sequence gap, mark the book unsynchronized. Ignore further updates until a new valid snapshot arrives.
- Reject updates for a different instrument.
- A rejected snapshot must not partially change the current book.

### Constraints and checks

- Use fixed-capacity storage; do not allocate memory while applying snapshots or updates.
- Check invalid side values and capacity limits.
- Verify that bids and asks remain correctly sorted.
- Manually trace a snapshot, an update, a level removal, a duplicate, a sequence gap, and recovery with a new snapshot.

Allow about 60–90 minutes. Be ready to explain how the book stays safe after a gap and why you require a fresh snapshot before applying more updates.
