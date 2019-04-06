#include "stdafx.h"
#include "StatusLines.h"

namespace {
	char constexpr Continue[]                      =  "HTTP/1.1 100 Continue\r\n";
	char constexpr SwitchingProtocols[]            =  "HTTP/1.1 101 Switching Protocols\r\n";
	char constexpr Processing[]                    =  "HTTP/1.1 102 Processing\r\n";
	char constexpr EarlyHints[]                    =  "HTTP/1.1 103 Early Hints\r\n";
	char constexpr OK[]                            =  "HTTP/1.1 200 OK\r\n";
	char constexpr Created[]                       =  "HTTP/1.1 201 Created\r\n";
	char constexpr Accepted[]                      =  "HTTP/1.1 202 Accepted\r\n";
	char constexpr NonAuthoritativeInformation[]   =  "HTTP/1.1 203 Non-Authoritative Information\r\n";
	char constexpr NoContent[]                     =  "HTTP/1.1 204 No Content\r\n";
	char constexpr ResetContent[]                  =  "HTTP/1.1 205 Reset Content\r\n";
	char constexpr PartialContent[]                =  "HTTP/1.1 206 Partial Content\r\n";
	char constexpr MultiStatus[]                   =  "HTTP/1.1 207 Multi-Status\r\n";
	char constexpr AlreadyReported[]               =  "HTTP/1.1 208 Already Reported\r\n";
	char constexpr IMUsed[]                        =  "HTTP/1.1 226 IM Used\r\n";
	char constexpr MultipleChoices[]               =  "HTTP/1.1 300 Multiple Choices\r\n";
	char constexpr MovedPermanently[]              =  "HTTP/1.1 301 Moved Permanently\r\n";
	char constexpr Found[]                         =  "HTTP/1.1 302 Found\r\n";
	char constexpr SeeOther[]                      =  "HTTP/1.1 303 See Other\r\n";
	char constexpr NotModified[]                   =  "HTTP/1.1 304 Not Modified\r\n";
	char constexpr UseProxy[]                      =  "HTTP/1.1 305 Use Proxy\r\n";
	char constexpr TemporaryRedirect[]             =  "HTTP/1.1 307 Temporary Redirect\r\n";
	char constexpr PermanentRedirect[]             =  "HTTP/1.1 308 Permanent Redirect\r\n";
	char constexpr BadRequest[]                    =  "HTTP/1.1 400 Bad Request\r\n";
	char constexpr Unauthorized[]                  =  "HTTP/1.1 401 Unauthorized\r\n";
	char constexpr PaymentRequired[]               =  "HTTP/1.1 402 Payment Required\r\n";
	char constexpr Forbidden[]                     =  "HTTP/1.1 403 Forbidden\r\n";
	char constexpr NotFound[]                      =  "HTTP/1.1 404 Not Found\r\n";
	char constexpr MethodNotAllowed[]              =  "HTTP/1.1 405 Method Not Allowed\r\n";
	char constexpr NotAcceptable[]                 =  "HTTP/1.1 406 Not Acceptable\r\n";
	char constexpr ProxyAuthenticationRequired[]   =  "HTTP/1.1 407 Proxy Authentication Required\r\n";
	char constexpr RequestTimeout[]                =  "HTTP/1.1 408 Request Timeout\r\n";
	char constexpr Conflict[]                      =  "HTTP/1.1 409 Conflict\r\n";
	char constexpr Gone[]                          =  "HTTP/1.1 410 Gone\r\n";
	char constexpr LengthRequired[]                =  "HTTP/1.1 411 Length Required\r\n";
	char constexpr PreconditionFailed[]            =  "HTTP/1.1 412 Precondition Failed\r\n";
	char constexpr PayloadTooLarge[]               =  "HTTP/1.1 413 Payload Too Large\r\n";
	char constexpr URITooLong[]                    =  "HTTP/1.1 414 URI Too Long\r\n";
	char constexpr UnsupportedMediaType[]          =  "HTTP/1.1 415 Unsupported Media Type\r\n";
	char constexpr RangeNotSatisfiable[]           =  "HTTP/1.1 416 Range Not Satisfiable\r\n";
	char constexpr ExpectationFailed[]             =  "HTTP/1.1 417 Expectation Failed\r\n";
	char constexpr MisdirectedRequest[]            =  "HTTP/1.1 421 Misdirected Request\r\n";
	char constexpr UnprocessableEntity[]           =  "HTTP/1.1 422 Unprocessable Entity\r\n";
	char constexpr Locked[]                        =  "HTTP/1.1 423 Locked\r\n";
	char constexpr FailedDependency[]              =  "HTTP/1.1 424 Failed Dependency\r\n";
	char constexpr UpgradeRequired[]               =  "HTTP/1.1 426 Upgrade Required\r\n";
	char constexpr PreconditionRequired[]          =  "HTTP/1.1 428 Precondition Required\r\n";
	char constexpr TooManyRequests[]               =  "HTTP/1.1 429 Too Many Requests\r\n";
	char constexpr RequestHeaderFieldsTooLarge[]   =  "HTTP/1.1 431 Request Header Fields Too Large\r\n";
	char constexpr UnavailableForLegalReasons[]    =  "HTTP/1.1 451 Unavailable For Legal Reasons\r\n";
	char constexpr InternalServerError[]           =  "HTTP/1.1 500 Internal Server Error\r\n";
	char constexpr NotImplemented[]                =  "HTTP/1.1 501 Not Implemented\r\n";
	char constexpr BadGateway[]                    =  "HTTP/1.1 502 Bad Gateway\r\n";
	char constexpr ServiceUnavailable[]            =  "HTTP/1.1 503 Service Unavailable\r\n";
	char constexpr GatewayTimeout[]                =  "HTTP/1.1 504 Gateway Timeout\r\n";
	char constexpr HTTPVersionNotSupported[]       =  "HTTP/1.1 505 HTTP Version Not Supported\r\n";
	char constexpr VariantAlsoNegotiates[]         =  "HTTP/1.1 506 Variant Also Negotiates\r\n";
	char constexpr InsufficientStorage[]           =  "HTTP/1.1 507 Insufficient Storage\r\n";
	char constexpr LoopDetected[]                  =  "HTTP/1.1 508 Loop Detected\r\n";
	char constexpr NotExtended[]                   =  "HTTP/1.1 510 Not Extended\r\n";
	char constexpr NetworkAuthenticationRequired[] =  "HTTP/1.1 511 Network Authentication Required\r\n";
}

