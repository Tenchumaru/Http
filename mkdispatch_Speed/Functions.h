#pragma once

using xstring = std::pair<char const*, char const*>;

class TcpSocket {
public:
};

class Response {
public:
};

class ClosableResponse : public Response {
public:
	ClosableResponse(TcpSocket& /*socket*/, char* /*begin*/, char* /*end*/) {}
};

class QueryBase {
public:
	virtual bool CollectQueryName(char const*& p, xstring*& q) {
		while(*p != '=' && *p != '&' && *p != ' ' && *p != '#' && *p != '\r' && *p != '\n') {
			++p;
		}
		if(*p == '=') {
			++p;
			q = nullptr;
			return true;
		}
		return false;
	}

	void CollectQueryValue(char const*& p, xstring& q) {
		q.first = p;
		while(*p != '&' && *p != ' ' && *p != '#' && *p != '\r' && *p != '\n') {
			++p;
		}
		q.second = p;
		if(*p == '&') {
			++p;
		}
	}

	bool CollectQueries(char const*& p, int offset) {
		p += offset;

		// Check for early termination cases.
		if(*p == '#') {
			do {
				++p;
			} while(*p != ' ' && *p != '\r' && *p != '\n');
		}
		if(*p == ' ') {
			return true;
		}
		if(*p != '?') {
			return false;
		}
		++p;

		// Loop, expecting name-value pairs.
		xstring* q;
		while(CollectQueryName(p, q)) {
			if(q) {
				CollectQueryValue(p, *q);
			} else {
				while(*p != '&' && *p != ' ' && *p != '#' && *p != '\r' && *p != '\n') {
					++p;
				}
				if(*p == '&') {
					++p;
				} else {
					break;
				}
			}
		}

		// Ignore any fragment.
		if(*p == '#') {
			do {
				++p;
			} while(*p != ' ' && *p != '\r' && *p != '\n');
		}
		return *p == ' ';
	}
};

class HeaderBase {
public:
	virtual bool CollectHeaderName(char const*& p, xstring*& q) {
		while(*p != ':' && *p != '\r' && *p != '\n') {
			++p;
		}
		if(*p == ':') {
			++p;
			q = nullptr;
			return true;
		}
		return false;
	}

	void CollectHeaderValue(char const*& p, xstring& q) {
		while(*p == ' ' || *p == '\t') {
			++p;
		}
		q.first = p;
		while(*p != '\r' && *p != '\n') {
			++p;
		}
		q.second = p;
		while(q.second[-1] == ' ' || q.second[-1] == '\t' && q.first < q.second) {
			--q.second;
		}
		if(*p == '\r') {
			++p;
		}
		if(*p == '\n') {
			++p;
		}
	}

	bool CollectHeaders(char const*& p) {
		// Parse the HTTP version from the rest of the request line.
		if(memcmp(p, " HTTP/", 6)) {
			return false;
		}
		p += 6;
		if(!std::isdigit(*p, std::locale()) || p[1] != '.' || !std::isdigit(p[2], std::locale())) {
			return false;
		}
		majorVersion = *p - '0';
		minorVersion = p[2] - '0';
		p += 3;
		if(*p == '\r') {
			++p;
		}
		if(*p != '\n') {
			return false;
		}
		++p;

		// Check for early termination cases.
		if(*p == '\r' || *p == '\n') {
			return true;
		}

		// Loop, expecting name-value pairs.
		xstring* q;
		while(CollectHeaderName(p, q)) {
			if(q) {
				CollectHeaderValue(p, *q);
			} else {
				while(*p != '\r' && *p != '\n') {
					++p;
				}
				if(*p == '\r') {
					++p;
				}
				if(*p == '\n') {
					++p;
				}
				if(*p == '\r' || *p == '\n') {
					break;
				}
			}
		}

		if(*p == '\r') {
			++p;
		}
		if(*p == '\n') {
			++p;
		}
		return true;
	}

private:
	unsigned short majorVersion, minorVersion;
};

inline bool AtEndOfPath(char ch) {
	return std::isspace(ch, std::locale()) || ch == '?' || ch == '#';
}

extern uint64_t DELETE_extensions__clientId_auth_secret_count;
void DELETE_extensions__clientId_auth_secret(char const* p0, char const* q0, Response& response);

extern uint64_t GET_extensions__clientId_auth_secret_count;
void GET_extensions__clientId_auth_secret(char const* p0, char const* q0, Response& response);

extern uint64_t GET_extensions__clientId_configurations_channels__channelId_count;
void GET_extensions__clientId_configurations_channels__channelId(char const* p0, char const* q0, char const* p1, char const* q1, Response& response);

extern uint64_t GET_extensions__clientId_configurations_segments_broadcaster_count;
void GET_extensions__clientId_configurations_segments_broadcaster(char const* p0, char const* q0, Response& response);

extern uint64_t GET_extensions__clientId_configurations_segments_developer_count;
void GET_extensions__clientId_configurations_segments_developer(char const* p0, char const* q0, Response& response);

extern uint64_t GET_extensions__clientId_configurations_segments_global_count;
void GET_extensions__clientId_configurations_segments_global(char const* p0, char const* q0, Response& response);

extern uint64_t GET_extensions__clientId_live_activated_channels_count;
void GET_extensions__clientId_live_activated_channels(char const* p0, char const* q0, Response& response);

