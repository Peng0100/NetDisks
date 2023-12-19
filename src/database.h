#ifndef DATABASE_H
#define DATABASE_H

#include "header.h"

#include <mysql/mysql.h>

#define HOST "localhost"
#define USER "debian-sys-maint"
#define PASSWORD "ay08ZxT5Z938wWnk"
#define DB_NAME "yunpan"

class Database {
public:
    Database();
    ~Database();

    pair<void*, size_t> Log1(void *data);
    pair<void*, size_t> Ls(void *data);

private:
    pair<void*, size_t> ResultPack();

    pair<void*, size_t> __Ls(int pre_all_files_id, int user_id);

    MYSQL *conn;
};

#endif
