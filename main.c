#include <crypt.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>


using namespace std;

int uid = 100;
int guid = 1010;


  const char *const seedchars =
    "./0123456789ABCDEFGHIJKLMNOPQRST"
    "UVWXYZabcdefghijklmnopqrstuvwxyz";

void addUser(const char user[], const char password[])
{
    int todayDays = (time(NULL))/(60*60*24);

    char salt[] = "$1$........";
    unsigned long seed[2];
    /* Generate a (not very) random seed.
       You should do it better than this... */
    seed[0] = time(NULL);
    seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);

    /* Turn it into printable characters from ‘seedchars’. */
    for (int i = 0; i < 8; i++)
        salt[3+i] = seedchars[(seed[i/5] >> (i%5)*6) & 0x3f];

    stringstream passwdLine;
    stringstream shadowLine;
    passwdLine << user << ":x:" << uid++ << ":" << guid << ":" << user << "_RealName:userDir:" << "/bin/bash" << "\n";
    shadowLine << user << ":" << crypt(password, salt) << ":" << todayDays << ":0:99999:7:::" << "\n";
    ofstream passwdFile ("./passwd.txt", ios::out | ios::app);
    ofstream shadowFile ("./shadow.txt", ios::out | ios::app);

    passwdFile << passwdLine.str();
    shadowFile <<  shadowLine.str();
    passwdFile.close();
    shadowFile.close();
}

int main(int argc, char* argv[])
{
    ifstream myfile;
    string line;
    myfile.open("users.txt");
    remove("shadow.txt");
    remove("passwd.txt");
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            int delimeter = line.find(' ');
            addUser(line.substr(0,delimeter).c_str(), line.substr(delimeter+1,line.length()).c_str());
        }
    }
    for(int i = 1; i < argc; i += 2)
    {
        addUser(argv[i], argv[i + 1]);
    }
}