extern uint64_t GET_helix_analytics_extensions_count;
void GET_helix_analytics_extensions(Response& response);

extern uint64_t GET_helix_analytics_games_count;
void GET_helix_analytics_games(Response& response);

extern uint64_t GET_helix_bits_leaderboard_count;
void GET_helix_bits_leaderboard(Response& response);

extern uint64_t GET_helix_clips_count;
void GET_helix_clips(Response& response);

extern uint64_t GET_helix_entitlements_codes_count;
void GET_helix_entitlements_codes(Response& response);

extern uint64_t GET_helix_games_top_count;
void GET_helix_games_top(Response& response);

extern uint64_t GET_helix_games_count;
void GET_helix_games(Response& response);

extern uint64_t GET_helix_streams_markers_count;
void GET_helix_streams_markers(Response& response);

extern uint64_t GET_helix_streams_metadata_count;
void GET_helix_streams_metadata(Response& response);

extern uint64_t GET_helix_streams_count;
void GET_helix_streams(Response& response);

extern uint64_t GET_helix_users_extensions_list_count;
void GET_helix_users_extensions_list(Response& response);

extern uint64_t GET_helix_users_extensions_count;
void GET_helix_users_extensions(Response& response);

extern uint64_t GET_helix_users_follows_count;
void GET_helix_users_follows(Response& response);

extern uint64_t GET_helix_users_count;
void GET_helix_users(Response& response);

extern uint64_t GET_helix_videos_count;
void GET_helix_videos(Response& response);

extern uint64_t GET_helix_webhooks_subscriptions_count;
void GET_helix_webhooks_subscriptions(Response& response);

extern uint64_t POST_extensions__clientId__version_channels__channelId_chat_count;
void POST_extensions__clientId__version_channels__channelId_chat(char const* p0, char const* q0, char const* p1, char const* q1, char const* p2, char const* q2, Response& response);

extern uint64_t POST_extensions__clientId_auth_secret_count;
void POST_extensions__clientId_auth_secret(char const* p0, char const* q0, Response& response);

extern uint64_t POST_extensions_message__channelId_count;
void POST_extensions_message__channelId(char const* p0, char const* q0, Response& response);

extern uint64_t POST_extensions_message_all_count;
void POST_extensions_message_all(Response& response);

extern uint64_t POST_helix_clips_count;
void POST_helix_clips(Response& response);

extern uint64_t POST_helix_entitlements_codes_count;
void POST_helix_entitlements_codes(Response& response);

extern uint64_t POST_helix_entitlements_upload_count;
void POST_helix_entitlements_upload(Response& response);

extern uint64_t POST_helix_streams_markers_count;
void POST_helix_streams_markers(Response& response);

extern uint64_t PUT_extensions__clientId__version_required_configuration_count;
void PUT_extensions__clientId__version_required_configuration(char const* p0, char const* q0, char const* p1, char const* q1, Response& response);

extern uint64_t PUT_extensions__clientId_configurations_count;
void PUT_extensions__clientId_configurations(char const* p0, char const* q0, Response& response);

extern uint64_t PUT_helix_users_extensions_count;
void PUT_helix_users_extensions(Response& response);

extern uint64_t PUT_helix_users_count;
void PUT_helix_users(Response& response);

using ptr_t = char const*;

extern ptr_t DELETE_extensions__clientId_auth_secret_p0, DELETE_extensions__clientId_auth_secret_q0;
extern ptr_t GET_extensions__clientId_auth_secret_p0, GET_extensions__clientId_auth_secret_q0;
extern ptr_t GET_extensions__clientId_configurations_channels__channelId_p0, GET_extensions__clientId_configurations_channels__channelId_q0, GET_extensions__clientId_configurations_channels__channelId_p1, GET_extensions__clientId_configurations_channels__channelId_q1;
extern ptr_t GET_extensions__clientId_configurations_segments_broadcaster_p0, GET_extensions__clientId_configurations_segments_broadcaster_q0;
extern ptr_t GET_extensions__clientId_configurations_segments_developer_p0, GET_extensions__clientId_configurations_segments_developer_q0;
extern ptr_t GET_extensions__clientId_configurations_segments_global_p0, GET_extensions__clientId_configurations_segments_global_q0;
extern ptr_t GET_extensions__clientId_live_activated_channels_p0, GET_extensions__clientId_live_activated_channels_q0;
extern ptr_t POST_extensions__clientId__version_channels__channelId_chat_p0, POST_extensions__clientId__version_channels__channelId_chat_q0, POST_extensions__clientId__version_channels__channelId_chat_p1, POST_extensions__clientId__version_channels__channelId_chat_q1, POST_extensions__clientId__version_channels__channelId_chat_p2, POST_extensions__clientId__version_channels__channelId_chat_q2;
extern ptr_t POST_extensions__clientId_auth_secret_p0, POST_extensions__clientId_auth_secret_q0;
extern ptr_t POST_extensions_message__channelId_p0, POST_extensions_message__channelId_q0;
extern ptr_t PUT_extensions__clientId__version_required_configuration_p0, PUT_extensions__clientId__version_required_configuration_q0, PUT_extensions__clientId__version_required_configuration_p1, PUT_extensions__clientId__version_required_configuration_q1;
extern ptr_t PUT_extensions__clientId_configurations_p0, PUT_extensions__clientId_configurations_q0;
