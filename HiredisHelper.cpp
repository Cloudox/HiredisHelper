/*
 * @Description: 
 * @Author: cloudox
 * @Date: 2019-06-04 19:36:19
 * @LastEditTime: 2019-06-04 20:35:33
 */

#include "HiredisHelper.h"

int HiredisHelper::Connect() {
    struct timeval tv;
    tv.tv_sec = m_timeout / 1000;
    tv.tv_usec = (m_timeout % 1000) * 1000;

    if (m_ctx != NULL) {
    	redisFree(m_ctx);
    	m_ctx = NULL;
    }

    m_ctx = redisConnectWithTimeout(m_ip.c_str(), m_port, tv);
    if (m_ctx == NULL || m_ctx->err != 0) {
        if (m_ctx != NULL) {
        	redisFree(m_ctx);
        	m_ctx = NULL;
        }

        cout << "redisConnect fail" << endl;
        return REDIS_ERR;
    }

    if (REDIS_ERR == redisSetTimeout(m_ctx, tv)){
    	redisFree(m_ctx);
    	m_ctx = NULL;
    	return REDIS_ERR;
    }
//     cout << "redis Connect success" << endl;

    string auth = "AUTH "+ m_authstring;

	redisReply *reply = (redisReply*)redisCommand(m_ctx, auth.c_str());

	if (reply == NULL) {
		if (m_ctx) {
			redisFree(m_ctx);
			m_ctx = NULL;
		}
		return REDIS_ERR;
	}

	if (m_ctx->err != 0) {
		cout << "err:"<<m_ctx->err << " errstr " << m_ctx->errstr << endl;
		redisFree(m_ctx);
		m_ctx = NULL;
		if (reply) {
			freeReplyObject(reply);
		}
		return REDIS_ERR;
	}

	if (REDIS_REPLY_ERROR == reply->type) {
		cout << "cmd:" << auth << " errstr " << reply->str << endl;
		if (m_ctx) {
			redisFree(m_ctx);
			m_ctx = NULL;
		}
		freeReplyObject(reply);
		return REDIS_ERR;
	}

	freeReplyObject(reply);
// 	cout << "redis Auth success" << endl;
    return 0;
}

redisReply* HiredisHelper::ExecuteCmd(const string &cmd) {
	int ret = 0;
	if (m_ctx == NULL) {
		ret = Connect();
		if (ret < 0) {
			return NULL;
		}
	}

	redisReply *reply = (redisReply*)redisCommand(m_ctx, cmd.c_str());

	if (reply == NULL) {
    	if (m_ctx) {
            cout << "err:" << m_ctx->err << " errstr " << m_ctx->errstr << endl;
            if (m_ctx->err == 3) {
                redisFree(m_ctx);
                m_ctx = NULL;
                // Server closed the connection, reconnect
                ret = Connect();
                if (ret == 0) {
                    cout << "reconnect and ex cmd" << endl;
                    reply = (redisReply*)redisCommand(m_ctx, cmd.c_str());
                    if (reply == NULL) {
                        if (m_ctx) {
                            cout << "err:" << m_ctx->err << " errstr " << m_ctx->errstr << endl;
                            redisFree(m_ctx);
                            m_ctx = NULL;
                        }
                    }
                }
            } else {
                redisFree(m_ctx);
                m_ctx = NULL;
            }
    	}

        if (reply == NULL || m_ctx == NULL) {
    	    return NULL;
        }
	}

    if (m_ctx->err != 0) {
    	cout << "err:" << m_ctx->err <<" errstr "<<m_ctx->errstr << endl;
    	redisFree(m_ctx);
    	m_ctx = NULL;

    	if (reply) {
    		freeReplyObject(reply);
    	}
    	return NULL;
    }

    if (REDIS_REPLY_ERROR == reply->type) {
    	cout << "cmd:" << cmd <<" errstr "<<reply->str << endl;

    	if (m_ctx) {
        	redisFree(m_ctx);
        	m_ctx = NULL;
    	}
    	freeReplyObject(reply);
    	return NULL;
    }

// 	cout << "redisCommand execute cmd:"<<cmd<<" success" << endl;
	return reply;
}

redisReply * HiredisHelper::ExecuteCmd(const char *format, ...) {
    int ret = 0;
    if (m_ctx == NULL) {
        ret = Connect();
        if (ret < 0) {
            return NULL;
        }
    }

    va_list valist;
    va_start(valist, format);
    redisReply *reply = (redisReply*)redisCommand(m_ctx, format, valist);

    if (reply == NULL) {
        if (m_ctx) {
            cout << "cmd :" << format << " err:" << m_ctx->err << " errstr " << m_ctx->errstr << endl;
            redisFree(m_ctx);
            m_ctx = NULL;
        }
        return NULL;
    }

    va_end(valist);

    if (m_ctx->err != 0) {
        cout << "err:" << m_ctx->err << " errstr " << m_ctx->errstr << endl;
        redisFree(m_ctx);
        m_ctx = NULL;

        if (reply) {
            freeReplyObject(reply);
        }
        return NULL;
    }

    if (REDIS_REPLY_ERROR == reply->type) {
        cout << "errstr: " << reply->str << endl;

        if (m_ctx) {
            redisFree(m_ctx);
            m_ctx = NULL;
        }
        freeReplyObject(reply);
        return NULL;
    }

//     cout << "redisCommand execute success" << endl;
    return reply;
}
