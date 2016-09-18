
/*
 * lib.c
 *
 *  Created on: Sep 8, 2012
 *      Author: Lars Mortensen
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <lib.h>

/**
 * Udskriver fejlmeddelelse og stoppe programmet
 *
 * @param msg Fejlmeddelelse
 */
void error(const std::string msg)
{
    perror(msg.c_str());
    exit(0);
}

/**
 * Udtrækker et filnavn fra en tekststreng
 *
 * @param fileName Filnavn incl. evt. stinavn
 * @return Kun filnavn
 */
const std::string extractFileName(const std::string fileName)
{
	return fileName.substr(fileName.find_last_of("/\\")+1);
}

/**
 * Læser en tekststreng fra en socket
 *
 * @param inFromServer stream that holds data from server
 * @return En streng modtaget fra en socket
 */
const std::string readTextTCP(std::string inText, int inFromServer)
{
    char ch;

    read(inFromServer, &ch, 1);
    while(ch != 0)
    {
   		inText += ch;
        read(inFromServer, &ch, 1);
    }
    return inText;
}

/**
 * Skriver en tekststreng til en socket med 0 terminering
 *
 * @param outToServer Stream hvortil der skrives data til socket
 * @param line Teksten der skal skrives til socket
 */
void writeTextTCP(std::string line, int outToServer)
{
	write(outToServer, line.c_str(), line.length());
	write(outToServer, "\0", 1);
}

/**
 * Konverter en streng som indeholder filstÃ¸rrelsen in ascii format til en talvÃ¦rdi
 *
 * @param inFromServer Stream som indeholder data fra socket
 * @return Filstørrelsen som talværdi
 */
const long getFileSizeTCP(int inFromServer)
{
	std::string fileName = "";
   	return atol(readTextTCP(fileName, inFromServer).c_str());
}

/**
 * Se om filen fileName eksisterer
 *
 * @param fileName Filnavn
 * @return Størrelsen på filen, 0 hvis den ikke findes
 */
const long check_File_Exists(std::string fileName)
{
	struct stat sts;
	if ((stat (fileName.c_str(), &sts)) == -1)
		return 0;

	return sts.st_size;
}
