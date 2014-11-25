Xilinx lwIP Emulation for Linux
===============================

In many projects using Xilinx hardware (for example their Zynq system-on-a-chip
that combines two ARM cores with some FPGA fabric) the go-to solution for
driving the Ethernet interface in the absence of an operating system is the lwIP
library, as the Xilinx SDK already includes drivers for it. During development,
it can be convenient to build the software running on the CPU cores for the
(Linux) development box for easier debugging without requiring hardware access.

Luckily, the lwIP library includes some support for this use case. However,
setting it up for raw mode applications (as compared to the socket layer) is
is somewhat non-trivial and does not quite work out-of-the-box with the official
lwIP codebase.

This repository serves as an example on how to set up an emulator build for a
typical Xilinx SDK project that uses lwIP. Right now, the code will only
run on Linux, as there is no cross-platform way to create the virtual network
device for the emulator to use. It should be fairly easy to extend the code to
other Posix-like systems, though. The lwIP source code also seems to contain a
Windows port, but I have not looked at it in any detail yet.


Getting Started
---------------

The `emu` directory contains everything related to the hardware emulator build.
In `emu/lib`, the lwIP source files can be found. They have been extracted from
the release tarballs available on the project website, with a few additional
patches (see Git history). `emu/include` and `emu/lib` contain the shims to
forward the routines specific to the Xilinx EMAC driver to the Linux
implementation.

The `xsdk` directory would normally contain the relevant Xilinx SDK source and
project files. For this demonstration, only a simple `main.c` is included. In
a real application, additional mock implementations of other Xilinx drivers
(e.g. platform setup, GPIOs, etc.) would also be used. To keep things simple,
no further headers have been included here. Thus, there are only a couple of
comments in `main.c` showing how the platform initialization code might look
like.

To build the Linux executable, run `make` in `emu`. When you execute it (the
network configuration code might require root privileges), it will listen on
IP address 192.168.0.2, TCP port 12345 for connections and just print out a
message whenever a packet is received. (If your PC is already connected to a
network that uses the 192.168.0.0/24 subnet, you might want to momentarily
disconnect from it or change the addresses in `main.c` before running the
sample program.)


Linux-side Network Configuration
--------------------------------

The implementation is based on the `mintapif` interface from `lwip-contrib`. It
works by creating a `tap` interface, which is a basically a pipe where a
user-space program can send/receive complete Ethernet frames to/from the Linux
kernel. As far as the latter is concerned, it is just another network interface,
even if it happens not to correspond to actual hardware.

When it comes to interacting with the emulator program from the Linux host, it
is important to realize that there are actually two places where network
configuration takes place. One is inside the binary using lwIP: How the program
interprets the Ethernet frames it receives from the kernel is entirely up it.
For example, we wouldn't even need to use any of the standard protocols (IP v4/6,
ARP, etc.) at all! In any case, this means that the Linux kernel is completely
oblivious to whatever the adddress settings and so on of the IP stack in the
emulator executable are.

However, if you look at the output of `ip a` while the example is running, you
will notice that an IP address is mentioned for the `tap` interface. This is the
address that the Linux end of the connection is assigned inside the kernel. In
theory it can be set arbitrarily, although it should be in the same subnet as
the emulator executable so that packets for the latter are actually sent to the
`tap` device.

This address will correspond to the gateway address specified in the lwIP client
code and is assigned during initialization of the lwIP driver. In fact, for
reasons of simplicity the original lwIP code always configures the `tap0` device
(when you open a `tap` device, you just receive an opaque handle; figuring out
its name in order to configure the Linux side of things is somewhat involved).
I did not get around to cleaning this up yet, as it makes for a smooth starting
experience in most cases.

But what if you want to emulate multiple devices connected to the same network
as the host? In this case, you manually need to create a bridge device. On
recent Linux distributions, this might be a useful starting point:


```
./first-program
./second-program

# Create a new bridge device.
sudo ip link add name br0 type bridge

# Add the tap devices to it so that the emulator
# programs can communicate with each other.
sudo ip link set dev tap0 master br0
sudo ip link set dev tap1 master br0

# Assign the host IP address to the bridge so both
# programs are reachable from Linux.
sudo ip addr del dev tap0 192.168.0.1/24
sudo ip addr del dev br0 192.168.0.1/24
```
