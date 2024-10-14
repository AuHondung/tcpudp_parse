IPAddress lip;
IPAddress.TryParse(MyIpAddr, out lip);

MySocket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
MySocket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, 1);
IPEndPoint ipLocalPoint = new IPEndPoint(lip, MyPortNum);

MySocket.Bind(ipLocalPoint);
ori_rcv_size = MySocket.ReceiveBufferSize;
MySocket.ReceiveBufferSize = 1024*1024;
_closeFlag = true;
RevThread = new Thread(new ThreadStart(revFun));
RevThread.Start();
MySocket.BeginReceive(_revDataBuffBytes, 0, _revDataBuffBytes.Length, SocketFlags.None, REceiveCallback, null);
return MySocket;