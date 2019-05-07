# Trivial File Transfer Protocol

<h2>Implemented TFTP protocol using C language.</h2>
--used TFTP_RFC1350 as reference


<h3>Summary</h3>
<p>
 TFTP is a simple protocol to transfer files, and therefore was named
 the Trivial File Transfer Protocol or TFTP. It has been implemented
 on top of the Internet User Datagram protocol (UDP or Datagram).
   so it may be used to move files between machines on different
 networks implementing UDP. (This should not exclude the possibility
 of implementing TFTP on top of other datagram protocols.) It is
 designed to be small and easy to implement. Therefore, it lacks most
 of the features of a regular FTP. The only thing it can do is read
 and write files (or mail) from/to a remote server. It cannot list
 directories, and currently has no provisions for user authentication.
 In common with other Internet protocols, it passes 8 bit bytes of
 data.

 </p>


<h3> Overview of the Protocol</h3>
 Any transfer begins with a request to read or write a file, which
 also serves to request a connection. If the server grants the
 request, the connection is opened and the file is sent in fixed
 length blocks of 512 bytes. Each data packet contains one block of
 data, and must be acknowledged by an acknowledgment packet before the
 next packet can be sent. A data packet of less than 512 bytes
 signals termination of a transfer. If a packet gets lost in the
 network, the intended recipient will timeout and may retransmit his
 last packet (which may be data or an acknowledgment), thus causing
 the sender of the lost packet to retransmit that lost packet. The
 sender has to keep just one packet on hand for retransmission, since
 the lock step acknowledgment guarantees that all older packets have
 been received. Notice that both machines involved in a transfer are
 considered senders and receivers. One sends data and receives
 acknowledgments, the other sends acknowledgments and receives data.
 Most errors cause termination of the connection. An error is
 signalled by sending an error packet. This packet is not
 acknowledged, and not retransmitted (i.e., a TFTP server or user may
 terminate after sending an error message), so the other end of the
 connection may not get it. Therefore timeouts are used to detect
 such a termination when the error packet has been lost. Errors are
 caused by three types of events: not being able to satisfy the
 request (e.g., file not found, access violation, or no such user),
 receiving a packet which cannot be explained by a delay or
 duplication in the network (e.g., an incorrectly formed packet), and
 losing access to a necessary resource (e.g., disk full or access
 denied during a transfer).
 TFTP recognizes only one error condition that does not cause
 termination, the source port of a received packet being incorrect.
 In this case, an error packet is sent to the originating host.
 This protocol is very restrictive, in order to simplify
 implementation. For example, the fixed length blocks make allocation
 straight forward, and the lock step acknowledgement provides flow
 control and eliminates the need to reorder incoming data packets.



 <h3>TFTP Packets</h3>
 TFTP supports five types of packets, all of which have been mentioned
 above:
 opcode operation
 <ul>
<li> 1 Read request (RRQ)</li>
 <li>2 Write request (WRQ)</li>
 <li>3 Data (DATA)</li>
<li> 4 Acknowledgment (ACK)</li>
 <li>5 Error (ERROR)</li>
</ul>
