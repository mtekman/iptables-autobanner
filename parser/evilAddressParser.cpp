#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#define VERSION "v0.1"
//#define DEBUG

using namespace std;

typedef vector<string> list_of; // ports and user lists use sam defn.
typedef vector<list_of> list_pair; // vector[0] = ports, vector[1] = users
typedef map<string, list_pair > datamap;

datamap DMAP;

bool verbose = false;

void usage(){
    cerr << "[sudo] evilAddressParser [OPTIONS]" << VERSION << endl;
	cerr << endl;
    cerr << "Looks for failed password attempts in a journalctl report (or a generated one)" << endl;
	cerr << "Output is a sorted list of: unique_ip[TAB]ports_attempted[TAB]users_attempted" << endl;
    exit(-1);
}

int main (int argc, char ** argv)
{
	//time_t timev;
	//time(&timev)
	//system("mv evil_addresses.txt evil_addresses.
//    system("journalctl -u sshd --since=yesterday > evil_addresses.txt");
    ifstream infile("evil_addresses.txt");

	string line;
    while (getline(infile, line))
    {
        //Skip to user part
        int start_user = line.find("Failed password for")+20;
        if (start_user!=-1) line = line.substr(start_user, line.npos);
        else continue;

        //skip the 'invalid user' part if neccesary
        int invalid_user = line.find("invalid user");
        if (invalid_user!=string::npos) line = line.substr(invalid_user+13, line.npos);

        // Grab user string
        string user;
        int end_user = line.find(" from");
        if (end_user!=string::npos) {
            user = line.substr(0,end_user);
            line = line.substr(end_user+6, line.npos);
        }
       if (user.empty()) continue;

        string address;
        int start_port = line.find(" port");
        if (start_port!=string::npos) {
            address = line.substr(0,start_port);
            line = line.substr(start_port+6, line.npos);
        }

        if(address.size() < 5) continue;

        string port;
        int end_port = line.find(" ssh2");
        if (end_port!=string::npos) port = line.substr(0,end_port);

#ifdef DEBUG
        cout << user.c_str() << "@" << address.c_str() << ":" << port.c_str() << '#' << endl;
#endif

        // Begin mapping
        if (DMAP.find(address) == DMAP.end())
        {
            list_of ports;
            list_of users;
            list_pair v;
            v.push_back(ports);
            v.push_back(users);

            DMAP[address] = v;
        }

        // Found address
        else{
            list_pair &v = DMAP[address];

            // !! Find unique ports
            list_of::iterator ii;
            for (ii = v[0].begin(); ii!= v[0].end(); ++ii){
                string &retrieved_port = *ii;

                if (port == retrieved_port) break;
            }
            //Didn't find it, insert
            if (ii==v[0].end()) v[0].push_back(port);




            // !! Find unique users
            list_of::iterator jj;
            for (jj = v[1].begin(); jj!= v[1].end(); ++jj){
                string &retrieved_user = *jj;

                if (user == retrieved_user) break;
            }
            //Didn't find it, insert
            if (jj==v[1].end()) v[1].push_back(user);
        }
    }
    infile.close();


    // Print header
    if (verbose) cout << "Address\tPorts\tUsers" << endl;
    else cout << "Address\t#Ports\t#Users" << endl;
    cout << "=======\t=====\t=====" << endl;

    //Print Map
    for (datamap::const_iterator mm = DMAP.begin(); mm!=DMAP.end(); ++mm)
    {
        //Address
        cout << (mm->first).c_str() << '\t' << flush;

        list_pair v = mm->second;
        list_of &ports = v[0];
        list_of &users = v[1];

        //unique ports
        if (verbose) for (list_of::iterator ii = v[0].begin(); ii!= v[0].end(); ++ii) cout << (*ii).c_str() << ',' << flush;
        else cout << v[0].size() << flush;

        cout << '\t' << flush;

        //unique users
        if (verbose) for (list_of::iterator ii = v[1].begin(); ii!= v[1].end(); ++ii) cout << (*ii).c_str() << ',' << flush;
        else cout << v[1].size() << flush;

        cout << endl;
    }


	return 0;
}
