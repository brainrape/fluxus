// Copyright (C) 2004 David Griffiths <dave@pawfal.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <string>
#include <lo/lo.h>
#include <set>
#include <map>
#include <vector>
#include <pthread.h>

using namespace std;

class OSCData
{
	public:
	virtual ~OSCData() {}
	virtual char Type() { return '0'; }
};

class OSCNumber : public OSCData
{
	public:
	OSCNumber(float s) { Value=s; }
	virtual ~OSCNumber() {}
	virtual char Type() { return 'n'; }
	float Value;
};

class OSCString : public OSCData
{
	public:
	OSCString(const string &s) { Value=s; }
	virtual ~OSCString() {}
	virtual char Type() { return 's'; }
	string Value;
};

class Client
{
public:
	Client() {}
	~Client() {}
	void SetDestination(const string &Port) { m_Destination=Port; }	
	void Send(const string &msg,const vector<OSCData*> &args);
	
private:
	string m_Destination;
	
};

class Server
{
public:
	Server(const string &Port);
	~Server();
	
	void Run();
	bool SetMsg(const string &name);
	bool GetArgs(vector<OSCData*> &args);
	string GetLastMsg() { return m_LastMsg; }
	void ClearHistory();

private:

	static int DefaultHandler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);
	static void ErrorHandler(int num, const char *m, const char *path);	
	
	static map<string,vector<OSCData*> > m_Map;
	static string m_LastMsg;
	static set<string> m_MessageHistory;
	
	lo_server_thread m_Server;
	static bool m_Exit;
	static pthread_mutex_t* m_Mutex;
	string m_CurrentOSCMsg;
};