#!/usr/bin/env python

from socket import *
from struct import *

class NetMalloc:
    sck = None

    def __init__(self, hostname, port):
        self.sck = socket(AF_INET, SOCK_STREAM)
        self.sck.connect((hostname, port))

    def Alloc(self, sz):
        packet = pack('<BI', 0x0, sz)

        self.sck.send(packet)
        error, id = self._Recv()
        if error != 0x0:
            return None
        return id

    def Free(self, id):
        packet = pack('<BI', 0x1, id)
        self.sck.send(packet)

        error, id = self._Recv()
        if error != 0x0:
            return False
        return True

    def Read(self, id, off, sz):
        packet = pack('<BIII', 0x2, id, off, sz)
        self.sck.send(packet)

        error, id, data = self._Recv(sz)
        if error != 0x0:
            return None
        return data

    def Write(self, id, buf, off, sz):
        packet = pack('<BIII%ds' % sz, 0x3, id, off, sz, buf)
        self.sck.send(packet)

        error, id = self._Recv()
        if error != 0x0:
            return False
        return True

    def __del__(self):
        self.sck.close()

    def _Recv(self, sz = 0):
        sz += 9
        packet = self.sck.recv(sz)

        error, id, data_len = unpack('<BII', packet)
        if data_len == 0:
            return (error, id)
        return (error, id, data[9:data_len + 9])

if __name__ == "__main__":
    nm = NetMalloc('localhost', 4567)

    print('Allocating... ',)
    id = nm.Alloc(0x100)
    print('Done')

    print('Writing... ',)
    nm.Write(id, b"AAAHello\x11", 0x1, 0x6)
    print('Done')

    print('Reading... ',)
    data = nm.Read(id, 4, 5)
    print('Done')
    print(data)

    print('Freeing... ')
    nm.Free(id)
    print('Done')

