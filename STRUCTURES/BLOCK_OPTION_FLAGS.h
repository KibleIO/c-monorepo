#ifndef BLOCK_OPTION_FLAGS_H_
#define BLOCK_OPTION_FLAGS_H_

#define BLOCK_OPTION_NONE                   0b00000000000000000000000000000000 //  0 - reserved for zeroing all option flags
#define BLOCK_OPTION_RETAIN_OFFLINE         0b00000000000000000000000000000001 //  1 - block structure is retained on connection termination
#define BLOCK_OPTION_STORE_BLOCK            0b00000000000000000000000000000010 //  2 - block structure is written to disk as convenient
#define BLOCK_OPTION_PROTECT_BLOCK          0b00000000000000000000000000000100 //  3 - block structure is written to disk immediately; does not send or confirm received until write is complete
#define BLOCK_OPTION_PARALLEL_COMMANDS      0b00000000000000000000000000001000 //  4 - commands are produced/applied concurrently in parallel threads, if unset commands are produced/applied sequentially in a single thread
#define BLOCK_OPTION_ORDER_COMMANDS         0b00000000000000000000000000010000 //  5 - commands are produced/applied in a defined order, if unset commands are produced/applied as they become available
#define BLOCK_OPTION_LATEST_COMMAND         0b00000000000000000000000001000000 //  7 - only the latest commit data is transferred, if unset all commited blocks are sent
#define BLOCK_OPTION_COMPRESS_COMMANDS      0b00000000000000000000000010000000 //  8 - compression is considered based on BLOCK_TYPE for transferred commands, if unset compression is not considered
#define BLOCK_OPTION_LOSSY_COMMANDS         0b00000000000000000000000100000000 //  9 - data MAY be inconsistent with its source. if this option is unset the data IS lossless. (Assumes BLOCK_OPTION_LOSSY)
#define BLOCK_OPTION_10                     0b00000000000000000000001000000000 // 10 - Reserved
#define BLOCK_OPTION_11                     0b00000000000000000000010000000000 // 11 - Reserved
#define BLOCK_OPTION_12                     0b00000000000000000000100000000000 // 12 - Reserved
#define BLOCK_OPTION_13                     0b00000000000000000001000000000000 // 13 - Reserved
#define BLOCK_OPTION_14                     0b00000000000000000010000000000000 // 14 - Reserved
#define BLOCK_OPTION_15                     0b00000000000000000100000000000000 // 15 - Reserved
#define BLOCK_OPTION_16                     0b00000000000000001000000000000000 // 16 - Reserved
#define BLOCK_OPTION_17                     0b00000000000000010000000000000000 // 17 - Reserved
#define BLOCK_OPTION_18                     0b00000000000000100000000000000000 // 18 - Reserved
#define BLOCK_OPTION_19                     0b00000000000001000000000000000000 // 19 - Reserved
#define BLOCK_OPTION_20                     0b00000000000010000000000000000000 // 20 - Reserved
#define BLOCK_OPTION_21                     0b00000000000100000000000000000000 // 21 - Reserved
#define BLOCK_OPTION_22                     0b00000000001000000000000000000000 // 22 - Reserved
#define BLOCK_OPTION_23                     0b00000000010000000000000000000000 // 23 - Reserved
#define BLOCK_OPTION_24                     0b00000000100000000000000000000000 // 24 - Reserved
#define BLOCK_OPTION_25                     0b00000001000000000000000000000000 // 25 - Reserved
#define BLOCK_OPTION_26                     0b00000010000000000000000000000000 // 26 - Reserved
#define BLOCK_OPTION_27                     0b00000100000000000000000000000000 // 27 - Reserved
#define BLOCK_OPTION_28                     0b00001000000000000000000000000000 // 28 - Reserved
#define BLOCK_OPTION_29                     0b00010000000000000000000000000000 // 29 - Reserved
#define BLOCK_OPTION_30                     0b00100000000000000000000000000000 // 30 - Reserved
#define BLOCK_OPTION_31                     0b01000000000000000000000000000000 // 31 - Reserved
#define BLOCK_OPTION_32                     0b10000000000000000000000000000000 // 32 - Reserved

