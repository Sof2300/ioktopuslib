

#include <dirent.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
//#include <sys/statvfs.h>

#include "CurFSx86.h"

//#include "printx86.h"

//#include "cppCompatUtils.h"

long getFileSize(std::string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

bool isDirectory(std::string filename)
{
	struct stat stat_buf;
//	int rc =
			stat(filename.c_str(), &stat_buf);
	return (stat_buf.st_mode & S_IFDIR);
//	return rc == 0 ? stat_buf.st_size : -1;
}


std::string stringreplaceAll(std::string src, std::string pattern,std::string newval){
	std::string::size_type n = 0;
	while ( ( n = src.find( pattern, n ) ) != std::string::npos )
	{
		src.replace( n, pattern.size(), newval );
		n += pattern.size();
	}
	return src;
}







std::string CurFSx86Prototype::pathToFinal(std::string path){
	std::string diskpath = programpath;
	path=stringreplaceAll(path,"/","\\");
	if(path.substr(0,1)=="\\") path=path.substr(1,path.size());
	diskpath+=path;
	return diskpath;
};

unsigned long CurFSx86Prototype::fileSize(std::string path) {
	long s=getFileSize(pathToFinal(path));
	if(s>=0) return s;
	else return 0;
	/*
		std::ifstream myfile(programpath+path, std::ios::in|std::ios::binary|std::ios::ate);
		std::size_t size = 0;
		if (myfile.is_open()){
			size = myfile.tellg();
			myfile.close();
			return size;
		} else return size;*/
};
bool CurFSx86Prototype::exists(std::string path) {
	std::string fpath=pathToFinal(path);
	if(getFileSize(fpath)>=0) return true;
	else return false;
	/*
 		std::ifstream myfile(programpath+path, std::ios::in|std::ios::binary|std::ios::ate);
		if (myfile.is_open()){
			myfile.close();
			return true;
		} else return false;*/
};

std::vector<FileFS> CurFSx86Prototype::listFilesByExt(std::string path0, std::string fileext){
	println(std::string()+"listFilesByExt:"+path0);
	std::string diskpath = pathToFinal(path0);
	std::vector<FileFS> v;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (diskpath.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			std::string fn=ent->d_name;
			if(fn.size()>fileext.size() && fn.substr(fn.size()-fileext.size())==fileext) {
				FileFS ffs;
				ffs.name=path0+fn;
				ffs.size=getFileSize(diskpath+"\\"+fn);
				v.push_back(ffs);
			}
			fn=diskpath+"\\"+fn;
			//		if(getFileSize(fn)>0) println(fn + " ("+to_string(getFileSize(fn))+")");
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("could not open directory");
		//  return EXIT_FAILURE;
	}

	return v;
};



std::vector<FileFS> CurFSx86Prototype::listFiles(std::string path0, std::string filenamebase){

	std::string diskpath = pathToFinal(path0);
//	println(std::string()+"listFiles diskpath:"+diskpath);
	std::vector<FileFS> v;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (diskpath.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			std::string fn=ent->d_name;
			if(fn.substr(0,filenamebase.size())==filenamebase) {
				FileFS ffs;
				ffs.name=path0+fn;
				ffs.size=getFileSize(diskpath+"\\"+fn);
				v.push_back(ffs);
			}
			fn=diskpath+"\\"+fn;
			//		if(getFileSize(fn)>0) println(fn + " ("+to_string(getFileSize(fn))+")");
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("could not open directory");
		//  return EXIT_FAILURE;
	}
	return v;
};


void CurFSx86Prototype::printFiles(std::string path0){
	if(path0.empty()) path0=programpath;
	std::vector<FileFS> vect=listFiles(path0,"");
	for(auto it:vect){
		std::string str;
//		if(isDirectory(path0+it.name )) str=it.name + " "+ path0+ " directory";		else str=it.name + " "+ path0+ " "+ to_string(it.size);
		if(isDirectory(path0+it.name )) str=it.name + " directory";
		else str=it.name + " "+ std::to_string(it.size)+" bytes";

		println(str);
	}
}


void CurFSx86Prototype::printInfo(){
		Serial.println("File system on board : LittleFS");
		unsigned int gt=getTotalSpace(),us=getUsedSpace(),as=gt-us;
		float peru=10000.0*us/gt, perf=10000.0*as/gt;
		Serial.println(String()+"- total space : "+gt+" bytes");
		Serial.println(String()+"- used space : "+us+" bytes "+"("+peru/100.0+"%)");
		Serial.println(String()+"- available space : "+as+" bytes ("+ (perf/100.0)+"%)");
		Serial.println(String()+"Files : ");
		printFiles();
	}
bool CurFSx86Prototype::erase(std::string path) {
	path = pathToFinal(path);
	if(exists(path)) {
		if(remove( path.c_str()) != 0 ) {println( std::string()+"Error deleting file :"+path );return false;}
		else { println(std::string()+ "File successfully deleted :"+path );return true;}
	}
	return false;
};
unsigned int CurFSx86Prototype::appendToFile(std::string path, std::vector<unsigned char> &vect) {
	std::string diskpath = pathToFinal(path);
	std::ofstream fout;
	fout.open(diskpath, std::ios_base::binary | std::ios::app);
	if(fout.is_open()) {
		fout.write((char *)vect.data(), vect.size());
		fout.close();
	} else std::cout << "CurFSx86Prototype::appendToFile: Failed to open file "<< diskpath<<" for write ! maybe the path does not exist ?" <<std::endl;

	return vect.size();
};
unsigned int CurFSx86Prototype::appendToFile(String path, String str){
	std::string s=str.c_str();
	std::vector<unsigned char>vect=std::vector<unsigned char>(s.begin(), s.end());
	unsigned int size= appendToFile(std::string(path.c_str()), vect);
	return size;
};

unsigned int CurFSx86Prototype::rewriteFile(std::string path, std::vector<unsigned char> &vect) {
	std::string diskpath =pathToFinal(path);
	std::ofstream fout;
	fout.open(diskpath, std::ios_base::binary);
	unsigned int i=0;
	if(fout.is_open()) {
		fout.write((char *)vect.data(), vect.size());
		i=vect.size();
		fout.close();
	}

	return i;
};

unsigned int CurFSx86Prototype::rewriteFile(std::string path, unsigned char *buff, unsigned int size) {
	std::string diskpath =pathToFinal(path);
	std::ofstream fout;
	fout.open(diskpath, std::ios_base::binary);
	unsigned int i=0;
	if(fout.is_open()) {
		fout.write((char *)buff, size);
		i=size;
		fout.close();
	}

	return i;
};

unsigned int CurFSx86Prototype::rewriteFile(String path, String content){
	return rewriteFile(path.c_str(),(unsigned char*)content.c_str(),content.length());
} ;
String CurFSx86Prototype::readFileToString(String path){
		path=pathToFinal(path.c_str()).c_str();
		FILE* f = fopen(path.c_str(), "rb" );
		if (!f) { Serial.println(String()+"readFileToString Error: could not open file '"+path+"'\n"); return String(); }
		fseek(f, 0, SEEK_END);
		size_t size = ftell(f);
		fseek(f, 0, SEEK_SET);
		unsigned char * buffer = new unsigned char[size+1];
		fread(buffer, sizeof(unsigned char), size, f);
		fclose(f);

		String str((const char *)buffer);
		delete buffer;				//		Serial.println(String()+"readFileToString 3 str:'"+str+"'\n");
		return str;
}

std::string CurFSx86Prototype::readFileToStdString(std::string path,size_t &size){	// size return file size
		path=pathToFinal(path);
		FILE* f = fopen(path.c_str(), "rb" );
		if (!f) { printf("readFileToStdString Error: could not open file %s\n", path.c_str()); return std::string(); }
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		fseek(f, 0, SEEK_SET);
		unsigned char * buffer = new unsigned char[size];
		//	int bytes =
		fread(buffer, sizeof(unsigned char), size, f);
		fclose(f);
			std::string str((const char *)buffer);
			delete buffer;
			return str;
		};

unsigned char* CurFSx86Prototype::readFileBuffer(std::string path,size_t &size){
	path=pathToFinal(path);
	FILE* f = fopen(path.c_str(), "rb" );
	if (!f) { printf("readFileBuffer Error: could not open file %s\n", path.c_str()); return 0; }
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	unsigned char * buffer = new unsigned char[size];
	//	int bytes =
	fread(buffer, sizeof(unsigned char), size, f);
	fclose(f);
	return buffer;
};


void cppprintBytes(unsigned char* pBytes, const uint32_t nBytes) {
	print("[");
	for ( uint32_t i = 0; i < nBytes; i++ ) {
		//  std::cout << std::hex << (unsigned int)( pBytes[ i ] );
		unsigned char c= pBytes[ i ];
		print((unsigned int)c);
		if((i+1)<nBytes) print(", ");
	}
	println("]");
};

template <typename... ParamTypes>
//	void setTimeOut(unsigned int milliseconds, bool repeat, bool **cancel, void (func)(ParamTypes...), ParamTypes... parames)
bool CurFSx86Prototype::readByChunk(std::string path, long chunksize,int backward, bool (func)(unsigned char*,ParamTypes...), ParamTypes... parames){
	std::string diskpath = pathToFinal(path);
	FILE *f = fopen(diskpath.c_str(), "r");
	if (!f) return false;
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned int left=fsize;
	if(chunksize>left) chunksize=left;
	unsigned char *buffer=(unsigned char *)malloc(chunksize);
	int err=-1;
	if(backward==1) err=fseek(f, -chunksize, SEEK_END);
	while(left>0){
		if(chunksize>left) chunksize=left;
		long pos=ftell(f);
		size_t res=fread(buffer, 1,chunksize, f);
		cppprintBytes(buffer,chunksize);
		bool b=func(buffer,parames...);
		if(!b) break;
		left-=chunksize;
		if(backward==0) fseek(f, chunksize, SEEK_CUR);
		else fseek(f, -chunksize, SEEK_CUR);
	}
	fclose(f);
	free (buffer);
	return true;
};



FileFS* CurFSx86Prototype::openFile(std::string path){
	std::string fpath=pathToFinal(path);
	FILE *f = fopen(fpath.c_str(), "rb");
	if (!f) return 0;
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	return new CurFileFS(path,fsize,f);
};
void CurFSx86Prototype::closeFile(FileFS* ffs){
	CurFileFS *cffs=(CurFileFS*) ffs;
	fclose(cffs->fileptr);
	delete cffs;
};
unsigned long CurFSx86Prototype::readFile(unsigned char *r,unsigned long size, FileFS* ffs){
	CurFileFS *cffs=(CurFileFS*) ffs;
	//		long pos = ftell(cffs->fileptr);
	//		println("readFile: reading at pos:"+to_string(pos));
	size_t res=fread(r, 1,size, cffs->fileptr);
	//		pos = ftell(cffs->fileptr);
	//		println("readFile: post reading at pos:"+to_string(pos));
	return res;
};
void CurFSx86Prototype::seekFile(unsigned long pos, FileFS* ffs){
	CurFileFS *cffs=(CurFileFS*) ffs;
	fseek(cffs->fileptr, pos, SEEK_SET);
};


