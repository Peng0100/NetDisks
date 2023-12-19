#include "database.h"

Database::Database() 
    :conn(mysql_init(NULL))
{
    if (!mysql_real_connect(conn, HOST, USER, PASSWORD, DB_NAME, 0, NULL, 0)) {
        cerr << mysql_error(conn) << endl;
    }
}

Database::~Database() {
    if (conn)
        mysql_close(conn);
}

pair<void*, size_t> Database::Log1(void *data) {
    return __Ls(0, *static_cast<int*>(data));
}

pair<void*, size_t> Database::Ls(void *data) {
    int curr_pre_all_files_id = *static_cast<int*>(data);
    int user_id = *reinterpret_cast<int*>((char*)data + 4);
    return __Ls(curr_pre_all_files_id, user_id);
}

pair<void*, size_t> Database::ResultPack() {
    auto *rows = mysql_store_result(conn);
    size_t len = 0;
    for (auto *row = mysql_fetch_row(rows); row; row = mysql_fetch_row(rows)) {
        for (int i = 0; i < mysql_num_fields(rows); ++i) {
            len = len + strlen(row[i]) + 1;
        }
    }

    void *result = malloc(len + 2 * sizeof(size_t));
    len = 0;
    len = mysql_num_rows(rows);
    memcpy(result, &len, sizeof(size_t));
    len = mysql_num_fields(rows);
    memcpy((char*)result + sizeof(size_t), &len, sizeof(size_t));
    len = sizeof(size_t) * 2;

    for (auto *row = mysql_fetch_row(rows); row; row = mysql_fetch_row(rows)) {
        for (int i = 0; i < mysql_num_fields(rows); ++i) {
            memcpy((char*)result + len, row[i], strlen(row[i]) + 1);
            len += strlen(row[i] + 1);
        }
    }

    mysql_free_result(rows);
    return make_pair(result, len);
}


pair<void*, size_t> Database::__Ls(int pre_all_files_id, int user_id) {
    string query("select file_name from all_files where pre_all_files_id=" + 
            to_string(pre_all_files_id) + " and user_id=" + to_string(user_id));
    int ret = mysql_query(conn, query.c_str());
    if (ret) {
        cerr << mysql_error(conn) << endl;
        return make_pair(nullptr, 0);
    }

    return ResultPack();
}

