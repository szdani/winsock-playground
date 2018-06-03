# winsock-playground
C++ Client Server code for Winsock "fun"

# How to speed up Winsock
## TCP loopback connection problem
![img](https://raw.githubusercontent.com/dspezia/redis-doc/client_command/topics/Data_size.png)

The default behavior of the TCP loopback interface is to move local TCP traffic through most of the network stack, including AFD (which is essentially the kernel mode representation of a user mode TCP socket),  as well as the layers corresponding to TCP and IP protocol layers. 

Windows Server 2012 and Windows 8 introduce a [new optional fast path](https://blogs.technet.microsoft.com/wincat/2012/12/05/fast-tcp-loopback-performance-and-low-latency-with-windows-server-2012-tcp-loopback-fast-path/) with **SIO_LOOPBACK_FAST_PATH** I/O Control Code (IOCTL) flag on socket.


1. Both ends of the TCP session must enable the capability by setting the SIO_LOOPBACK_FAST_PATH IOCTL.
2.    The process initiating the TCP connection must set the SIO_LOOPBACK_FAST_PATH IOCTL prior to establishing the TCP session.
3.   The target of the connection request must set the SIO_LOOPBACK_FAST_PATH IOCTL on the listen socket, that is, prior to   accepting the connection.
4.    This capability requires Windows Server 2012 or Windows 8.

![img2](https://msdnshared.blob.core.windows.net/media/TNBlogsFS/prod.evol.blogs.technet.com/CommunityServer.Blogs.Components.WeblogFiles/00/00/00/49/02/4314.lbfp.png)

## Windows restrictions
- 200,000 simultaneous TCP connections
- 25,000 HTTP requests per second
- 750Mbps on a transcontinental gigabit network consisting of 10 hops

How to avoid: trust your arhitect. :D 

## UNIX Domain socket on Windows
Beginning in Insider Build [17063](https://blogs.msdn.microsoft.com/commandline/2017/12/19/af_unix-comes-to-windows/), you’ll be able to use the unix socket (AF_UNIX) address family on Windows to communicate between Win32 processes. Unix sockets allow inter-process communication (IPC) between processes on the same machine. 

#### Unsupported

- AF_UNIX datagram (SOCK_DGRAM) or sequence packet (SOCK_SEQPACKET) socket type.
    - commonly used to avoid message framing and boundary parsing
- Windows cannot pass file descriptors (`SCM_RIGHTS`) or credentials (`SCM_CREDENTIALS`) over the socket. 
- Autobind feature (see the section on ‘sockaddr_un’ for details). socketpair: socketpair socket API is not supported in Winsock 2.0. 
    - Commonly used for IPC between parent-child processes

#### How to check if my win build is compatible or not?
`sc query afunix`

## Blocking - Non-Blocking Sockets
https://support.microsoft.com/en-us/help/823764/slow-performance-occurs-when-you-copy-data-to-a-tcp-server-by-using-a

## Sockets or Pipes?
https://stackoverflow.com/questions/10872557/how-slow-are-tcp-sockets-compared-to-named-pipes-on-windows-for-localhost-ipc
