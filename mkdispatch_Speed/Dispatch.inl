void TestDispatch(int count) {
	std::cout << "Testing " << name << std::endl;
	std::chrono::high_resolution_clock clock;
	TcpSocket client;
	char* next = nullptr;
	auto before = clock.now();
	for(int i = 0; i < count; ++i) {
		Dispatch("POST /extensions/clientId/auth/secret\r", nullptr, next, nullptr, client);
		Dispatch("GET /extensions/clientId/auth/secret\r", nullptr, next, nullptr, client);
		Dispatch("DELETE /extensions/clientId/auth/secret\r", nullptr, next, nullptr, client);
		Dispatch("GET /extensions/clientId/live_activated_channels\r", nullptr, next, nullptr, client);
		Dispatch("PUT /extensions/clientId/version/required_configuration\r", nullptr, next, nullptr, client);
		Dispatch("PUT /extensions/clientId/configurations\r", nullptr, next, nullptr, client);
		Dispatch("GET /extensions/clientId/configurations/channels/channelId\r", nullptr, next, nullptr, client);
		Dispatch("GET /extensions/clientId/configurations/segments/broadcaster\r", nullptr, next, nullptr, client);
		Dispatch("GET /extensions/clientId/configurations/segments/developer\r", nullptr, next, nullptr, client);
		Dispatch("GET /extensions/clientId/configurations/segments/global\r", nullptr, next, nullptr, client);
		Dispatch("POST /extensions/message/channelId\r", nullptr, next, nullptr, client);
		Dispatch("POST /extensions/message/all\r", nullptr, next, nullptr, client);
		Dispatch("POST /extensions/clientId/version/channels/channelId/chat\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/analytics/extensions\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/analytics/games\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/bits/leaderboard\r", nullptr, next, nullptr, client);
		Dispatch("POST /helix/clips\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/clips\r", nullptr, next, nullptr, client);
		Dispatch("POST /helix/entitlements/upload\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/entitlements/codes\r", nullptr, next, nullptr, client);
		Dispatch("POST /helix/entitlements/codes\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/games/top\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/games\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/streams\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/streams/metadata\r", nullptr, next, nullptr, client);
		Dispatch("POST /helix/streams/markers\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/streams/markers\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/users\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/users/follows\r", nullptr, next, nullptr, client);
		Dispatch("PUT /helix/users\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/users/extensions/list\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/users/extensions\r", nullptr, next, nullptr, client);
		Dispatch("PUT /helix/users/extensions\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/videos\r", nullptr, next, nullptr, client);
		Dispatch("GET /helix/webhooks/subscriptions\r", nullptr, next, nullptr, client);
	}
	auto after = clock.now();
	std::chrono::duration<double> duration = after - before;
	std::cout << "Duration:  " << duration.count() << std::endl;
	std::cout << "DELETE_extensions__clientId_auth_secret_count:  " << DELETE_extensions__clientId_auth_secret_count << std::endl;
	std::cout << "GET_extensions__clientId_auth_secret_count:  " << GET_extensions__clientId_auth_secret_count << std::endl;
	std::cout << "GET_extensions__clientId_configurations_channels__channelId_count:  " << GET_extensions__clientId_configurations_channels__channelId_count << std::endl;
	std::cout << "GET_extensions__clientId_configurations_segments_broadcaster_count:  " << GET_extensions__clientId_configurations_segments_broadcaster_count << std::endl;
	std::cout << "GET_extensions__clientId_configurations_segments_developer_count:  " << GET_extensions__clientId_configurations_segments_developer_count << std::endl;
	std::cout << "GET_extensions__clientId_configurations_segments_global_count:  " << GET_extensions__clientId_configurations_segments_global_count << std::endl;
	std::cout << "GET_extensions__clientId_live_activated_channels_count:  " << GET_extensions__clientId_live_activated_channels_count << std::endl;
	std::cout << "GET_helix_analytics_extensions_count:  " << GET_helix_analytics_extensions_count << std::endl;
	std::cout << "GET_helix_analytics_games_count:  " << GET_helix_analytics_games_count << std::endl;
	std::cout << "GET_helix_bits_leaderboard_count:  " << GET_helix_bits_leaderboard_count << std::endl;
	std::cout << "GET_helix_clips_count:  " << GET_helix_clips_count << std::endl;
	std::cout << "GET_helix_entitlements_codes_count:  " << GET_helix_entitlements_codes_count << std::endl;
	std::cout << "GET_helix_games_top_count:  " << GET_helix_games_top_count << std::endl;
	std::cout << "GET_helix_games_count:  " << GET_helix_games_count << std::endl;
	std::cout << "GET_helix_streams_markers_count:  " << GET_helix_streams_markers_count << std::endl;
	std::cout << "GET_helix_streams_metadata_count:  " << GET_helix_streams_metadata_count << std::endl;
	std::cout << "GET_helix_streams_count:  " << GET_helix_streams_count << std::endl;
	std::cout << "GET_helix_users_extensions_list_count:  " << GET_helix_users_extensions_list_count << std::endl;
	std::cout << "GET_helix_users_extensions_count:  " << GET_helix_users_extensions_count << std::endl;
	std::cout << "GET_helix_users_follows_count:  " << GET_helix_users_follows_count << std::endl;
	std::cout << "GET_helix_users_count:  " << GET_helix_users_count << std::endl;
	std::cout << "GET_helix_videos_count:  " << GET_helix_videos_count << std::endl;
	std::cout << "GET_helix_webhooks_subscriptions_count:  " << GET_helix_webhooks_subscriptions_count << std::endl;
	std::cout << "POST_extensions__clientId__version_channels__channelId_chat_count:  " << POST_extensions__clientId__version_channels__channelId_chat_count << std::endl;
	std::cout << "POST_extensions__clientId_auth_secret_count:  " << POST_extensions__clientId_auth_secret_count << std::endl;
	std::cout << "POST_extensions_message__channelId_count:  " << POST_extensions_message__channelId_count << std::endl;
	std::cout << "POST_extensions_message_all_count:  " << POST_extensions_message_all_count << std::endl;
	std::cout << "POST_helix_clips_count:  " << POST_helix_clips_count << std::endl;
	std::cout << "POST_helix_entitlements_codes_count:  " << POST_helix_entitlements_codes_count << std::endl;
	std::cout << "POST_helix_entitlements_upload_count:  " << POST_helix_entitlements_upload_count << std::endl;
	std::cout << "POST_helix_streams_markers_count:  " << POST_helix_streams_markers_count << std::endl;
	std::cout << "PUT_extensions__clientId__version_required_configuration_count:  " << PUT_extensions__clientId__version_required_configuration_count << std::endl;
	std::cout << "PUT_extensions__clientId_configurations_count:  " << PUT_extensions__clientId_configurations_count << std::endl;
	std::cout << "PUT_helix_users_extensions_count:  " << PUT_helix_users_extensions_count << std::endl;
	std::cout << "PUT_helix_users_count:  " << PUT_helix_users_count << std::endl;

	std::string DELETE_extensions__clientId_auth_secret_s0(DELETE_extensions__clientId_auth_secret_p0, DELETE_extensions__clientId_auth_secret_q0);
	if(DELETE_extensions__clientId_auth_secret_s0 != "clientId") {
		throw std::runtime_error("DELETE_extensions__clientId_auth_secret_s0");
	}
	std::string GET_extensions__clientId_auth_secret_s0(GET_extensions__clientId_auth_secret_p0, GET_extensions__clientId_auth_secret_q0);
	if(GET_extensions__clientId_auth_secret_s0 != "clientId") {
		throw std::runtime_error("GET_extensions__clientId_auth_secret_s0");
	}
	std::string GET_extensions__clientId_configurations_channels__channelId_s0(GET_extensions__clientId_configurations_channels__channelId_p0, GET_extensions__clientId_configurations_channels__channelId_q0);
	if(GET_extensions__clientId_configurations_channels__channelId_s0 != "clientId") {
		throw std::runtime_error("GET_extensions__clientId_configurations_channels__channelId_s0");
	}
	std::string GET_extensions__clientId_configurations_channels__channelId_s1(GET_extensions__clientId_configurations_channels__channelId_p1, GET_extensions__clientId_configurations_channels__channelId_q1);
	if(GET_extensions__clientId_configurations_channels__channelId_s1 != "channelId") {
		throw std::runtime_error("GET_extensions__clientId_configurations_channels__channelId_s1");
	}
	std::string GET_extensions__clientId_configurations_segments_broadcaster_s0(GET_extensions__clientId_configurations_segments_broadcaster_p0, GET_extensions__clientId_configurations_segments_broadcaster_q0);
	if(GET_extensions__clientId_configurations_segments_broadcaster_s0 != "clientId") {
		throw std::runtime_error("GET_extensions__clientId_configurations_segments_broadcaster_s0");
	}
	std::string GET_extensions__clientId_configurations_segments_developer_s0(GET_extensions__clientId_configurations_segments_developer_p0, GET_extensions__clientId_configurations_segments_developer_q0);
	if(GET_extensions__clientId_configurations_segments_developer_s0 != "clientId") {
		throw std::runtime_error("GET_extensions__clientId_configurations_segments_developer_s0");
	}
	std::string GET_extensions__clientId_configurations_segments_global_s0(GET_extensions__clientId_configurations_segments_global_p0, GET_extensions__clientId_configurations_segments_global_q0);
	if(GET_extensions__clientId_configurations_segments_global_s0 != "clientId") {
		throw std::runtime_error("GET_extensions__clientId_configurations_segments_global_s0");
	}
	std::string GET_extensions__clientId_live_activated_channels_s0(GET_extensions__clientId_live_activated_channels_p0, GET_extensions__clientId_live_activated_channels_q0);
	if(GET_extensions__clientId_live_activated_channels_s0 != "clientId") {
		throw std::runtime_error("GET_extensions__clientId_live_activated_channels_s0");
	}
	std::string POST_extensions__clientId__version_channels__channelId_chat_s0(POST_extensions__clientId__version_channels__channelId_chat_p0, POST_extensions__clientId__version_channels__channelId_chat_q0);
	if(POST_extensions__clientId__version_channels__channelId_chat_s0 != "clientId") {
		throw std::runtime_error("POST_extensions__clientId__version_channels__channelId_chat_s0");
	}
	std::string POST_extensions__clientId__version_channels__channelId_chat_s1(POST_extensions__clientId__version_channels__channelId_chat_p1, POST_extensions__clientId__version_channels__channelId_chat_q1);
	if(POST_extensions__clientId__version_channels__channelId_chat_s1 != "version") {
		throw std::runtime_error("POST_extensions__clientId__version_channels__channelId_chat_s1");
	}
	std::string POST_extensions__clientId__version_channels__channelId_chat_s2(POST_extensions__clientId__version_channels__channelId_chat_p2, POST_extensions__clientId__version_channels__channelId_chat_q2);
	if(POST_extensions__clientId__version_channels__channelId_chat_s2 != "channelId") {
		throw std::runtime_error("POST_extensions__clientId__version_channels__channelId_chat_s2");
	}
	std::string POST_extensions__clientId_auth_secret_s0(POST_extensions__clientId_auth_secret_p0, POST_extensions__clientId_auth_secret_q0);
	if(POST_extensions__clientId_auth_secret_s0 != "clientId") {
		throw std::runtime_error("POST_extensions__clientId_auth_secret_s0");
	}
	std::string POST_extensions_message__channelId_s0(POST_extensions_message__channelId_p0, POST_extensions_message__channelId_q0);
	if(POST_extensions_message__channelId_s0 != "channelId") {
		throw std::runtime_error("POST_extensions_message__channelId_s0");
	}
	std::string PUT_extensions__clientId__version_required_configuration_s0(PUT_extensions__clientId__version_required_configuration_p0, PUT_extensions__clientId__version_required_configuration_q0);
	if(PUT_extensions__clientId__version_required_configuration_s0 != "clientId") {
		throw std::runtime_error("PUT_extensions__clientId__version_required_configuration_s0");
	}
	std::string PUT_extensions__clientId__version_required_configuration_s1(PUT_extensions__clientId__version_required_configuration_p1, PUT_extensions__clientId__version_required_configuration_q1);
	if(PUT_extensions__clientId__version_required_configuration_s1 != "version") {
		throw std::runtime_error("PUT_extensions__clientId__version_required_configuration_s1");
	}
	std::string PUT_extensions__clientId_configurations_s0(PUT_extensions__clientId_configurations_p0, PUT_extensions__clientId_configurations_q0);
	if(PUT_extensions__clientId_configurations_s0 != "clientId") {
		throw std::runtime_error("PUT_extensions__clientId_configurations_s0");
	}
}
