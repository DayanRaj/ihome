import socket
import threading
import sys
import MySQLdb
import os
import glob
import urllib2
import requests
import json
import time

baseURL = 'https://test1-fd420.firebaseio.com'


os.environ["PORT"]="117"
host = '172.24.1.1'
port = 117

class client(threading.Thread):
    def __init__(self, conn):
        super(client, self).__init__()
        self.conn = conn
        self.data = ""
        self.db = MySQLdb.connect('localhost','root','lc@15','iotdb')
        self.exe=self.db.cursor()

    def run(self):
        while True:
            self.data = self.data + self.conn.recv(30)
            if self.data.endswith('FE'):
                self.t=self.data.split(",")
                print self.data
                for x in range(0,7):
                    print self.t[x],"\t"
                self.cmd="INSERT INTO demodb VALUES(0,%d,%d,'%s','%s','%s',now())"%(int(float(self.t[1])),int(float(self.t[2])),self.t[3],self.t[4],self.t[5])
                self.exe.execute(self.cmd)
                self.db.commit()
                self.tspeak()
                self.data = ""

    def tspeak(self):
        print "Entered tspeak"
	time_hhmmss = time.strftime('%H:%M:%S')
	date_mmddyyyy = time.strftime('%d/%m/%Y')
        self.apikey=baseURL+'/'+self.t[5]+'/'+self.t[5]+'.json'
	data = {'date':date_mmddyyyy,'time':time_hhmmss,'temperature':self.t[1],'humidity':self.t[2],'pir':self.t[3],'ldr':self.t[4]}
        print self.apikey + str(data)
	result = requests.post(self.apikey, data=json.dumps(data))
            
    def send_msg(self,msg):
        self.conn.send(msg)

    def close(self):
        self.conn.close()

class connectionThread(threading.Thread):
    def __init__(self, host, port):
        super(connectionThread, self).__init__()
        try:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
            self.s.bind((host,port))
            self.s.listen(6)
        except socket.error as e:
            es=str(e)
            print es
            sys.exit()
        self.clients = []

    def run(self):
        while True:
            conn, address = self.s.accept()
            c = client(conn)
            c.start()
            c.send_msg(u"\r\n")
            self.clients.append(c)
            print '[+] Client connected: {0}'.format(address[0])


def main():
    print "lc $ Server started...."
    print "lc $ Press Ctrl+C to exit\nlc $ ...."
    get_conns = connectionThread(host, port)
    get_conns.start()
    while True:
        try:
            for c in get_conns.clients:
                c.run()

        except KeyboardInterrupt:
            print "Exiting..."
            os.system("sudo fuser -k 117/tcp")
            sys.exit()


if __name__ == '__main__':
    main()