namespace StatusLines {
	StatusLine Continue                      = { ::Continue, sizeof(::Continue) - 1 };
	StatusLine SwitchingProtocols            = { ::SwitchingProtocols, sizeof(::SwitchingProtocols) - 1 };
	StatusLine Processing                    = { ::Processing, sizeof(::Processing) - 1 };
	StatusLine EarlyHints                    = { ::EarlyHints, sizeof(::EarlyHints) - 1 };
	StatusLine OK                            = { ::OK, sizeof(::OK) - 1 };
	StatusLine Created                       = { ::Created, sizeof(::Created) - 1 };
	StatusLine Accepted                      = { ::Accepted, sizeof(::Accepted) - 1 };
	StatusLine NonAuthoritativeInformation   = { ::NonAuthoritativeInformation, sizeof(::NonAuthoritativeInformation) - 1 };
	StatusLine NoContent                     = { ::NoContent, sizeof(::NoContent) - 1 };
	StatusLine ResetContent                  = { ::ResetContent, sizeof(::ResetContent) - 1 };
	StatusLine PartialContent                = { ::PartialContent, sizeof(::PartialContent) - 1 };
	StatusLine MultiStatus                   = { ::MultiStatus, sizeof(::MultiStatus) - 1 };
	StatusLine AlreadyReported               = { ::AlreadyReported, sizeof(::AlreadyReported) - 1 };
	StatusLine IMUsed                        = { ::IMUsed, sizeof(::IMUsed) - 1 };
	StatusLine MultipleChoices               = { ::MultipleChoices, sizeof(::MultipleChoices) - 1 };
	StatusLine MovedPermanently              = { ::MovedPermanently, sizeof(::MovedPermanently) - 1 };
	StatusLine Found                         = { ::Found, sizeof(::Found) - 1 };
	StatusLine SeeOther                      = { ::SeeOther, sizeof(::SeeOther) - 1 };
	StatusLine NotModified                   = { ::NotModified, sizeof(::NotModified) - 1 };
	StatusLine UseProxy                      = { ::UseProxy, sizeof(::UseProxy) - 1 };
	StatusLine TemporaryRedirect             = { ::TemporaryRedirect, sizeof(::TemporaryRedirect) - 1 };
	StatusLine PermanentRedirect             = { ::PermanentRedirect, sizeof(::PermanentRedirect) - 1 };
	StatusLine BadRequest                    = { ::BadRequest, sizeof(::BadRequest) - 1 };
	StatusLine Unauthorized                  = { ::Unauthorized, sizeof(::Unauthorized) - 1 };
	StatusLine PaymentRequired               = { ::PaymentRequired, sizeof(::PaymentRequired) - 1 };
	StatusLine Forbidden                     = { ::Forbidden, sizeof(::Forbidden) - 1 };
	StatusLine NotFound                      = { ::NotFound, sizeof(::NotFound) - 1 };
	StatusLine MethodNotAllowed              = { ::MethodNotAllowed, sizeof(::MethodNotAllowed) - 1 };
	StatusLine NotAcceptable                 = { ::NotAcceptable, sizeof(::NotAcceptable) - 1 };
	StatusLine ProxyAuthenticationRequired   = { ::ProxyAuthenticationRequired, sizeof(::ProxyAuthenticationRequired) - 1 };
	StatusLine RequestTimeout                = { ::RequestTimeout, sizeof(::RequestTimeout) - 1 };
	StatusLine Conflict                      = { ::Conflict, sizeof(::Conflict) - 1 };
	StatusLine Gone                          = { ::Gone, sizeof(::Gone) - 1 };
	StatusLine LengthRequired                = { ::LengthRequired, sizeof(::LengthRequired) - 1 };
	StatusLine PreconditionFailed            = { ::PreconditionFailed, sizeof(::PreconditionFailed) - 1 };
	StatusLine PayloadTooLarge               = { ::PayloadTooLarge, sizeof(::PayloadTooLarge) - 1 };
	StatusLine URITooLong                    = { ::URITooLong, sizeof(::URITooLong) - 1 };
	StatusLine UnsupportedMediaType          = { ::UnsupportedMediaType, sizeof(::UnsupportedMediaType) - 1 };
	StatusLine RangeNotSatisfiable           = { ::RangeNotSatisfiable, sizeof(::RangeNotSatisfiable) - 1 };
	StatusLine ExpectationFailed             = { ::ExpectationFailed, sizeof(::ExpectationFailed) - 1 };
	StatusLine MisdirectedRequest            = { ::MisdirectedRequest, sizeof(::MisdirectedRequest) - 1 };
	StatusLine UnprocessableEntity           = { ::UnprocessableEntity, sizeof(::UnprocessableEntity) - 1 };
	StatusLine Locked                        = { ::Locked, sizeof(::Locked) - 1 };
	StatusLine FailedDependency              = { ::FailedDependency, sizeof(::FailedDependency) - 1 };
	StatusLine UpgradeRequired               = { ::UpgradeRequired, sizeof(::UpgradeRequired) - 1 };
	StatusLine PreconditionRequired          = { ::PreconditionRequired, sizeof(::PreconditionRequired) - 1 };
	StatusLine TooManyRequests               = { ::TooManyRequests, sizeof(::TooManyRequests) - 1 };
	StatusLine RequestHeaderFieldsTooLarge   = { ::RequestHeaderFieldsTooLarge, sizeof(::RequestHeaderFieldsTooLarge) - 1 };
	StatusLine UnavailableForLegalReasons    = { ::UnavailableForLegalReasons, sizeof(::UnavailableForLegalReasons) - 1 };
	StatusLine InternalServerError           = { ::InternalServerError, sizeof(::InternalServerError) - 1 };
	StatusLine NotImplemented                = { ::NotImplemented, sizeof(::NotImplemented) - 1 };
	StatusLine BadGateway                    = { ::BadGateway, sizeof(::BadGateway) - 1 };
	StatusLine ServiceUnavailable            = { ::ServiceUnavailable, sizeof(::ServiceUnavailable) - 1 };
	StatusLine GatewayTimeout                = { ::GatewayTimeout, sizeof(::GatewayTimeout) - 1 };
	StatusLine HTTPVersionNotSupported       = { ::HTTPVersionNotSupported, sizeof(::HTTPVersionNotSupported) - 1 };
	StatusLine VariantAlsoNegotiates         = { ::VariantAlsoNegotiates, sizeof(::VariantAlsoNegotiates) - 1 };
	StatusLine InsufficientStorage           = { ::InsufficientStorage, sizeof(::InsufficientStorage) - 1 };
	StatusLine LoopDetected                  = { ::LoopDetected, sizeof(::LoopDetected) - 1 };
	StatusLine NotExtended                   = { ::NotExtended, sizeof(::NotExtended) - 1 };
	StatusLine NetworkAuthenticationRequired = { ::NetworkAuthenticationRequired, sizeof(::NetworkAuthenticationRequired) - 1 };
}
