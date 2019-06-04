/*
 * @Description: 
 * @Author: cloudox
 * @Date: 2019-06-04 19:36:53
 * @LastEditTime: 2019-06-04 21:04:10
 */
#include <iostream>
#include <vector>
#include <sstream>
#include "HiredisHelper.h"

using namespace std;

// hiredis官方： https://github.com/redis/hiredis/

#define CHECK_FREE_REDIS_REPLY(reply) \
    if (reply) {\
        freeReplyObject(reply);\
    } else {\
        cout << "freeReplyObject Fail" << endl;\
    }

int addStrContent(HiredisHelper &redis_conn) {
    string m_redis_key = "cloudox";
    stringstream ss_cmd;
    ss_cmd << "SET " << m_redis_key;
    ss_cmd << " " << "boy";

    string cmd = ss_cmd.str()
    redisReply *reply = redis_conn.ExecuteCmd(cmd);
    if (reply) {
        cout << "push redis succ " << cmd << endl;
        return 0;
    } else {
        cout << "push redis fail " << cmd << endl;
        return -1;
    }
    CHECK_FREE_REDIS_REPLY(reply);
}

int getStrContent(HiredisHelper &redis_conn, string &item) {
    string m_redis_key = "cloudox";
    stringstream ss_cmd;
    ss_cmd << "GET " << m_redis_key;

    string cmd = ss_cmd.str()
    redisReply *reply = redis_conn.ExecuteCmd(cmd);
    if (reply && reply->type == REDIS_REPLY_STRING) {
        item = reply->str;
        return 0;
    } else {
        cout << "get redis fail " << cmd << endl;
        return -1;
    }
    CHECK_FREE_REDIS_REPLY(reply);
}

int addZsetContent(HiredisHelper &redis_conn) {
    string m_redis_key = "cloudox";
    stringstream ss_cmd;
    ss_cmd << "ZADD " << m_redis_key;
    ss_cmd << " " << "1" << " " << "boy";

    string cmd = ss_cmd.str()
    redisReply *reply = redis_conn.ExecuteCmd(cmd);
    if (reply) {
        freeReplyObject(reply);
        cout << "push redis succ " << cmd << endl;
        return 0;
    } else {
        cout << "push redis fail " << cmd << endl;
        return -1;
    }
    CHECK_FREE_REDIS_REPLY(reply);
}

int getZsetContent(HiredisHelper &redis_conn, vector<string> &items) {
    string m_redis_key = "cloudox";
    stringstream ss_cmd;
    ss_cmd << "ZRANGEBYSCORE " << m_redis_key << " -inf " << "5" << " limit 0 10";

    string cmd = ss_cmd.str()
    redisReply *reply = redis_conn.ExecuteCmd(cmd);
    if (reply && reply->type == REDIS_REPLY_ARRAY) {
        cout << "reply size:" << reply->elements << endl;
        for (size_t i = 0; i < reply->elements; ++i) {
            if (NULL != reply->element[i]) {
                redisReply *ele = reply->element[i];
                if (ele->type == REDIS_REPLY_INTEGER) {
                    items.push_back(to_string(ele->integer));// C++11
                } else if (ele->type == REDIS_REPLY_STRING) {
                    string s(ele->str, ele->len);
                    items.push_back(s);
                }
            }
        }

        return 0;
    } else {
        cout << "get redis fail " << cmd << endl;
        return -1;
    }
    CHECK_FREE_REDIS_REPLY(reply);
}

int main() {

    HiredisHelper redis_conn;
    string redis_ip = "172.20.0.4";
    int redis_port = 6379;
    string redis_auth = "crs-fsdfjlskdjflsajdlfas";
    redis_conn.Init(redis_ip, redis_port, redis_auth);

    // 添加字符串内容
    int ret = addStrContent(redis_conn);

    // 获取字符串内容
    string item;
    ret = getStrContent(redis_conn, item);
    cout << item << endl;


    // 添加有序集合内容
    ret = addZsetContent(redis_conn);

    // 获取有序集合内容
    vector<string> items;
    ret = getZsetContent(redis_conn, items);
    if (ret == 0) {
        for (int i = 0; i < items.size(); ++i) {
            cout << items[i] << endl;
        }
    }

    system("pause");// 暂停以显示终端窗口
    return 0;
}