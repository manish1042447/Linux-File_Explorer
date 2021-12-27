#include <bits/stdc++.h>
#include <filesystem>
#include <sys/stat.h>
#include<pwd.h>
#include <grp.h>
#include <termios.h>
#include <sys/ioctl.h>
using namespace std;
using namespace std::filesystem;

string perms_linux(perms p){
	ostringstream ss;
	ss << ((p & perms::owner_read) != perms::none ? "r" : "-")
		<< ((p & perms::owner_write) != perms::none ? "w" : "-")
		<< ((p & perms::owner_exec) != perms::none ? "x" : "-")
		<< ((p & perms::group_read) != perms::none ? "r" : "-")
		<< ((p & perms::group_write) != perms::none ? "w" : "-")
		<< ((p & perms::group_exec) != perms::none ? "x" : "-")
		<< ((p & perms::others_read) != perms::none ? "r" : "-")
		<< ((p & perms::others_write) != perms::none ? "w" : "-")
		<< ((p & perms::others_exec) != perms::none ? "x" : "-");
	return ss.str();
}

string file_size(double size) {
	ostringstream ss;
	string PreFix = "B";
	if (size > 1000) {
		size /= 1000;
		PreFix = "KB";
	} if (size > 1000) {
		size /= 1000;
		PreFix = "MB";
	} if (size > 1000) {
		size /= 1000;
		PreFix = "GB";
	} if (size > 1000) {
		size /= 1000;
		PreFix = "TB";
	}
	ss.precision(2);
	ss << fixed << size << " " << PreFix;
	return ss.str();
}

void print_dirent(directory_entry const& d,bool color=false){
        ostringstream out;
        string filename=d.path().filename().c_str();
        filename=(filename.size()>24)?(filename.substr(0,21)+".. "):filename;
		out << filename<< string(24 - out.str().length(), ' ');

		if(!d.is_directory()) out << file_size(d.file_size());
		out << string(40 - out.str().length(), ' ');

		out << ((d.is_directory()) ? 'd' : '-');

		out << perms_linux(d.status().permissions())<< string(56 - out.str().length(), ' ');

		struct stat fileStat;
        stat(d.path().filename().c_str(), &fileStat);
        //Unsigned It type User ID to user name and Group ID to group name
        struct passwd *pw = getpwuid(fileStat.st_uid);
		out << pw->pw_name << string(72 - out.str().length(), ' ');
		struct group *gp = getgrgid(fileStat.st_gid);
		out << gp->gr_name << string(88 - out.str().length(), ' ');

		out << ctime(&fileStat.st_mtime);

		string entry=out.str();
		if(color)entry="\033[1;31;106m"+entry+"\033[0m";
		cout << entry;
}
void scrollDir(vector<directory_entry> &dir, int first,int last,int curr){
    struct termios initialrsettings, newrsettings;
    tcgetattr(fileno(stdin), &initialrsettings);
    //switch to canonical mode and echo mode
    newrsettings = initialrsettings;
    newrsettings.c_lflag &= ~ICANON;
	newrsettings.c_lflag &= ~ECHO;
	tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings);
    while(true){
        cout << "\033[2J\033[1;1H";
        ostringstream out;
        out << "File Name\t\tFile Size\tPermission\tUserID\t\tGroupID\t\tLast Modified\n" ;
	    out << string(112,'-');
	    cout << out.str() << "\n";
        for (int i=first;i<last;i++){
        if(i==curr)print_dirent(dir[i],true);
        else print_dirent(dir[i]);
        }

        if ((cin.get()==27)){
            cin.get();
            char c=cin.get();
            if (c=='A') {
                curr=curr-1;
                if(curr<first){
                    if(curr>=0){
                        first--;
                        last--;
                    }else{
                        last=dir.size();
                        curr=last-1;
                        first=last-25;
                        if(first<0)first=0;
                    }
                }
            }
            else if (c=='B') {
                curr=curr+1;
                if(curr>=last){
                    if(last<dir.size()){
                            last++;
                            first++;
                    }
                    else {
                        curr=0;
                        first=0;
                        last=dir.size();
                        if(last>25)last=25;
                    }
                }
            }
            //if (e==67) { cout << "RIGHT";}
            //if (e==68) { cout << "LEFT";}
        }
        else break;
    }
    tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);
}
void printDir(path p){
	directory_iterator d_itr(p);
	vector<directory_entry> dir({directory_entry("."),directory_entry("..")});
	for (auto e : d_itr)
		dir.push_back(e);
    int first=0,last=dir.size();
    if(last>25)last=25;
    int curr=1;
    scrollDir(dir,first,last,curr);
}