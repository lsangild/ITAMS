using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace UDPAckServer
{
    class Program
    {
        const int port = 30000;

        static void Main(string[] args)
        {
            UdpClient listener = new UdpClient(port);
            byte[] data = new byte[1024]; 

            while (true)
            {
                IPEndPoint sender = new IPEndPoint(IPAddress.Any, 0);
                data = listener.Receive(ref sender);

                Console.WriteLine(Encoding.ASCII.GetString(data, 0, data.Length));

                string msg = "Ack + " + data.Length;
                byte[] msgData = Encoding.ASCII.GetBytes(msg);

                listener.Send(msgData, msgData.Length, sender);
            }            
        }
    }
}
