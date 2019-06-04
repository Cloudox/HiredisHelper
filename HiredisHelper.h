/*
 * @Description: 
 * @Author: cloudox
 * @Date: 2019-06-04 19:36:19
 * @LastEditTime: 2019-06-04 20:35:29
 */


#ifndef HIREDISHELPER_H_
#define HIREDISHELPER_H_

#include "hiredis/include/hiredis.h"
#include <string>
#include <vector>

using namespace std;

class HiredisHelper {
public:
	void Init(const string &ip, int port, const string &auth_str,int timeout = 200) { //ms
		m_ip = ip;
		m_port = port;
		m_timeout = timeout;
		m_ctx = NULL;
		m_authstring = auth_str;
	}

	virtual ~HiredisHelper() {
		if (m_ctx) {
			redisFree(m_ctx);
			m_ctx = NULL;
		}
	}

	//调用者需自行调用  freeReplyObject(reply);
    redisReply *ExecuteCmd(const char *format, ...);
	redisReply* ExecuteCmd(const string &cmd);
private:
	int Connect();

private:
    int m_timeout;
    int m_port;
    string m_ip;
    string m_authstring;
    redisContext* m_ctx;
};



#endif /* HIREDISHELPER_H_ */
