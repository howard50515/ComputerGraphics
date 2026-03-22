#include "gui/WebServer.h"

#include <chrono>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

namespace hw1 {

namespace {

#ifdef _WIN32
bool sendAll(SOCKET socket, const std::uint8_t* data, std::size_t size) {
	std::size_t totalSent = 0;
	while (totalSent < size) {
		const int sent = send(socket, reinterpret_cast<const char*>(data + totalSent), static_cast<int>(size - totalSent), 0);
		if (sent <= 0) {
			return false;
		}
		totalSent += static_cast<std::size_t>(sent);
	}
	return true;
}
#endif

}  // namespace

WebServer::WebServer()
	: running_(false),
	  port_(0),
	  latestFps_(0.0f),
	  latestRenderMs_(0.0f),
	  latestGeometryMs_(0.0f),
	  latestRasterMs_(0.0f),
	  latestUploadMs_(0.0f),
	  latestEncodeMs_(0.0f),
	  shutdownRequested_(false) {}

WebServer::~WebServer() {
	stop();
}

void WebServer::start(std::uint16_t port) {
	if (running_) {
		return;
	}

	port_ = port;
	shutdownRequested_ = false;
	running_ = true;
	serverThread_ = std::thread(&WebServer::serverLoop, this);
}

void WebServer::stop() {
	if (!running_) {
		return;
	}

	running_ = false;
	if (serverThread_.joinable()) {
		serverThread_.join();
	}
}

void WebServer::setFrame(const Framebuffer& framebuffer) {
	std::lock_guard<std::mutex> lock(frameMutex_);
	encodeRgba(framebuffer, latestFrameRgba_);
}

void WebServer::setFps(float fps) {
	latestFps_.store(fps);
}

void WebServer::setRenderMs(float renderMs) {
	latestRenderMs_.store(renderMs);
}

void WebServer::setGeometryMs(float geometryMs) {
	latestGeometryMs_.store(geometryMs);
}

void WebServer::setRasterMs(float rasterMs) {
	latestRasterMs_.store(rasterMs);
}

void WebServer::setUploadMs(float uploadMs) {
	latestUploadMs_.store(uploadMs);
}

GUIWindow& WebServer::guiWindow() {
	return window_;
}

const GUIWindow& WebServer::guiWindow() const {
	return window_;
}

bool WebServer::shutdownRequested() const {
	return shutdownRequested_.load();
}

void WebServer::serverLoop() {
#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		running_ = false;
		return;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET) {
		WSACleanup();
		running_ = false;
		return;
	}

	u_long nonBlocking = 1;
	ioctlsocket(listenSocket, FIONBIO, &nonBlocking);

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port_);

	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR ||
		listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		closesocket(listenSocket);
		WSACleanup();
		running_ = false;
		return;
	}

	while (running_) {
		fd_set readSet;
		FD_ZERO(&readSet);
		FD_SET(listenSocket, &readSet);

		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;

		const int ready = select(0, &readSet, nullptr, nullptr, &timeout);
		if (ready <= 0) {
			continue;
		}

		SOCKET client = accept(listenSocket, nullptr, nullptr);
		if (client == INVALID_SOCKET) {
			continue;
		}

		u_long blocking = 0;
		ioctlsocket(client, FIONBIO, &blocking);

		handleClient(static_cast<std::uintptr_t>(client));
		closesocket(client);
	}

	closesocket(listenSocket);
	WSACleanup();
#else
	running_ = false;
#endif
}

