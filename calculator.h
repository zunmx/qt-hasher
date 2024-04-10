#include <QString>
#include <QFile>
#include <QCryptographicHash>
#include <map>

#ifndef CALCULATOR_H
#define CALCULATOR_H

extern int calc_number;
extern std::map<QString ,std::map<QString ,double>> process;

const unsigned int pre_size = 1024*1024;
QString GetCRC32(QString fileName,int col);
QString GetMD4(QString fileName,int col);
QString GetMD5 ( QString fileName ,int col);
QString GetSHA1(QString fileName,int col);
QString GetSHA256(QString fileName,int col);
QString GetSHA384(QString fileName,int col);
QString GetSHA512(QString fileName,int col);

#endif // CALCULATOR_H

