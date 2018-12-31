#include "pch.h"
#include "MyPrinter.h"

int main(int argc, char* argv[]) {
	bool wantsStrings = argc > 1 && argv[1] == std::string("-s");

	// TODO:  read the requests.
	Printer::vector requests = {
		{ "POST /extensions/:clientId/auth/secret", "POST_extensions__clientId_auth_secret" },
		{ "GET /extensions/:clientId/auth/secret", "GET_extensions__clientId_auth_secret" },
		{ "DELETE /extensions/:clientId/auth/secret", "DELETE_extensions__clientId_auth_secret" },
		{ "GET /extensions/:clientId/live_activated_channels", "GET_extensions__clientId_live_activated_channels" },
		{ "PUT /extensions/:clientId/:version/required_configuration", "PUT_extensions__clientId__version_required_configuration" },
		{ "PUT /extensions/:clientId/configurations", "PUT_extensions__clientId_configurations" },
		{ "GET /extensions/:clientId/configurations/channels/:channelId", "GET_extensions__clientId_configurations_channels__channelId" },
		{ "GET /extensions/:clientId/configurations/segments/broadcaster", "GET_extensions__clientId_configurations_segments_broadcaster" },
		{ "GET /extensions/:clientId/configurations/segments/developer", "GET_extensions__clientId_configurations_segments_developer" },
		{ "GET /extensions/:clientId/configurations/segments/global", "GET_extensions__clientId_configurations_segments_global" },
		{ "POST /extensions/message/:channelId", "POST_extensions_message__channelId" },
		{ "POST /extensions/message/all", "POST_extensions_message_all" },
		{ "POST /extensions/:clientId/:version/channels/:channelId/chat", "POST_extensions__clientId__version_channels__channelId_chat" },
		{ "GET /helix/analytics/extensions", "GET_helix_analytics_extensions" },
		{ "GET /helix/analytics/games", "GET_helix_analytics_games" },
		{ "GET /helix/bits/leaderboard", "GET_helix_bits_leaderboard" },
		{ "POST /helix/clips", "POST_helix_clips" },
		{ "GET /helix/clips", "GET_helix_clips" },
		{ "POST /helix/entitlements/upload", "POST_helix_entitlements_upload" },
		{ "GET /helix/entitlements/codes", "GET_helix_entitlements_codes" },
		{ "POST /helix/entitlements/codes", "POST_helix_entitlements_codes" },
		{ "GET /helix/games/top", "GET_helix_games_top" },
		{ "GET /helix/games", "GET_helix_games" },
		{ "GET /helix/streams", "GET_helix_streams" },
		{ "GET /helix/streams/metadata", "GET_helix_streams_metadata" },
		{ "POST /helix/streams/markers", "POST_helix_streams_markers" },
		{ "GET /helix/streams/markers", "GET_helix_streams_markers" },
		{ "GET /helix/users", "GET_helix_users" },
		{ "GET /helix/users/follows", "GET_helix_users_follows" },
		{ "PUT /helix/users", "PUT_helix_users" },
		{ "GET /helix/users/extensions/list", "GET_helix_users_extensions_list" },
		{ "GET /helix/users/extensions", "GET_helix_users_extensions" },
		{ "PUT /helix/users/extensions", "PUT_helix_users_extensions" },
		{ "GET /helix/videos", "GET_helix_videos" },
		{ "GET /helix/webhooks/subscriptions", "GET_helix_webhooks_subscriptions" },
	};

	// Print the Dispatcher class.
	Options options = { wantsStrings };
	MyPrinter printer;
	(static_cast<Printer&>(printer)).Print(requests, options);
}
