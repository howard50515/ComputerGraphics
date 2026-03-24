#include "io/MeshLoader.h"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "core/Color.h"
#include "core/Triangle.h"
#include "core/Vertex.h"
#include "math/Vector3.h"

namespace hw1 {

namespace {

bool HasJsonExtension(const std::string& path) {
	if (path.size() < 5) {
		return false;
	}

	const std::string extension = path.substr(path.size() - 5);
	return extension == ".json" || extension == ".JSON";
}

size_t FindMatchingBracket(const std::string& text, size_t openPos) {
	int depth = 0;
	for (size_t i = openPos; i < text.size(); ++i) {
		if (text[i] == '[') {
			++depth;
		} else if (text[i] == ']') {
			--depth;
			if (depth == 0) {
				return i;
			}
		}
	}

	throw std::runtime_error("Malformed JSON array: missing closing ']'.");
}

std::vector<float> ParseFloatArray(const std::string& text, const std::string& key) {
	const std::string quotedKey = "\"" + key + "\"";
	const size_t keyPos = text.find(quotedKey);
	if (keyPos == std::string::npos) {
		return {};
	}

	const size_t arrayOpen = text.find('[', keyPos);
	if (arrayOpen == std::string::npos) {
		throw std::runtime_error("Malformed JSON: missing '[' after key '" + key + "'.");
	}

	const size_t arrayClose = FindMatchingBracket(text, arrayOpen);
	std::vector<float> values;

	const char* cursor = text.c_str() + arrayOpen + 1;
	const char* end = text.c_str() + arrayClose;

	while (cursor < end) {
		while (cursor < end && (std::isspace(static_cast<unsigned char>(*cursor)) || *cursor == ',')) {
			++cursor;
		}

		if (cursor >= end) {
			break;
		}

		char* parseEnd = nullptr;
		const float value = std::strtof(cursor, &parseEnd);
		if (parseEnd == cursor) {
			throw std::runtime_error("Invalid number in JSON array for key '" + key + "'.");
		}

		values.push_back(value);
		cursor = parseEnd;
	}

	return values;
}

std::vector<float> ParseFirstExistingFloatArray(const std::string& text, const std::vector<std::string>& keys) {
	for (const std::string& key : keys) {
		std::vector<float> values = ParseFloatArray(text, key);
		if (!values.empty()) {
			return values;
		}
	}

	return {};
}

Mesh LoadJsonMesh(const std::string& text, const std::string& path) {
	const std::vector<float> positions = ParseFirstExistingFloatArray(text, {"vertexPositions", "positions"});
	if (positions.empty()) {
		throw std::runtime_error("JSON mesh missing 'vertexPositions' array: " + path);
	}

	if (positions.size() % 9 != 0) {
		throw std::runtime_error("JSON mesh positions must be grouped in triangles (9 floats per triangle): " + path);
	}

	const std::vector<float> normals = ParseFirstExistingFloatArray(text, {"vertexNormals", "normals"});
	if (!normals.empty() && normals.size() != positions.size()) {
		throw std::runtime_error("JSON mesh normals count must match positions count: " + path);
	}

	Mesh mesh;
	for (size_t i = 0; i < positions.size(); i += 9) {
		Triangle triangle;
		triangle.frontColor = Color(1.0f, 1.0f, 1.0f);
		triangle.backColor = Color(1.0f, 1.0f, 1.0f);

		const Vector3 p0(positions[i + 0], positions[i + 1], positions[i + 2]);
		const Vector3 p1(positions[i + 3], positions[i + 4], positions[i + 5]);
		const Vector3 p2(positions[i + 6], positions[i + 7], positions[i + 8]);

		const Vector3 n0 = normals.empty() ? Vector3(0.0f, 0.0f, 1.0f) : Vector3(normals[i + 0], normals[i + 1], normals[i + 2]);
		const Vector3 n1 = normals.empty() ? Vector3(0.0f, 0.0f, 1.0f) : Vector3(normals[i + 3], normals[i + 4], normals[i + 5]);
		const Vector3 n2 = normals.empty() ? Vector3(0.0f, 0.0f, 1.0f) : Vector3(normals[i + 6], normals[i + 7], normals[i + 8]);

		triangle.v0 = Vertex(p0, n0);
		triangle.v1 = Vertex(p1, n1);
		triangle.v2 = Vertex(p2, n2);

		mesh.addTriangle(triangle);
	}

	return mesh;
}

}  // namespace

Mesh MeshLoader::loadFromFile(const std::string& path) {
	std::ifstream input(path, std::ios::binary);
	if (!input.is_open()) {
		throw std::runtime_error("Cannot open mesh file: " + path);
	}

	std::ostringstream buffer;
	buffer << input.rdbuf();
	const std::string content = buffer.str();

	if (!HasJsonExtension(path)) {
		throw std::runtime_error("Only .json mesh files are supported: " + path);
	}

	return LoadJsonMesh(content, path);
}

}  // namespace hw1
