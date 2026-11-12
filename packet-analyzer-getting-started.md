# Packet Analyzer — Getting Started

A C++20 command-line packet analyzer for Linux. The goal is to capture packets from an interface, decode their protocol headers yourself, and display useful summaries.

This project deliberately uses **libpcap only for packet capture**. You will implement Ethernet, IPv4, TCP, UDP, and ICMP parsing yourself. This keeps the learning focused on packet formats, byte handling, and systems programming.

## 1. Scope and milestones

Build one milestone at a time. Keep each milestone working before adding the next.

### Milestone 1 — Capture packets

**Goal:** Open a network interface and print a line for every captured packet.

Tasks:

1. Install the build tools and libpcap development headers.
2. Create a CMake project using C++20.
3. List available interfaces using `pcap_findalldevs`.
4. Open the selected interface using `pcap_open_live`.
5. Capture packets using `pcap_loop` or `pcap_dispatch`.
6. Print the timestamp and captured/original packet lengths.
7. Handle Ctrl+C and clean up the capture handle.

At this stage, do not decode protocol headers.

Example output:

```text
19:42:01.123456  captured=74 bytes  wire=74 bytes
19:42:01.234567  captured=128 bytes wire=128 bytes
```

**Done when:** you can select an interface, capture traffic, stop cleanly, and report packet lengths.

### Milestone 2 — Ethernet decoding

**Goal:** Interpret the first bytes as an Ethernet II header.

Extract:

- Destination MAC address.
- Source MAC address.
- EtherType.

Initially support IPv4 (`0x0800`) and ARP (`0x0806`) identification. You do not need to decode ARP yet.

**Done when:** captured Ethernet frames display both MAC addresses and their EtherType.

### Milestone 3 — IPv4 decoding

**Goal:** Parse IPv4 packets safely.

Extract:

- Version and Internet Header Length (IHL).
- Total length.
- Identification and fragmentation fields.
- TTL.
- Protocol number.
- Header checksum.
- Source and destination IPv4 addresses.

Important cases:

- IPv4 headers can contain options, so the header is not always 20 bytes.
- A captured packet can be truncated.
- IP packets can be fragmented.
- Validate every length before reading bytes.

**Done when:** IPv4 packets are decoded correctly and malformed/truncated packets are rejected without crashing.

### Milestone 4 — Transport protocols

Use the IPv4 protocol field to identify the payload:

- TCP (`6`)
- UDP (`17`)
- ICMP (`1`)

For TCP, decode source/destination ports, sequence and acknowledgement numbers, data offset, flags, and window size.

For UDP, decode source/destination ports, length, and checksum.

For ICMP, start with type, code, and checksum.

Remember that TCP headers can contain options, just like IPv4 headers can.

**Done when:** the analyzer prints useful summaries for TCP, UDP, and ICMP packets.

### Milestone 5 — Filtering and statistics

Add command-line filters such as:

```bash
packet-analyzer --interface eth0 --protocol tcp
packet-analyzer --interface eth0 --port 443
packet-analyzer --interface eth0 --host 192.168.1.10
```

Add statistics:

- Packets per protocol.
- Bytes per protocol.
- Top source and destination IPs.
- Packets per second.
- Total captured and dropped packets, where available.

You can use libpcap's BPF filter support (`pcap_compile` and `pcap_setfilter`) for capture-time filtering. You should still understand what the filter means.

### Milestone 6 — Offline PCAP analysis

Add the ability to read saved capture files:

```bash
packet-analyzer --read capture.pcap
```

Use libpcap's offline capture functions to read packet records, then pass each record through the same decoder used for live capture.

Add packet export if useful.

### Later extensions

Only after the earlier milestones work:

- DNS message parsing.
- TCP flow tracking and stream reassembly.
- IPv6.
- VLAN tags.
- IP fragment reassembly.
- Concurrent capture and analysis using a bounded queue.
- Terminal UI.
- PCAP-NG support.

Stream reassembly is a substantial feature: TCP is a byte stream, and packets may arrive out of order, be retransmitted, or contain only part of an application message.

## 2. Recommended tools

- **Language:** C++20
- **Build system:** CMake
- **Capture library:** libpcap
- **Tests:** GoogleTest, or a small test executable initially
- **Debugging:** GDB
- **Memory/undefined-behavior checks:** AddressSanitizer and UndefinedBehaviorSanitizer
- **Formatting:** clang-format

On Fedora:

```bash
sudo dnf install gcc-c++ cmake make libpcap-devel
```

On Ubuntu/Debian:

```bash
sudo apt install build-essential cmake libpcap-dev
```

On Arch Linux:

```bash
sudo pacman -S base-devel cmake libpcap
```

You may need elevated privileges or the appropriate capture permissions to capture on a live interface. Prefer granting only the required capture capability/permissions rather than running the entire application as root. For early development, an existing PCAP file is also useful.

## 3. Important implementation rules

1. **Do not cast arbitrary packet bytes directly to C++ header structs.** Alignment, padding, endianness, and bounds make this unsafe or incorrect.
2. Parse from a byte buffer using explicit offsets and bounds checks.
3. Convert network byte order explicitly. Network protocol integers are big-endian.
4. Treat captured length and original wire length as different values. Never read beyond the captured length.
5. Validate header lengths before accessing fields or payloads.
6. Keep packet parsing independent of live capture so the same decoder can be unit-tested with byte arrays and reused for PCAP files.
7. Do not assume every Ethernet frame contains IPv4, or that every IPv4 packet contains TCP.
8. Do not try to decrypt HTTPS. The analyzer can inspect packet and TLS metadata, but encrypted application content remains encrypted.

## 4. First coding session

Start with only these tasks:

1. Create the directory structure below.
2. Write a minimal `CMakeLists.txt`.
3. Write `main.cpp` that lists interfaces.
4. Add an interface-selection argument.
5. Open the selected interface and capture packets.
6. Print timestamp, captured length, and original length.

Do not implement protocol parsing until this works.

A good first commit would be:

```text
Initial project setup and libpcap interface listing
```

Then:

```text
Add live packet capture and packet length reporting
```

## 5. Definition of a good first release

Version 0.1 should:

- Build cleanly with CMake.
- List and select interfaces.
- Capture packets on Linux.
- Stop cleanly on Ctrl+C.
- Print packet timestamps and lengths.
- Handle capture errors.
- Have a README with build and usage instructions.

Keep the first release small. A reliable capture loop is a better foundation than a large collection of half-working protocol parsers.
