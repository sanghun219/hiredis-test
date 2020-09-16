#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <WinSock2.h>
#include "hiredis.h"

class RedisManager {
public:
	RedisManager(const int port, const char* ip) {
		m_HostPort = port;
		m_HostIP = const_cast<char*>(ip);
		m_Context = nullptr;
		m_RedisReply = nullptr;
	}

	void Connect() {
		struct timeval tv { 1, 500000 };
		m_Context = redisConnectWithTimeout(m_HostIP, m_HostPort, tv);
		if (m_Context == NULL || m_Context->err) {
			if (m_Context) {
				printf("Connection error: %s\n", m_Context->errstr);
				redisFree(m_Context);
			}
			else {
				printf("Connection error: can't allocate redis context\n");
			}
			exit(1);
		}
	}

	BOOL Request(std::string query_) {
		const char* query = query_.c_str();
		m_RedisReply = (redisReply*)redisCommand(m_Context, query);

		return TRUE;
	}

	BOOL Request(const char* query_, ...) {
		char buf[512];
		va_list ap;
		va_start(ap, query_);
		vsprintf_s(buf, query_, ap);
		va_end(ap);

		m_RedisReply = (redisReply*)redisCommand(m_Context, query_);

		return TRUE;
	}

	redisReply* GetRedisReply() {
		if (m_RedisReply == nullptr) {
			std::cout << "Request를 먼저 하십셔!" << std::endl;
			return nullptr;
		}
		return m_RedisReply;
	}

	void Close() {
		freeReplyObject(m_RedisReply);
		redisFree(m_Context);
	}
	~RedisManager() {
		if (m_RedisReply != nullptr || m_Context != nullptr)
			Close();
	}
private:
	redisContext* m_Context;
	UINT16 m_HostPort;
	redisReply* m_RedisReply;
	char* m_HostIP;
};