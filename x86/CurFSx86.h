#ifndef CURFSx86_H
#define CURFSx86_H

#include <string>
#include <vector>

#include "printx86.h"

struct FileFS {
public:
	std::string name;
	unsigned int size;

};
struct CurFileFS : public FileFS
{
	FILE *fileptr;
	CurFileFS(std::string name0, unsigned int size0, FILE *fileptr0): fileptr(fileptr0) {name=name0; size=size0;};
};


static
class CurFSx86Prototype {
public:
	std::string programpath = ".\\";//C:\\Users\\Shpok\\Documents\\Arduino\\AllLogger\\data\\";
	CurFSx86Prototype(){};
	~CurFSx86Prototype(){};

	void begin(){}
	std::string pathToFinal(std::string path);

	unsigned long fileSize(std::string path) ;

	bool exists(std::string path) ;

	std::vector<FileFS> listFilesByExt(std::string path0, std::string fileext);
	std::vector<FileFS> listFiles(std::string path0, std::string filenamebase);
	void printFiles(std::string path0="");

	bool erase(std::string path) ;
	bool eraseAllFiles() {return false;};	// not implemented.. yet ?
	unsigned int appendToFile(std::string path, std::vector<unsigned char> &vect);
	unsigned int appendToFile(String path, String str);
	unsigned int rewriteFile(std::string path, std::vector<unsigned char> &vect) ;
	unsigned int rewriteFile(std::string path, unsigned char *buff, unsigned int size) ;
	unsigned int rewriteFile(String path, String content) ;

	unsigned char* readFileBuffer(std::string path,size_t &size);
	std::string readFileToStdString(std::string path,size_t &size);
	String readFileToString(String path);

	unsigned int getTotalSpace(){return 999999999;}	// max value of unsigned int is 4 giga, maybe should switch to uint64_t ?
	unsigned int getUsedSpace(){return 9;}
	unsigned int getFreeSpace(){return (getTotalSpace()-getUsedSpace());}

	void printInfo();

	template <typename... ParamTypes>
	//	void setTimeOut(unsigned int milliseconds, bool repeat, bool **cancel, void (func)(ParamTypes...), ParamTypes... parames)
	bool readByChunk( std::string path, long chunksize,int backward, bool (func)(unsigned char*,ParamTypes...), ParamTypes... parames);

	FileFS* openFile(std::string path);
	void closeFile(FileFS* ffs);
	unsigned long readFile(unsigned char *r,unsigned long size,FileFS* ffs);
	void seekFile(unsigned long pos, FileFS* ffs);

} CurFSx86; // static instance of CURFS

//#include "CurFSx86.cc"

#endif
