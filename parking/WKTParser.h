#pragma once

struct WKTPoint
{
	double x = 0.0;
	double y = 0.0;
};

using Ring = std::vector<WKTPoint>;

struct WKTPolygon
{
	std::vector<Ring> rings; // [0] = exterior, rest = holes
};

struct LineString
{
	std::vector<WKTPoint> WKTPoints;
};

enum class TokenType
{
	Keyword,
	Number,
	LParen,
	RParen,
	Comma,
	End
};

struct Token
{
	TokenType type;
	std::string text;
};

class Tokenizer
{
public:
	explicit Tokenizer(const std::string &s);

	Token next();
	Token peek();

private:
	const std::string &str;
	size_t pos = 0;

	void skipWhitespace();
};



class WKTParser
{
public:
	explicit WKTParser(const std::string &wkt);

	std::vector<WKTPolygon> m_polygons;
	std::vector<LineString> m_linestrings;

	void parse();

private:
	Tokenizer tokenizer;

	void expect(TokenType type);
	WKTPoint parsePoint();
	std::vector<WKTPoint> parsePointList();

	void parseLineString();
	void parsePolygon();
	void parseMultiLineString();
	void parseMultiPolygon();
	void parseGeometryCollection();
	void parseGeometry();
};
