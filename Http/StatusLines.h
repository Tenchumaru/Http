#pragma once

// https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
namespace StatusLines {
	using StatusLine = std::pair<char const*, size_t>;

	extern StatusLine Continue;
	extern StatusLine SwitchingProtocols;
	extern StatusLine Processing;
	extern StatusLine EarlyHints;
	extern StatusLine OK;
	extern StatusLine Created;
	extern StatusLine Accepted;
	extern StatusLine NonAuthoritativeInformation;
	extern StatusLine NoContent;
	extern StatusLine ResetContent;
	extern StatusLine PartialContent;
	extern StatusLine MultiStatus;
	extern StatusLine AlreadyReported;
	extern StatusLine IMUsed;
	extern StatusLine MultipleChoices;
	extern StatusLine MovedPermanently;
	extern StatusLine Found;
	extern StatusLine SeeOther;
	extern StatusLine NotModified;
	extern StatusLine UseProxy;
	extern StatusLine TemporaryRedirect;
	extern StatusLine PermanentRedirect;
	extern StatusLine BadRequest;
	extern StatusLine Unauthorized;
	extern StatusLine PaymentRequired;
	extern StatusLine Forbidden;
	extern StatusLine NotFound;
	extern StatusLine MethodNotAllowed;
	extern StatusLine NotAcceptable;
	extern StatusLine ProxyAuthenticationRequired;
	extern StatusLine RequestTimeout;
	extern StatusLine Conflict;
	extern StatusLine Gone;
	extern StatusLine LengthRequired;
	extern StatusLine PreconditionFailed;
	extern StatusLine PayloadTooLarge;
	extern StatusLine URITooLong;
	extern StatusLine UnsupportedMediaType;
	extern StatusLine RangeNotSatisfiable;
	extern StatusLine ExpectationFailed;
	extern StatusLine MisdirectedRequest;
	extern StatusLine UnprocessableEntity;
	extern StatusLine Locked;
	extern StatusLine FailedDependency;
	extern StatusLine UpgradeRequired;
	extern StatusLine PreconditionRequired;
	extern StatusLine TooManyRequests;
	extern StatusLine RequestHeaderFieldsTooLarge;
	extern StatusLine UnavailableForLegalReasons;
	extern StatusLine InternalServerError;
	extern StatusLine NotImplemented;
	extern StatusLine BadGateway;
	extern StatusLine ServiceUnavailable;
	extern StatusLine GatewayTimeout;
	extern StatusLine HTTPVersionNotSupported;
	extern StatusLine VariantAlsoNegotiates;
	extern StatusLine InsufficientStorage;
	extern StatusLine LoopDetected;
	extern StatusLine NotExtended;
	extern StatusLine NetworkAuthenticationRequired;
}