void WebServer::handleClient(std::uintptr_t clientSocket) {
#ifdef _WIN32
	SOCKET client = static_cast<SOCKET>(clientSocket);

	char request[4096];
	int received = recv(client, request, sizeof(request) - 1, 0);
	if (received == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK) {
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
		received = recv(client, request, sizeof(request) - 1, 0);
	}
	if (received <= 0) {
		return;
	}
	request[received] = '\0';

	const std::string reqStr(request);
	const bool wantFrame = (reqStr.find("GET /frame.rgba") == 0);
	const bool wantFps = (reqStr.find("GET /fps") == 0);
	const bool wantPerf = (reqStr.find("GET /perf") == 0);
	const bool wantShading = (reqStr.find("GET /shading") == 0);
	const bool wantSetShading = (reqStr.find("GET /set_shading?mode=") == 0);
	const bool wantCamera = (reqStr.find("GET /camera") == 0);
	const bool wantSetCamera = (reqStr.find("GET /set_camera?index=") == 0);
	const bool wantObject = (reqStr.find("GET /object") == 0);
	const bool wantSetObjectScale = (reqStr.find("GET /set_object_scale?value=") == 0);
	const bool wantSetCameraPosition = (reqStr.find("GET /set_camera_position?index=") == 0);
	const bool wantSetLightPosition = (reqStr.find("GET /set_light_position?index=") == 0);
	const bool wantSetLightColor = (reqStr.find("GET /set_light_color?index=") == 0);
	const bool wantRoot = (reqStr.find("GET / ") == 0 || reqStr.find("GET /index.html") == 0);
	const bool wantShutdown = (reqStr.find("GET /shutdown") == 0);

	if (wantRoot) {
		const std::vector<std::uint8_t> html = window_.htmlPage();
		const std::string header =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html; charset=utf-8\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(html.size()) + "\r\n\r\n";

		sendAll(client, reinterpret_cast<const std::uint8_t*>(header.data()), header.size());
		sendAll(client, html.data(), html.size());
		return;
	}

	if (wantFrame) {
		std::vector<std::uint8_t> frame;
		{
			std::lock_guard<std::mutex> lock(frameMutex_);
			frame = latestFrameRgba_;
		}

		if (frame.empty()) {
			const std::string emptyResponse =
				"HTTP/1.1 503 Service Unavailable\r\n"
				"Connection: close\r\n"
				"Content-Length: 0\r\n\r\n";
			sendAll(client, reinterpret_cast<const std::uint8_t*>(emptyResponse.data()), emptyResponse.size());
			return;
		}

		const std::string header =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/octet-stream\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(frame.size()) + "\r\n\r\n";

		sendAll(client, reinterpret_cast<const std::uint8_t*>(header.data()), header.size());
		sendAll(client, frame.data(), frame.size());
		return;
	}

	if (wantFps) {
		char body[64];
		const int len = std::snprintf(body, sizeof(body), "{\"fps\":%.2f}", latestFps_.load());
		const std::string payload(body, body + ((len > 0) ? len : 0));
		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json; charset=utf-8\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(payload.size()) + "\r\n\r\n" + payload;
		sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
		return;
	}

	if (wantPerf) {
		char body[256];
		const int len = std::snprintf(
			body,
			sizeof(body),
			"{\"fps\":%.2f,\"renderMs\":%.3f,\"geometryMs\":%.3f,\"rasterMs\":%.3f,\"uploadMs\":%.3f,\"encodeMs\":%.3f}",
			latestFps_.load(),
			latestRenderMs_.load(),
			latestGeometryMs_.load(),
			latestRasterMs_.load(),
			latestUploadMs_.load(),
			latestEncodeMs_.load());
		const std::string payload(body, body + ((len > 0) ? len : 0));
		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json; charset=utf-8\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(payload.size()) + "\r\n\r\n" + payload;
		sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
		return;
	}

	if (wantShading) {
		const std::string payload = window_.shadingModeJson();
		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json; charset=utf-8\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(payload.size()) + "\r\n\r\n" + payload;
		sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
		return;
	}

	if (wantSetShading) {
		std::string payload;
		if (!window_.trySetShadingModeFromRequest(reqStr, payload)) {
			const std::string body = "Invalid shading mode.";
			const std::string response =
				"HTTP/1.1 400 Bad Request\r\n"
				"Content-Type: text/plain; charset=utf-8\r\n"
				"Connection: close\r\n"
				"Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
			sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
			return;
		}

		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json; charset=utf-8\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(payload.size()) + "\r\n\r\n" + payload;
		sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
		return;
	}

	if (wantCamera) {
		const std::string payload = window_.cameraJson();
		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json; charset=utf-8\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(payload.size()) + "\r\n\r\n" + payload;
		sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
		return;
	}

	if (wantSetCamera) {
		std::string payload;
		if (!window_.trySetCameraFromRequest(reqStr, payload)) {
			const std::string body = "Invalid camera index.";
			const std::string response =
				"HTTP/1.1 400 Bad Request\r\n"
				"Content-Type: text/plain; charset=utf-8\r\n"
				"Connection: close\r\n"
				"Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
			sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
			return;
		}

		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json; charset=utf-8\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(payload.size()) + "\r\n\r\n" + payload;
		sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
		return;
	}

	if (wantObject) {
		const std::string payload = window_.objectJson();
		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json; charset=utf-8\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(payload.size()) + "\r\n\r\n" + payload;
		sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
		return;
	}

	if (wantSetObjectScale) {
		std::string payload;
		if (!window_.trySetObjectScaleFromRequest(reqStr, payload)) {
			const std::string body = "Invalid object scale.";
			const std::string response =
				"HTTP/1.1 400 Bad Request\r\n"
				"Content-Type: text/plain; charset=utf-8\r\n"
				"Connection: close\r\n"
				"Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
			sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
			return;
		}

		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json; charset=utf-8\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(payload.size()) + "\r\n\r\n" + payload;
		sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
		return;
	}

	if (wantSetCameraPosition) {
		std::string payload;
		if (!window_.trySetCameraPositionFromRequest(reqStr, payload)) {
			const std::string body = "Invalid camera position.";
			const std::string response =
				"HTTP/1.1 400 Bad Request\r\n"
				"Content-Type: text/plain; charset=utf-8\r\n"
				"Connection: close\r\n"
				"Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
			sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
			return;
		}

		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json; charset=utf-8\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(payload.size()) + "\r\n\r\n" + payload;
		sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
		return;
	}

	if (wantSetLightPosition) {
		std::string payload;
		if (!window_.trySetLightPositionFromRequest(reqStr, payload)) {
			const std::string body = "Invalid light position.";
			const std::string response =
				"HTTP/1.1 400 Bad Request\r\n"
				"Content-Type: text/plain; charset=utf-8\r\n"
				"Connection: close\r\n"
				"Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
			sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
			return;
		}

		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json; charset=utf-8\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(payload.size()) + "\r\n\r\n" + payload;
		sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
		return;
	}

	if (wantSetLightColor) {
		std::string payload;
		if (!window_.trySetLightColorFromRequest(reqStr, payload)) {
			const std::string body = "Invalid light color.";
			const std::string response =
				"HTTP/1.1 400 Bad Request\r\n"
				"Content-Type: text/plain; charset=utf-8\r\n"
				"Connection: close\r\n"
				"Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
			sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
			return;
		}

		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json; charset=utf-8\r\n"
			"Cache-Control: no-store\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(payload.size()) + "\r\n\r\n" + payload;
		sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
		return;
	}

	if (wantShutdown) {
		shutdownRequested_ = true;
		const std::string body = "Shutdown requested.";
		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain; charset=utf-8\r\n"
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
		sendAll(client, reinterpret_cast<const std::uint8_t*>(response.data()), response.size());
		return;
	}

	const std::string notFound =
		"HTTP/1.1 404 Not Found\r\n"
		"Connection: close\r\n"
		"Content-Length: 0\r\n\r\n";
	sendAll(client, reinterpret_cast<const std::uint8_t*>(notFound.data()), notFound.size());
#else
	(void)clientSocket;
#endif
}

}  // namespace hw1