#define BLOCK_LATENCY_IMMEDIATE             0x00000000 // Highest priority - Reserved for key exhaustion and protocol synchronization
#define BLOCK_LATENCY_URGENT                0x00000005 //    5 milliseconds (200 Hz)
#define BLOCK_LATENCY_LOW                   0x0000000A //   10 milliseconds (100 Hz)
#define BLOCK_LATENCY_AUDIBLE               0x0000000A //   10 milliseconds (100 Hz) // https://en.wikipedia.org/wiki/Latency_(audio)
#define BLOCK_LATENCY_VISABLE_GAMING        0x00000014 //   20 milliseconds ( 50 Hz)
#define BLOCK_LATENCY_VISABLE               0x0000002A //   42 milliseconds ( 24 Hz)
#define BLOCK_LATENCY_MODERATE              0x00000032 //   50 milliseconds ( 20 Hz)
#define BLOCK_LATENCY_HIGH                  0x00000064 //  100 milliseconds ( 10 Hz)
#define BLOCK_LATENCY_1_SECOND              0x000003E8 // 1000 milliseconds (  1 Hz)
#define BLOCK_LATENCY_3_SECONDS             0x00000BB8 // 1000 * 3
#define BLOCK_LATENCY_5_SECONDS             0x00001388 // 1000 * 5
#define BLOCK_LATENCY_10_SECONDS            0x00002710 // 1000 * 10
#define BLOCK_LATENCY_20_SECONDS            0x00004E20 // 1000 * 20
#define BLOCK_LATENCY_25_SECONDS            0x000061A8 // 1000 * 25
#define BLOCK_LATENCY_30_SECONDS            0x00007530 // 1000 * 30
#define BLOCK_LATENCY_45_SECONDS            0x0000AFC8 // 1000 * 45
#define BLOCK_LATENCY_1_MINUTE              0x0000EA60 // 1000 * 60 * 1
#define BLOCK_LATENCY_3_MINUTES             0x0002BF20 // 1000 * 60 * 3
#define BLOCK_LATENCY_5_MINUTES             0x000493E0 // 1000 * 60 * 5
#define BLOCK_LATENCY_10_MINUTES            0x000927C0 // 1000 * 60 * 10
#define BLOCK_LATENCY_20_MINUTES            0x00124F80 // 1000 * 60 * 20
#define BLOCK_LATENCY_25_MINUTES            0x0016E360 // 1000 * 60 * 25
#define BLOCK_LATENCY_30_MINUTES            0x001B7740 // 1000 * 60 * 30
#define BLOCK_LATENCY_45_MINUTE             0x002932E0 // 1000 * 60 * 45
#define BLOCK_LATENCY_1_HOUR                0x0036EE80 // 1000 * 60 * 60 * 1
#define BLOCK_LATENCY_3_HOURS               0x00A4CB80 // 1000 * 60 * 60 * 3
#define BLOCK_LATENCY_6_HOURS               0x01499700 // 1000 * 60 * 60 * 6
#define BLOCK_LATENCY_12_HOURS              0x02932E00 // 1000 * 60 * 60 * 12
#define BLOCK_LATENCY_18_HOURS              0x03DCC500 // 1000 * 60 * 60 * 18
#define BLOCK_LATENCY_1_DAY                 0x05265C00 // 1000 * 60 * 60 * 24 * 1
#define BLOCK_LATENCY_2_DAYS                0x0A4CB800 // 1000 * 60 * 60 * 24 * 2
#define BLOCK_LATENCY_3_DAYS                0x0F731400 // 1000 * 60 * 60 * 24 * 3
#define BLOCK_LATENCY_5_DAYS                0x19BFCC00 // 1000 * 60 * 60 * 24 * 5
#define BLOCK_LATENCY_7_DAYS                0x240C8400 // 1000 * 60 * 60 * 24 * 7
#define BLOCK_LATENCY_14_DAYS               0x48190800 // 1000 * 60 * 60 * 24 * 14
#define BLOCK_LATENCY_28_DAYS               0x90321000 // 1000 * 60 * 60 * 24 * 28
#define BLOCK_LATENCY_30_DAYS               0x9A7EC800 // 1000 * 60 * 60 * 24 * 30
#define BLOCK_LATENCY_EVENTUALLY            0xFFFFFFFE // Lowest send priority - transfered only if no other packets are staged
#define BLOCK_LATENCY_INFINITE              0xFFFFFFFF // Indicates that block data will never be sent

#endif // BLOCK_OPTION_FLAGS_H_
