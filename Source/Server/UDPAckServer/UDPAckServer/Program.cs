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
        public struct GPS_Data_T
        {
            public ushort year;
            public byte month;
            public byte date;
            public byte hour;
            public byte minute;
            public byte second;
            public uint lat;
            public uint lon;
            public byte dataValid;

            public void Print()
            {
                Console.WriteLine("Time recevied:" + DateTime.Now.ToString());
                Console.WriteLine("Year: \t\t" + year);
                Console.WriteLine("Month: \t\t" + month);
                Console.WriteLine("Date: \t\t" + date);
                Console.WriteLine("Hour: \t\t" + hour);
                Console.WriteLine("Minute: \t" + minute);
                Console.WriteLine("Second: \t" + second);
                Console.WriteLine("Latitude: \t" + (float)lat / 10000000);
                Console.WriteLine("Longitude: \t" + (float)lon / 10000000);
                Console.WriteLine("Data valid: \t" + dataValid);
            }

            public GPS_Data_T(byte[] data)
            {
                year = BitConverter.ToUInt16(data, 0);
                month = data[2];
                date = data[3];
                hour = data[4];
                minute = data[5];
                second = data[6];
                dataValid = data[7];
                lat = BitConverter.ToUInt32(data, 8);
                lon = BitConverter.ToUInt32(data, 12);
            }
        }
        static void Main(string[] args)
        {
            UdpClient listener = new UdpClient(port);
            byte[] data = new byte[1024]; 

            while (true)
            {
                IPEndPoint sender = new IPEndPoint(IPAddress.Any, 0);
                data = listener.Receive(ref sender);

                Console.WriteLine(BitConverter.ToString(data, 0, data.Length));

                GPS_Data_T gps = new GPS_Data_T(data);

                gps.Print();

                string msg = "Ack + " + data.Length;
                byte[] msgData = Encoding.ASCII.GetBytes(msg);

                listener.Send(msgData, msgData.Length, sender);
            }            
        }
    }
}
