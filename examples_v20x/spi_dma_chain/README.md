# SPI chained network

This examples demonstrates how to use two SPI peripherals on a single MCU to make a chained network of similar devices. One SPI works in Master mode and the other in Slave mode. Each node is identical in terms of *network* config (except the node ID) and can be inserted as a link in any part of the chain. Theoretically this gives ability to make a *network* with dynamic topology without reconfiguring individual nodes. The main focus of the code is to show how to configure and use both SPI peripherals to achieve these goals. Rudimentary networking protocol is very basic and needs serious work to become a robust implementation.

## Setup

This example should work on any CH32 MCU that has 2 SPI peripherals. All of them have the same pinout:

|        | CS  | SCK | MISO | MOSI |
| :-:    | :-: | :-: | :-:  | :-:  |
|**SPI1**|PA4  |PA5  |PA6   |PA7   |
|**SPI2**|PB12 |PB13 |PB14  |PB15  |

By default SPI1 is configured in Master mode and SPI2 - in Slave. You can change that designation in code by changing ``spi_master`` and ``spi_slave`` pointers.

Nodes should be connected directly to corresponding pins, without crossing MISO and MOSI.

Common GND connection needs to be made between each two nodes.

## Discovered quirks/limitations

- At least with my setup MISO GPIO of Slave SPI needs to be in 2MHz mode, or it will trash both incoming and outgoing transmissions while sending.
- 2MHz GPIO mode works for all pins on both SPIs even when working at higher baudrate.
- I'm using HW CS pin on Slave side and toggling CS in software on Master side. This makes the most sense.
- You can modify the code to use 16 bit transmission mode. This will work at the same speed with double data rate. But then you'd need to pack uint8_t to uint16_t and vice versa.
- In this setup SPI works in Full Duplex mode. This means that it's transmitting and receiving at the same time synchronously. Pro is that it fast and *convenient*. Con is that it's *inconvenient* because if you want to receive something on Master from Slave you need to send something, all zeroes for example. And if you want to send something from Slave to Master, you need for Master to initiate the transmission first. There is a way to make signalling from Slave back to Master to trigger the transmission, but for now I think polling periodically is the best solution.
- Interrupts are strange, I don't think I was able to make nesting priority work, so I decided to use DMA Transmission Complete interrupt only to disable DMA Out channel and set flag. And then do all the work in main loop. Slave SPI has to do everything in interrupt, because it is very time sensitive procedure.
- Baudrate can be varied along the *network*. You can chose different SPI frequency on per link basis. Just pick the frequency on the Master side that Slave can keep up with.
- You may need to disable Master polling on the last node or terminate pins in some way, otherwise you can potentially get garbage data, that could confuse the network.

Following was written before I switched Slave to DMA. But still I think these are useful observations:

- Processor couldn't keep up sending data in time at speeds > 18MHz.
- Slave SPI needs a delay on Master side between pulling CS low and starting a transmission to be able to write data to DATAR register in time.
- DATAR isn't cleared when bits are shifted out. The same data will be shifted on next byte transfer unless you clear it manually.
- If you *clear* DATAR manually, but after transmission is done, ``0`` will be sent as a first byte on the next transmission.

## Protocol

This protocol is built using 2 separate SPI peripherals - one in Master mode and one in Slave mode. Both are running in Full-duplex mode. The master represents a downstream segment of the chain and slave - an upstream. Slave doesn't have control over how many bytes it will send during each transmission, it will send anything it has in the buffer until upstream node's master sends anything to it. Master of the current node, on the other hand, can send any amount of data it wants, and it will receive the same amount of bytes in return. So in both cases both in and out buffers are being fulled/emptied at the same rate. And if all nodes on the chain are working according to the same standard, we also can expect that number of bytes in each transmission will be a multiple of the chosen *packet size*.

### Message structure

Packet = 10 bytes:

| 0           | 1             | 2-3                                         | 4-9               |
|       :-    | :-            | :-                                          | :-                |
| [sender ID] | [receiver ID] | [message number + ACK/NAK bit + reply flag] | [6 bytes of data] |

Each message has unique ID number so nodes can know what command is data replies to. Also you can make messages of any length by sending multiple packets with the same message number.

Two LSB of message ID are for detecting the type of the message. If the first bit is set it indicates that this is a *new message* in the meaning that it's not a reply and is originated on the sender side. The second bit's meaning depends on first one - if this is a new message ``1`` as a second bit will show that this is a beginning of the message and ``0`` will indicate that is's a continuation of a multi-part message. If the first bit indicates that this is a reply message - ``1`` is ACK and ``0`` is NAK.

Each message has to have *sender ID* or it will be discarded. *Receiver ID* can be left blank for sending messages to a neighbor node. Also initial enumeration will be triggered only on a messages from neighbors - with *receiver ID* set to ``0``.

If incoming command can't be processed at the moment (node is processing another command for example) NAK reply should be send. Sender will repeat the command in new message later.

Reply flag is used to indicate that the message is a reply to previously sent message. If receiver has a pending command with this message ID, then it processes the data, otherwise message is discarded.

If received packet has receiver ID that is different from the current node ID, then it copied to OUT buffer of another SPI peripheral to be sent during next transmission. This way messages can be sent from each node to any other node in both up and downstream directions.

### Enumeration

Each SPI struct has an array for nodes map. This array will contain all node IDs that are in a chain after this node. First ([0]) ID is filled when node is start communication. For SPI in Slave mode it will eventually receive a polling message from upstream Master. If this is a polling message, only the first byte will be present and since it represents the sender's ID it will be placed into an array as a first node. For SPI in Master node it will eventually receive a response for its polling messages and it will extract an ID from it. (All messages with empty message number will be discarded and won't be passed further by the chain, so if such message arrives we can be sure it is from a neighbor.)

When first ID is placed into nodes map array the ``enumeration_state`` field of SPI struct is set to ``ENUM_CHANGED`` state. This field will be checked on the next ``process_message`` call and then message with ``CMD_ENUM_UPDATE`` will be sent to both SPIs with current node map from opposite SPIs. Every time new node is detected on either end of the chain it will propagate through the rest of nodes and all nodes will get updated node maps.

### Polling

To get messages from the Slave even when whe don't have anything to send to it we need to make polling transmissions with empty messages. By default we are polling after predetermined delay. If we've received anything from the Slave it means that it's buffer can have more messages, so we poll once more without a delay, until we get empty message, that will indicate that other node doesn't have anything to send at the moment.

## Using the demo

You need to flash at least two nodes to try this demo. If you want longer than neighbor-to-neighbor communication you need to flash every node with a unique ``NODE_ID``. By default every node will be polling on it's Master (downstream) interface every 1.5s and once it found a neighbor it will send periodical static messages (every 1s). Also a simple LED toggle function is preconfigured. Pressing a button on a node will send ``LED_TOGGLE`` command to the last known node downstream.

LED and Button GPIOs are configured according to the ones that are used on *BluePill* WeACT CH32V203 boards or cheap CH32V307VCT6 devboard. You can use any other, simply adjust the defines.

I've tested this with 4 nodes chain: 2 CH32V203, 1 CH32V307 and 1 CH32V103.

## WiP

- Message processing could be better
- Protocol could be better
- Enumeration logic needs some love
- Probably there are some bugs in buffer handling (I hope not)ss

Bigger/faster setups probably will need a bigger buffer size.

I don't know how fast reliable it can be in real life scenario. Any fixes/additions are welcomed.
